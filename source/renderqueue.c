#include "internal.h"
#include <c3d/base.h>
#include <c3d/renderqueue.h>
#include <stdlib.h>

static const u8 colorFmtSizes[] = {2,1,0,0,0};
static const u8 depthFmtSizes[] = {0,0,1,2};

static C3D_RenderTarget *firstTarget, *lastTarget;
static C3D_RenderTarget *linkedTarget[3];
static C3D_RenderTarget *transferQueue, *clearQueue;

static TickCounter gpuTime, cpuTime;

static struct
{
	C3D_RenderTarget* targetList;
	u32* cmdBuf;
	u32 cmdBufSize;
	u8 flags;
} queuedFrame[2];
static u8 queueSwap, queuedCount, queuedState;

static bool inFrame, inSafeTransfer, inSafeClear;
static float framerate = 60.0f;
static float framerateCounter[2] = { 60.0f, 60.0f };

static void onRenderFinish(void* unused);
static void onTransferFinish(void* unused);
static void onClearDone(void* unused);

static void performDraw(void)
{
	gspSetEventCallback(GSPGPU_EVENT_P3D, onRenderFinish, NULL, true);
	GX_ProcessCommandList(queuedFrame[queueSwap].cmdBuf, queuedFrame[queueSwap].cmdBufSize, queuedFrame[queueSwap].flags);
	osTickCounterStart(&gpuTime);
}

static void performTransfer(void)
{
	if (inSafeTransfer) return; // Let the safe transfer finish handler retry this
	C3D_FrameBuf* frameBuf = &transferQueue->frameBuf;
	u32* outputFrameBuf = (u32*)gfxGetFramebuffer(transferQueue->screen, transferQueue->side, NULL, NULL);
	if (transferQueue->side == GFX_LEFT)
		gfxConfigScreen(transferQueue->screen, false);

	u32 dim = GX_BUFFER_DIM((u32)frameBuf->width, (u32)frameBuf->height);
	gspSetEventCallback(GSPGPU_EVENT_PPF, onTransferFinish, NULL, true);
	GX_DisplayTransfer((u32*)frameBuf->colorBuf, dim, outputFrameBuf, dim, transferQueue->transferFlags);
}

static void performClear(void)
{
	if (inSafeClear) return; // Let the safe clear finish handler retry this
	C3D_RenderTarget* target = clearQueue;
	while (target && !target->clearBits)
	{
		target->drawOk = true;
		target = target->link;
		clearQueue = target;
	}
	if (!target) return;

	C3D_FrameBuf* frameBuf = &target->frameBuf;
	u32 size = (u32)frameBuf->width * frameBuf->height;
	u32 cfs = colorFmtSizes[frameBuf->colorFmt];
	u32 dfs = depthFmtSizes[frameBuf->depthFmt];
	void* colorBufEnd = (u8*)frameBuf->colorBuf + size*(2+cfs);
	void* depthBufEnd = (u8*)frameBuf->depthBuf + size*(2+dfs);

	gspSetEventCallback(GSPGPU_EVENT_PSC0, onClearDone, NULL, true);
	if (target->clearBits & C3D_CLEAR_COLOR)
	{
		if (target->clearBits & C3D_CLEAR_DEPTH)
			GX_MemoryFill(
				(u32*)frameBuf->colorBuf, target->clearColor, (u32*)colorBufEnd, BIT(0) | (cfs << 8),
				(u32*)frameBuf->depthBuf, target->clearDepth, (u32*)depthBufEnd, BIT(0) | (dfs << 8));
		else
			GX_MemoryFill(
				(u32*)frameBuf->colorBuf, target->clearColor, (u32*)colorBufEnd, BIT(0) | (cfs << 8),
				NULL, 0, NULL, 0);
	} else
		GX_MemoryFill(
			(u32*)frameBuf->depthBuf, target->clearDepth, (u32*)depthBufEnd, BIT(0) | (dfs << 8),
			NULL, 0, NULL, 0);
}

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

static void updateFrameQueue(void)
{
	C3D_RenderTarget* a;
	if (queuedState>0) return; // Still rendering

	// Check that all targets are OK to be drawn on
	for (a = queuedFrame[queueSwap].targetList; a; a = a->frame[queueSwap])
		if (!a->drawOk)
			return; // Nope, we can't start rendering yet

	// Start rendering the frame
	queuedState=1;
	for (a = queuedFrame[queueSwap].targetList; a; a = a->frame[queueSwap])
		a->drawOk = false;
	performDraw();
}

static void transferTarget(C3D_RenderTarget* target)
{
	C3D_RenderTarget* a;
	target->transferOk = false;
	target->link = NULL;
	if (!transferQueue)
	{
		transferQueue = target;
		performTransfer();
		return;
	}
	for (a = transferQueue; a->link; a = a->link);
	a->link = target;
}

static void clearTarget(C3D_RenderTarget* target)
{
	C3D_RenderTarget* a;
	target->link = NULL;
	if (!clearQueue)
	{
		clearQueue = target;
		performClear();
		return;
	}
	for (a = clearQueue; a->link; a = a->link);
	a->link = target;
}

static void onVBlank0(C3D_UNUSED void* unused)
{
	if (!linkedTarget[0] || !framerateLimit(0)) return;

	if (gfxIs3D())
	{
		if (linkedTarget[1] && linkedTarget[1]->transferOk)
			transferTarget(linkedTarget[1]);
		else if (linkedTarget[0]->transferOk)
		{
			// Use a temporary copy of the left framebuffer to fill in the missing right image.
			static C3D_RenderTarget temp;
			memcpy(&temp, linkedTarget[0], sizeof(temp));
			temp.side = GFX_RIGHT;
			temp.clearBits = false;
			transferTarget(&temp);
		}
	}
	if (linkedTarget[0]->transferOk)
		transferTarget(linkedTarget[0]);
}

static void onVBlank1(C3D_UNUSED void* unused)
{
	if (linkedTarget[2] && framerateLimit(1) && linkedTarget[2]->transferOk)
		transferTarget(linkedTarget[2]);
}

void onRenderFinish(C3D_UNUSED void* unused)
{
	C3D_RenderTarget *a, *next;
	osTickCounterUpdate(&gpuTime);

	// The following check should never trigger
	if (queuedState!=1) svcBreak(USERBREAK_PANIC);

	for (a = queuedFrame[queueSwap].targetList; a; a = next)
	{
		next = a->frame[queueSwap];
		a->frame[queueSwap] = NULL;
		if (a->linked)
			a->transferOk = true;
		else if (a->clearBits)
			clearTarget(a);
		else
			a->drawOk = true;
	}

	// Consume the frame that has been just rendered
	memset(&queuedFrame[queueSwap], 0, sizeof(queuedFrame[queueSwap]));
	queueSwap ^= 1;
	queuedCount--;
	queuedState = 0;

	// Update the frame queue if there are still frames to render
	if (queuedCount>0)
		updateFrameQueue();
}

void onTransferFinish(C3D_UNUSED void* unused)
{
	C3D_RenderTarget* target = transferQueue;
	if (inSafeTransfer)
	{
		inSafeTransfer = false;
		// Try again if there are queued transfers
		if (target)
			performTransfer();
		return;
	}
	transferQueue = target->link;
	if (target->clearBits)
		clearTarget(target);
	else
		target->drawOk = true;
	if (transferQueue)
		performTransfer();
	if (target->drawOk && queuedCount>0 && queuedState==0)
		updateFrameQueue();
}

void onClearDone(C3D_UNUSED void* unused)
{
	C3D_RenderTarget* target = clearQueue;
	if (inSafeClear)
	{
		inSafeClear = false;
		// Try again if there are queued clears
		if (target)
			performClear();
		return;
	}
	clearQueue = target->link;
	target->drawOk = true;
	if (clearQueue)
		performClear();
	if (queuedCount>0 && queuedState==0)
		updateFrameQueue();
}

static void C3Di_RenderQueueInit(void)
{
	gspSetEventCallback(GSPGPU_EVENT_VBlank0, onVBlank0, NULL, false);
	gspSetEventCallback(GSPGPU_EVENT_VBlank1, onVBlank1, NULL, false);
}

static void C3Di_RenderQueueExit(void)
{
	int i;
	C3D_RenderTarget *a, *next;

	for (a = firstTarget; a; a = next)
	{
		next = a->next;
		C3D_RenderTargetDelete(a);
	}

	gspSetEventCallback(GSPGPU_EVENT_VBlank0, NULL, NULL, false);
	gspSetEventCallback(GSPGPU_EVENT_VBlank1, NULL, NULL, false);

	for (i = 0; i < 3; i ++)
		linkedTarget[i] = NULL;

	memset(queuedFrame, 0, sizeof(queuedFrame));
	queueSwap = 0;
	queuedCount = 0;
	queuedState = 0;
}

static void C3Di_RenderQueueWaitDone(void)
{
	while (queuedCount || transferQueue || clearQueue)
		gspWaitForAnyEvent();
}

static bool checkRenderQueueInit(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return false;

	if (!ctx->renderQueueExit)
	{
		C3Di_RenderQueueInit();
		ctx->renderQueueWaitDone = C3Di_RenderQueueWaitDone;
		ctx->renderQueueExit = C3Di_RenderQueueExit;
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
	int maxCount = (flags & C3D_FRAME_SYNCDRAW) ? 1 : 2;
	while (queuedCount >= maxCount)
	{
		if (flags & C3D_FRAME_NONBLOCK)
			return false;
		gspWaitForP3D();
	}
	osTickCounterStart(&cpuTime);
	inFrame = true;
	return true;
}

bool C3D_FrameDrawOn(C3D_RenderTarget* target)
{
	if (!inFrame) return false;

	// Queue the target in the frame if it hasn't already been.
	int pos = queueSwap^queuedCount;
	if (!target->frame[pos])
	{
		if (!queuedFrame[pos].targetList)
			queuedFrame[pos].targetList = target;
		else
		{
			C3D_RenderTarget* a;
			for (a = queuedFrame[pos].targetList; a->frame[pos]; a = a->frame[pos]);
			a->frame[pos] = target;
		}
	}

	C3D_SetFrameBuf(&target->frameBuf);
	C3D_SetViewport(0, 0, target->frameBuf.width, target->frameBuf.height);
	return true;
}

void C3D_FrameEnd(u8 flags)
{
	if (!inFrame) return;
	inFrame = false;
	osTickCounterUpdate(&cpuTime);

	int pos = queueSwap^queuedCount;
	if (!queuedFrame[pos].targetList) return;

	// Add the frame to the queue
	queuedCount++;
	C3Di_FinalizeFrame(&queuedFrame[pos].cmdBuf, &queuedFrame[pos].cmdBufSize);
	queuedFrame[pos].flags = flags;

	// Flush the entire linear memory if the user did not explicitly mandate to flush the command list
	if (!(flags & GX_CMDLIST_FLUSH))
	{
		// Take advantage of GX_FlushCacheRegions to flush gsp heap
		extern u32 __ctru_linear_heap;
		extern u32 __ctru_linear_heap_size;
		GX_FlushCacheRegions(queuedFrame[queueSwap].cmdBuf, queuedFrame[queueSwap].cmdBufSize, (u32 *) __ctru_linear_heap, __ctru_linear_heap_size, NULL, 0);
	}

	// Update the frame queue
	updateFrameQueue();
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
	target->drawOk = true;
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

	u32 size = width*height;
	GPU_DEPTHBUF depthFmtReal = GPU_RB_DEPTH16;
	void* depthBuf = NULL;
	void* colorBuf = vramAlloc(size*(2+colorFmtSizes[colorFmt]));
	if (!colorBuf) goto _fail0;
	if (C3D_DEPTHTYPE_OK(depthFmt))
	{
		depthFmtReal = C3D_DEPTHTYPE_VAL(depthFmt);
		depthBuf = vramAlloc(size*(2+depthFmtSizes[depthFmtReal]));
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
		u32 size = (u32)fb->width*fb->height;
		void* depthBuf = vramAlloc(size*(2+depthFmtSizes[depthFmtReal]));
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

void C3D_RenderTargetDelete(C3D_RenderTarget* target)
{
	target->clearBits = 0;
	target->linked = false;
	while (!target->drawOk)
		gspWaitForAnyEvent();

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

void C3D_RenderTargetSetClear(C3D_RenderTarget* target, C3D_ClearBits clearBits, u32 clearColor, u32 clearDepth)
{
	if (!target->frameBuf.colorBuf) clearBits &= ~C3D_CLEAR_COLOR;
	if (!target->frameBuf.depthBuf) clearBits &= ~C3D_CLEAR_DEPTH;

	C3D_ClearBits oldClearBits = target->clearBits;
	target->clearBits = clearBits;
	target->clearColor = clearColor;
	target->clearDepth = clearDepth;

	if (clearBits &~ oldClearBits && target->drawOk)
	{
		target->drawOk = false;
		clearTarget(target);
	}
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

void C3D_SafeDisplayTransfer(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 flags)
{
	while (transferQueue || inSafeTransfer)
		gspWaitForPPF();
	inSafeTransfer = true;
	gspSetEventCallback(GSPGPU_EVENT_PPF, onTransferFinish, NULL, true);
	GX_DisplayTransfer(inadr, indim, outadr, outdim, flags);
}

void C3D_SafeTextureCopy(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 size, u32 flags)
{
	while (transferQueue || inSafeTransfer)
		gspWaitForPPF();
	inSafeTransfer = true;
	gspSetEventCallback(GSPGPU_EVENT_PPF, onTransferFinish, NULL, true);
	GX_TextureCopy(inadr, indim, outadr, outdim, size, flags);
}

void C3D_SafeMemoryFill(u32* buf0a, u32 buf0v, u32* buf0e, u16 control0, u32* buf1a, u32 buf1v, u32* buf1e, u16 control1)
{
	while (clearQueue || inSafeClear)
		gspWaitForAnyEvent();
	inSafeClear = true;
	gspSetEventCallback(buf0a ? GSPGPU_EVENT_PSC0 : GSPGPU_EVENT_PSC1, onClearDone, NULL, true);
	GX_MemoryFill(buf0a, buf0v, buf0e, control0, buf1a, buf1v, buf1e, control1);
}
