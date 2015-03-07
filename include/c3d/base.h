#pragma once
#include "types.h"
#include "buffers.h"
#define C3D_DEFAULT_CMDBUF_SIZE 0x40000

bool C3D_Init(size_t cmdBufSize);
void C3D_FlushAsync(void);
void C3D_Fini(void);

void C3D_SetViewport(u32 x, u32 y, u32 w, u32 h);
void C3D_SetScissor(GPU_SCISSORMODE mode, u32 x, u32 y, u32 w, u32 h);

void C3D_DrawArray(C3D_VBO* vbo, GPU_Primitive_t primitive);
void C3D_DrawElements(C3D_IBO* ibo, GPU_Primitive_t primitive);

static inline void C3D_FlushAwait(void)
{
	gspWaitForP3D();
}

static inline void C3D_Flush(void)
{
	C3D_FlushAsync();
	C3D_FlushAwait();
}
