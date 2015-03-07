#pragma once
#include "types.h"

typedef struct
{
	void *colorBuf, *depthBuf;
	u16 colorFmt, depthFmt;
	u16 width, height;
	u32 clearColor, clearDepth;
} C3D_RenderBuf;

enum
{
	// Renderbuffer color formats
	GPU_RB_RGBA8 = 0,
	GPU_RB_RGB8,
	GPU_RB_RGB5_A1,
	GPU_RB_R5_G6_B5,
	GPU_RB_RGBA4,

	// Renderbuffer depth formats
	GPU_RB_DEPTH16 = 0,
	GPU_RB_DEPTHXX,
	GPU_RB_DEPTH24,
	GPU_RB_DEPTH24_STENCIL8,
};

bool C3D_RenderBufInit(C3D_RenderBuf* rb, int width, int height, int colorFmt, int depthFmt);
void C3D_RenderBufClearAsync(C3D_RenderBuf* rb);
void C3D_RenderBufTransferAsync(C3D_RenderBuf* rb, u32* frameBuf, u32 flags);
void C3D_RenderBufBind(C3D_RenderBuf* rb);
void C3D_RenderBufDelete(C3D_RenderBuf* rb);

static inline void C3D_RenderBufClear(C3D_RenderBuf* rb)
{
	C3D_RenderBufClearAsync(rb);
	gspWaitForPSC0();
}

static inline void C3D_RenderBufTransfer(C3D_RenderBuf* rb, u32* frameBuf, u32 flags)
{
	C3D_RenderBufTransferAsync(rb, frameBuf, flags);
	gspWaitForPPF();
}
