#pragma once
#include "types.h"
#include "texture.h"

typedef struct
{
	C3D_Tex colorBuf, depthBuf;
	u32 clearColor, clearDepth;
	int depthFmt;
} C3D_RenderBuf;

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
