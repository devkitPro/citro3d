#include "context.h"

static const u8 colorFmtSizes[] = {2,1,0,0,0};
static const u8 depthFmtSizes[] = {0,0,1,2};

static inline u32 calcColorBufSize(u32 width, u32 height, u32 fmt)
{
	return width*height*(colorFmtSizes[fmt]+2);
}

static inline u32 calcDepthBufSize(u32 width, u32 height, u32 fmt)
{
	return width*height*(depthFmtSizes[fmt]+2);
}

bool C3D_RenderBufInit(C3D_RenderBuf* rb, int width, int height, int colorFmt, int depthFmt)
{
	if (rb->colorBuf || rb->depthBuf) return false;

	rb->colorFmt = colorFmt;
	rb->depthFmt = depthFmt;
	rb->width = width;
	rb->height = height;
	rb->clearColor = rb->clearDepth = 0;

	rb->colorBuf = vramAlloc(calcColorBufSize(width, height, colorFmt));
	if (!rb->colorBuf) return false;

	rb->depthBuf = vramAlloc(calcDepthBufSize(width, height, depthFmt));
	if (!rb->depthBuf)
	{
		vramFree(rb->colorBuf);
		rb->colorBuf = NULL;
		return false;
	}

	return true;
}

void C3D_RenderBufClearAsync(C3D_RenderBuf* rb)
{
	u32 colorBufSize = calcColorBufSize(rb->width, rb->height, rb->colorFmt);
	u32 depthBufSize = calcDepthBufSize(rb->width, rb->height, rb->depthFmt);
	GX_MemoryFill(
		(u32*)rb->colorBuf, rb->clearColor, (u32*)((u8*)rb->colorBuf+colorBufSize), BIT(0) | ((u32)colorFmtSizes[rb->colorFmt] << 8),
		(u32*)rb->depthBuf, rb->clearDepth, (u32*)((u8*)rb->depthBuf+depthBufSize), BIT(0) | ((u32)depthFmtSizes[rb->depthFmt] << 8));
}

void C3D_RenderBufTransferAsync(C3D_RenderBuf* rb, u32* frameBuf, u32 flags)
{
	u32 dim = GX_BUFFER_DIM((u32)rb->width, (u32)rb->height);
	GX_DisplayTransfer((u32*)rb->colorBuf, dim, frameBuf, dim, flags);
}

void C3D_RenderBufBind(C3D_RenderBuf* rb)
{
	C3D_Context* ctx = C3Di_GetContext();
	ctx->flags |= C3DiF_RenderBuf;
	ctx->rb = rb;
	C3D_SetViewport(0, 0, rb->width, rb->height);
}

void C3Di_RenderBufBind(C3D_RenderBuf* rb)
{
	u32 param[4];

	GPUCMD_AddWrite(GPUREG_FRAMEBUFFER_INVALIDATE, 1);

	param[0] = osConvertVirtToPhys((u32)rb->depthBuf) >> 3;
	param[1] = osConvertVirtToPhys((u32)rb->colorBuf) >> 3;
	param[2] = 0x01000000 | (((u32)(rb->height-1) & 0xFFF) << 12) | (rb->width & 0xFFF);
	GPUCMD_AddIncrementalWrites(GPUREG_DEPTHBUFFER_LOC, param, 3);

	GPUCMD_AddWrite(GPUREG_FRAMEBUFFER_DIM2, param[2]); //?
	GPUCMD_AddWrite(GPUREG_DEPTHBUFFER_FORMAT, rb->depthFmt);
	GPUCMD_AddWrite(GPUREG_COLORBUFFER_FORMAT, ((u32)rb->colorFmt << 16) | colorFmtSizes[rb->colorFmt]);
	GPUCMD_AddWrite(GPUREG_FRAMEBUFFER_BLOCK32, 0x00000000); //?

	// "Enable depth buffer" (?)
	param[0] = param[1] = 0xF;
	param[2] = param[3] = 0x2;
	GPUCMD_AddIncrementalWrites(GPUREG_COLORBUFFER_READ, param, 4);
}

void C3D_RenderBufDelete(C3D_RenderBuf* rb)
{
	if (rb->colorBuf)
	{
		vramFree(rb->colorBuf);
		rb->colorBuf = NULL;
	}

	if (rb->depthBuf)
	{
		vramFree(rb->depthBuf);
		rb->depthBuf = NULL;
	}
}
