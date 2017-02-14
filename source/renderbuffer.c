#include "internal.h"
#include <c3d/base.h>
#include <c3d/renderbuffer.h>

static const u8 colorFmtSizes[] = {2,1,0,0,0};
static const u8 depthFmtSizes[] = {0,0,1,2};

static inline u16 getColorBufFillFlag(int fmt)
{
	if (fmt < 0) return 0;
	return BIT(0) | ((u32)colorFmtSizes[fmt] << 8);
}

static inline u16 getDepthBufFillFlag(int fmt)
{
	if (fmt < 0) return 0;
	return BIT(0) | ((u32)depthFmtSizes[fmt] << 8);
}

static inline GPU_TEXCOLOR colorFmtFromDepthFmt(int fmt)
{
	switch (fmt)
	{
		case GPU_RB_DEPTH16:
			return GPU_RGB565;
		case GPU_RB_DEPTH24:
			return GPU_RGB8;
		case GPU_RB_DEPTH24_STENCIL8:
			return GPU_RGBA8;
		default:
			return 0;
	}
}

bool C3D_RenderBufInit(C3D_RenderBuf* rb, int width, int height, int colorFmt, int depthFmt)
{
	memset(rb, 0, sizeof(*rb));

	rb->depthFmt = depthFmt;
	rb->clearColor = rb->clearDepth = 0;

	if (colorFmt < 0)
		return false;
	if (!C3D_TexInitVRAM(&rb->colorBuf, width, height, colorFmt))
		return false;

	if (depthFmt >= 0)
	{
		if (!C3D_TexInitVRAM(&rb->depthBuf, width, height, colorFmtFromDepthFmt(depthFmt)))
		{
			C3D_TexDelete(&rb->colorBuf);
			return false;
		}
	}

	return true;
}

void C3D_RenderBufClearAsync(C3D_RenderBuf* rb)
{
	GX_MemoryFill(
		(u32*)rb->colorBuf.data, rb->clearColor, (u32*)((u8*)rb->colorBuf.data+rb->colorBuf.size), getColorBufFillFlag(rb->colorBuf.fmt),
		(u32*)rb->depthBuf.data, rb->clearDepth, (u32*)((u8*)rb->depthBuf.data+rb->depthBuf.size), getDepthBufFillFlag(rb->depthFmt));
}

void C3D_RenderBufTransferAsync(C3D_RenderBuf* rb, u32* frameBuf, u32 flags)
{
	u32 dim = GX_BUFFER_DIM((u32)rb->colorBuf.width, (u32)rb->colorBuf.height);
	GX_DisplayTransfer((u32*)rb->colorBuf.data, dim, frameBuf, dim, flags);
}

void C3D_RenderBufBind(C3D_RenderBuf* rb)
{
	C3D_Context* ctx = C3Di_GetContext();
	C3D_FrameBuf* fb = &ctx->fb;
	ctx->flags   |= C3DiF_FrameBuf;
	fb->colorBuf  = rb->colorBuf.data;
	fb->depthBuf  = rb->depthBuf.data;
	fb->width     = rb->colorBuf.width;
	fb->height    = rb->colorBuf.height;
	fb->colorFmt  = (GPU_COLORBUF)rb->colorBuf.fmt;
	fb->depthFmt  = rb->depthFmt >= 0 ? (GPU_DEPTHBUF)rb->depthFmt : GPU_RB_DEPTH16;
	fb->block32   = false;
	fb->colorMask = 0xF;
	fb->depthMask = 0x2;
	C3D_SetViewport(0, 0, rb->colorBuf.width, rb->colorBuf.height);
}

void C3D_RenderBufDelete(C3D_RenderBuf* rb)
{
	C3D_TexDelete(&rb->colorBuf);
	C3D_TexDelete(&rb->depthBuf);
}
