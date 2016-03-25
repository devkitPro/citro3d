#include "context.h"
#include <c3d/renderqueue.h>
#include <string.h>
#include <stdlib.h>

static C3D_RenderTarget *firstTarget, *lastTarget;
static C3D_RenderTarget *linkedTarget[3];
static C3D_RenderTarget *transferQueue, *clearQueue;

static struct
{
	C3D_RenderTarget* targetList;
	u32* cmdBuf;
	u32 cmdBufSize;
	u8 flags;
} queuedFrame[2];
static u8 queueSwap, queuedCount, queuedState;

static bool inFrame;

static void onRenderFinish(void* unused);
static void onTransferFinish(void* unused);
static void onClearDone(void* unused);

static void performDraw(void)
{
	gspSetEventCallback(GSPGPU_EVENT_P3D, onRenderFinish, NULL, true);
	GX_ProcessCommandList(queuedFrame[queueSwap].cmdBuf, queuedFrame[queueSwap].cmdBufSize, queuedFrame[queueSwap].flags);
}

static void performTransfer(void)
{
	C3D_RenderBuf* renderBuf = &transferQueue->renderBuf;
	u32* frameBuf = (u32*)gfxGetFramebuffer(transferQueue->screen, transferQueue->side, NULL, NULL);
	if (transferQueue->side == GFX_LEFT)
		gfxConfigScreen(transferQueue->screen, false);
	gspSetEventCallback(GSPGPU_EVENT_PPF, onTransferFinish, NULL, true);
	C3D_RenderBufTransferAsync(renderBuf, frameBuf, transferQueue->transferFlags);
}

static void performClear(void)
{
	C3D_RenderBuf* renderBuf = &clearQueue->renderBuf;
	// TODO: obey renderBuf->clearBits
	gspSetEventCallback(renderBuf->colorBuf.data ? GSPGPU_EVENT_PSC0 : GSPGPU_EVENT_PSC1, onClearDone, NULL, true);
	C3D_RenderBufClearAsync(renderBuf);
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

static void onVBlank0(void* unused)
{
	if (!linkedTarget[0]) return;

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

static void onVBlank1(void* unused)
{
	if (linkedTarget[2] && linkedTarget[2]->transferOk)
		transferTarget(linkedTarget[2]);
}

void onRenderFinish(void* unused)
{
	C3D_RenderTarget *a, *next;

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

void onTransferFinish(void* unused)
{
	C3D_RenderTarget* target = transferQueue;
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

void onClearDone(void* unused)
{
	C3D_RenderTarget* target = clearQueue;
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

bool checkRenderQueueInit(void)
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

	C3D_RenderBufBind(&target->renderBuf);
	return true;
}

void C3D_FrameEnd(u8 flags)
{
	if (!inFrame) return;
	inFrame = false;

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

C3D_RenderTarget* C3D_RenderTargetCreate(int width, int height, int colorFmt, int depthFmt)
{
	if (!checkRenderQueueInit()) return NULL;
	C3D_RenderTarget* target = (C3D_RenderTarget*)malloc(sizeof(C3D_RenderTarget));
	if (!target) return NULL;
	memset(target, 0, sizeof(C3D_RenderTarget));
	if (!C3D_RenderBufInit(&target->renderBuf, width, height, colorFmt, depthFmt))
	{
		free(target);
		return NULL;
	}

	target->drawOk = true;
	target->prev = lastTarget;
	target->next = NULL;
	if (lastTarget)
		lastTarget->next = target;
	if (!firstTarget)
		firstTarget = target;
	lastTarget = target;

	return target;
}

void C3D_RenderTargetDelete(C3D_RenderTarget* target)
{
	target->clearBits = 0;
	target->linked = false;
	while (!target->drawOk)
		gspWaitForAnyEvent();
	C3D_RenderBufDelete(&target->renderBuf);
	C3D_RenderTarget** prevNext = target->prev ? &target->prev->next : &firstTarget;
	C3D_RenderTarget** nextPrev = target->next ? &target->next->prev : &lastTarget;
	*prevNext = target->next;
	*nextPrev = target->prev;
	free(target);
}

void C3D_RenderTargetSetClear(C3D_RenderTarget* target, u32 clearBits, u32 clearColor, u32 clearDepth)
{
	if (target->renderBuf.colorBuf.data==NULL) clearBits &= ~C3D_CLEAR_COLOR;
	if (target->renderBuf.depthBuf.data==NULL) clearBits &= ~C3D_CLEAR_DEPTH;

	u32 oldClearBits = target->clearBits;
	target->clearBits = clearBits & 0xFF;
	target->renderBuf.clearColor = clearColor;
	target->renderBuf.clearDepth = clearDepth;

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
