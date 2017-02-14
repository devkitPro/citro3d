#pragma once
#include "texture.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

typedef struct C3D_DEPRECATED
{
	C3D_Tex colorBuf, depthBuf;
	u32 clearColor, clearDepth;
	int depthFmt;
} C3D_RenderBuf;

C3D_DEPRECATED bool C3D_RenderBufInit(C3D_RenderBuf* rb, int width, int height, int colorFmt, int depthFmt);
C3D_DEPRECATED void C3D_RenderBufClearAsync(C3D_RenderBuf* rb);
C3D_DEPRECATED void C3D_RenderBufTransferAsync(C3D_RenderBuf* rb, u32* frameBuf, u32 flags);
C3D_DEPRECATED void C3D_RenderBufBind(C3D_RenderBuf* rb);
C3D_DEPRECATED void C3D_RenderBufDelete(C3D_RenderBuf* rb);

C3D_DEPRECATED static inline void C3D_RenderBufClear(C3D_RenderBuf* rb)
{
	C3D_RenderBufClearAsync(rb);
	gspWaitForPSC0();
}

C3D_DEPRECATED static inline void C3D_RenderBufTransfer(C3D_RenderBuf* rb, u32* frameBuf, u32 flags)
{
	C3D_RenderBufTransferAsync(rb, frameBuf, flags);
	gspWaitForPPF();
}

#pragma GCC diagnostic pop
