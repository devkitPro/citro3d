#include "internal.h"
#include <c3d/base.h>
#include <c3d/renderqueue.h>
#include <stdlib.h>

static C3D_RenderTarget *firstTarget, *lastTarget;
static C3D_RenderTarget *linkedTarget[3];

static TickCounter gpuTime, cpuTime;

#define STAGE_HAS_TRANSFER(n)   BIT(0+(n))
#define STAGE_HAS_ANY_TRANSFER  (7<<0)
#define STAGE_NEED_TRANSFER(n)  BIT(3+(n))
#define STAGE_NEED_TOP_TRANSFER (STAGE_NEED_TRANSFER(0)|STAGE_NEED_TRANSFER(1))
#define STAGE_NEED_BOT_TRANSFER STAGE_NEED_TRANSFER(2)
#define STAGE_WAIT_TRANSFER     BIT(6)

static bool initialized;
static bool inFrame, inSafeTransfer, measureGpuTime;
static u8 frameStage;
static float framerate = 60.0f;
static float framerateCounter[2] = { 60.0f, 60.0f };
static u32 frameCounter[2];
static void (* frameEndCb)(void*);
static void* frameEndCbData;

static bool framerateLimit(int id)
{
	framerateCounter[id] -= framerate;
	if (framerateCounter[id] <= 0.0f)
	{
		framerateCounter[id] += 60.0f;
		return true;
	}
	return false;
}

static void onVBlank0(C3D_UNUSED void* unused)
{
	if (frameStage & STAGE_NEED_TOP_TRANSFER)
	{
		C3D_RenderTarget *left = linkedTarget[0], *right = linkedTarget[1];
		if (left && !(frameStage&STAGE_NEED_TRANSFER(0)))
			left = NULL;
		if (right && !(frameStage&STAGE_NEED_TRANSFER(1)))
			right = NULL;
		if (gfxIs3D() && !right)
			right = left;

		frameStage &= ~STAGE_NEED_TOP_TRANSFER;
		if (left || right)
		{
			frameStage |= STAGE_WAIT_TRANSFER;
			if (left)
				C3D_FrameBufTransfer(&left->frameBuf, GFX_TOP, GFX_LEFT, left->transferFlags);
			if (right)
				C3D_FrameBufTransfer(&right->frameBuf, GFX_TOP, GFX_RIGHT, right->transferFlags);
			gfxConfigScreen(GFX_TOP, false);
		}
	}
	if (framerateLimit(0))
		frameCounter[0]++;
}

static void onVBlank1(C3D_UNUSED void* unused)
{
	if (frameStage & STAGE_NEED_BOT_TRANSFER)
	{
		frameStage &= ~STAGE_NEED_BOT_TRANSFER;
		C3D_RenderTarget* target = linkedTarget[2];
		if (target)
		{
			frameStage |= STAGE_WAIT_TRANSFER;
			C3D_FrameBufTransfer(&target->frameBuf, GFX_BOTTOM, GFX_LEFT, target->transferFlags);
			gfxConfigScreen(GFX_BOTTOM, false);
		}
	}
	if (framerateLimit(1))
		frameCounter[1]++;
}

static void onQueueFinish(gxCmdQueue_s* queue)
{
	if (measureGpuTime)
	{
		osTickCounterUpdate(&gpuTime);
		measureGpuTime = false;
	}
	if (inSafeTransfer)
	{
		inSafeTransfer = false;
		if (inFrame)
		{
			gxCmdQueueStop(queue);
			gxCmdQueueClear(queue);
		}
	}
	else if (frameStage & STAGE_WAIT_TRANSFER)
		frameStage &= ~STAGE_WAIT_TRANSFER;
	else
	{
		u8 needs = frameStage & STAGE_HAS_ANY_TRANSFER;
		frameStage = (frameStage&~STAGE_HAS_ANY_TRANSFER) | (needs<<3);
	}
}

void C3D_FrameSync(void)
{
	u32 cur[2];
	u32 start[2] = { frameCounter[0], frameCounter[1] };
	do
	{
		gspWaitForAnyEvent();
		cur[0] = frameCounter[0];
		cur[1] = frameCounter[1];
	} while (cur[0]==start[0] || cur[1]==start[1]);
}

u32 C3D_FrameCounter(int id)
{
	return frameCounter[id];
}

static bool C3Di_WaitAndClearQueue(s64 timeout)
{
	gxCmdQueue_s* queue = &C3Di_GetContext()->gxQueue;
	if (!gxCmdQueueWait(queue, timeout))
		return false;
	if (timeout==0 && frameStage)
		return false;
	while (frameStage)
		gspWaitForAnyEvent();
	gxCmdQueueStop(queue);
	gxCmdQueueClear(queue);
	return true;
}

static void C3Di_RenderQueueInit(void)
{
	gspSetEventCallback(GSPGPU_EVENT_VBlank0, onVBlank0, NULL, false);
	gspSetEventCallback(GSPGPU_EVENT_VBlank1, onVBlank1, NULL, false);
	gxCmdQueueSetCallback(&C3Di_GetContext()->gxQueue, onQueueFinish, NULL);
}

static void C3Di_RenderTargetDestroy(C3D_RenderTarget* target);

void C3Di_RenderQueueExit(void)
{
	int i;
	C3D_RenderTarget *a, *next;

	if (!initialized)
		return;

	C3Di_WaitAndClearQueue(-1);
	for (a = firstTarget; a; a = next)
	{
		next = a->next;
		C3Di_RenderTargetDestroy(a);
	}

	gspSetEventCallback(GSPGPU_EVENT_VBlank0, NULL, NULL, false);
	gspSetEventCallback(GSPGPU_EVENT_VBlank1, NULL, NULL, false);
	gxCmdQueueSetCallback(&C3Di_GetContext()->gxQueue, NULL, NULL);

	for (i = 0; i < 3; i ++)
		linkedTarget[i] = NULL;

	initialized = false;
}

void C3Di_RenderQueueWaitDone(void)
{
	if (!initialized)
		return;
	C3Di_WaitAndClearQueue(-1);
}

static bool checkRenderQueueInit(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return false;

	if (!initialized)
	{
		C3Di_RenderQueueInit();
		initialized = true;
	}

	return true;
}

float C3D_FrameRate(float fps)
{
	float old = framerate;
	if (fps > 0.0f && fps <= 60.0f)
	{
		framerate = fps;
		framerateCounter[0] = fps;
		framerateCounter[1] = fps;
	}
	return old;
}

bool C3D_FrameBegin(u8 flags)
{
	if (inFrame) return false;
	if (flags & C3D_FRAME_SYNCDRAW)
		C3D_FrameSync();
	if (!C3Di_WaitAndClearQueue((flags & C3D_FRAME_NONBLOCK) ? 0 : -1))
		return false;
	inFrame = true;
	osTickCounterStart(&cpuTime);
	return true;
}

bool C3D_FrameDrawOn(C3D_RenderTarget* target)
{
	if (!inFrame) return false;

	target->used = true;
	C3D_SetFrameBuf(&target->frameBuf);
	C3D_SetViewport(0, 0, target->frameBuf.width, target->frameBuf.height);
	return true;
}

void C3D_FrameSplit(u8 flags)
{
	u32 *cmdBuf, cmdBufSize;
	if (!inFrame) return;
	if (C3Di_SplitFrame(&cmdBuf, &cmdBufSize))
		GX_ProcessCommandList(cmdBuf, cmdBufSize*4, flags);
}

void C3D_FrameEnd(u8 flags)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (frameEndCb)
		frameEndCb(frameEndCbData);

	C3D_FrameSplit(flags);
	inFrame = false;
	osTickCounterUpdate(&cpuTime);

	// Flush the entire linear memory if the user did not explicitly mandate to flush the command list
	if (!(flags & GX_CMDLIST_FLUSH))
	{
		extern u32 __ctru_linear_heap;
		extern u32 __ctru_linear_heap_size;
		GSPGPU_FlushDataCache((void*)__ctru_linear_heap, __ctru_linear_heap_size);
	}

	int i;
	C3D_RenderTarget* target;
	for (i = 2; i >= 0; i --)
	{
		target = linkedTarget[i];
		if (!target || !target->used)
			continue;
		target->used = false;
		frameStage |= STAGE_HAS_TRANSFER(i);
	}

	GPUCMD_SetBuffer(ctx->cmdBuf, ctx->cmdBufSize, 0);
	measureGpuTime = true;
	osTickCounterStart(&gpuTime);
	gxCmdQueueRun(&ctx->gxQueue);
}

void C3D_FrameEndHook(void (* hook)(void*), void* param)
{
	frameEndCb = hook;
	frameEndCbData = param;
}

float C3D_GetDrawingTime(void)
{
	return osTickCounterRead(&gpuTime);
}

float C3D_GetProcessingTime(void)
{
	return osTickCounterRead(&cpuTime);
}

static C3D_RenderTarget* C3Di_RenderTargetNew(void)
{
	C3D_RenderTarget* target = (C3D_RenderTarget*)malloc(sizeof(C3D_RenderTarget));
	if (!target) return NULL;
	memset(target, 0, sizeof(C3D_RenderTarget));
	return target;
}

static void C3Di_RenderTargetFinishInit(C3D_RenderTarget* target)
{
	target->prev = lastTarget;
	target->next = NULL;
	if (lastTarget)
		lastTarget->next = target;
	if (!firstTarget)
		firstTarget = target;
	lastTarget = target;
}

C3D_RenderTarget* C3D_RenderTargetCreate(int width, int height, GPU_COLORBUF colorFmt, C3D_DEPTHTYPE depthFmt)
{
	if (!checkRenderQueueInit()) goto _fail0;

	GPU_DEPTHBUF depthFmtReal = GPU_RB_DEPTH16;
	void* depthBuf = NULL;
	void* colorBuf = vramAlloc(C3D_CalcColorBufSize(width,height,colorFmt));
	if (!colorBuf) goto _fail0;
	if (C3D_DEPTHTYPE_OK(depthFmt))
	{
		depthFmtReal = C3D_DEPTHTYPE_VAL(depthFmt);
		depthBuf = vramAlloc(C3D_CalcDepthBufSize(width,height,depthFmtReal));
		if (!depthBuf) goto _fail1;
	}

	C3D_RenderTarget* target = C3Di_RenderTargetNew();
	if (!target) goto _fail2;

	C3D_FrameBuf* fb = &target->frameBuf;
	C3D_FrameBufAttrib(fb, width, height, false);
	C3D_FrameBufColor(fb, colorBuf, colorFmt);
	target->ownsColor = true;
	if (depthBuf)
	{
		C3D_FrameBufDepth(fb, depthBuf, depthFmtReal);
		target->ownsDepth = true;
	}
	C3Di_RenderTargetFinishInit(target);
	return target;

_fail2:
	if (depthBuf) vramFree(depthBuf);
_fail1:
	vramFree(colorBuf);
_fail0:
	return NULL;
}

C3D_RenderTarget* C3D_RenderTargetCreateFromTex(C3D_Tex* tex, GPU_TEXFACE face, int level, C3D_DEPTHTYPE depthFmt)
{
	if (!checkRenderQueueInit()) return NULL;

	C3D_RenderTarget* target = C3Di_RenderTargetNew();
	if (!target) return NULL;

	C3D_FrameBuf* fb = &target->frameBuf;
	C3D_FrameBufTex(fb, tex, face, level);

	if (C3D_DEPTHTYPE_OK(depthFmt))
	{
		GPU_DEPTHBUF depthFmtReal = C3D_DEPTHTYPE_VAL(depthFmt);
		void* depthBuf = vramAlloc(C3D_CalcDepthBufSize(fb->width,fb->height,depthFmtReal));
		if (!depthBuf)
		{
			free(target);
			return NULL;
		}

		C3D_FrameBufDepth(fb, depthBuf, depthFmtReal);
		target->ownsDepth = true;
	}

	C3Di_RenderTargetFinishInit(target);
	return target;
}

void C3Di_RenderTargetDestroy(C3D_RenderTarget* target)
{
	if (target->ownsColor)
		vramFree(target->frameBuf.colorBuf);
	if (target->ownsDepth)
		vramFree(target->frameBuf.depthBuf);

	C3D_RenderTarget** prevNext = target->prev ? &target->prev->next : &firstTarget;
	C3D_RenderTarget** nextPrev = target->next ? &target->next->prev : &lastTarget;
	*prevNext = target->next;
	*nextPrev = target->prev;
	free(target);
}

void C3D_RenderTargetDelete(C3D_RenderTarget* target)
{
	if (inFrame)
		svcBreak(USERBREAK_PANIC); // Shouldn't happen.
	C3Di_WaitAndClearQueue(-1);
	C3Di_RenderTargetDestroy(target);
}

void C3D_RenderTargetSetOutput(C3D_RenderTarget* target, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags)
{
	int id = 0;
	if (screen==GFX_BOTTOM) id = 2;
	else if (side==GFX_RIGHT) id = 1;
	if (linkedTarget[id])
		linkedTarget[id]->linked = false;
	linkedTarget[id] = target;
	target->linked = true;
	target->transferFlags = transferFlags;
	target->screen = screen;
	target->side = side;
}

static void C3Di_SafeDisplayTransfer(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 flags)
{
	C3Di_WaitAndClearQueue(-1);
	inSafeTransfer = true;
	GX_DisplayTransfer(inadr, indim, outadr, outdim, flags);
	gxCmdQueueRun(&C3Di_GetContext()->gxQueue);
}

static void C3Di_SafeTextureCopy(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 size, u32 flags)
{
	C3Di_WaitAndClearQueue(-1);
	inSafeTransfer = true;
	GX_TextureCopy(inadr, indim, outadr, outdim, size, flags);
	gxCmdQueueRun(&C3Di_GetContext()->gxQueue);
}

static void C3Di_SafeMemoryFill(u32* buf0a, u32 buf0v, u32* buf0e, u16 control0, u32* buf1a, u32 buf1v, u32* buf1e, u16 control1)
{
	C3Di_WaitAndClearQueue(-1);
	inSafeTransfer = true;
	GX_MemoryFill(buf0a, buf0v, buf0e, control0, buf1a, buf1v, buf1e, control1);
	gxCmdQueueRun(&C3Di_GetContext()->gxQueue);
}

void C3D_SyncDisplayTransfer(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 flags)
{
	if (inFrame)
	{
		C3D_FrameSplit(0);
		GX_DisplayTransfer(inadr, indim, outadr, outdim, flags);
	} else
	{
		C3Di_SafeDisplayTransfer(inadr, indim, outadr, outdim, flags);
		gspWaitForPPF();
	}
}

void C3D_SyncTextureCopy(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 size, u32 flags)
{
	if (inFrame)
	{
		C3D_FrameSplit(0);
		GX_TextureCopy(inadr, indim, outadr, outdim, size, flags);
	} else
	{
		C3Di_SafeTextureCopy(inadr, indim, outadr, outdim, size, flags);
		gspWaitForPPF();
	}
}

void C3D_SyncMemoryFill(u32* buf0a, u32 buf0v, u32* buf0e, u16 control0, u32* buf1a, u32 buf1v, u32* buf1e, u16 control1)
{
	if (inFrame)
	{
		C3D_FrameSplit(0);
		GX_MemoryFill(buf0a, buf0v, buf0e, control0, buf1a, buf1v, buf1e, control1);
	} else
	{
		C3Di_SafeMemoryFill(buf0a, buf0v, buf0e, control0, buf1a, buf1v, buf1e, control1);
		gspWaitForPSC0();
	}
}
