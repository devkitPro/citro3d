#pragma once
#include "buffers.h"
#include "maths.h"

#define C3D_DEFAULT_CMDBUF_SIZE 0x40000

enum
{
	C3D_UNSIGNED_BYTE = 0,
	C3D_UNSIGNED_SHORT = 1,
};

bool C3D_Init(size_t cmdBufSize);
void C3D_Fini(void);

float C3D_GetCmdBufUsage(void);

void C3D_BindProgram(shaderProgram_s* program);

void C3D_SetViewport(u32 x, u32 y, u32 w, u32 h);
void C3D_SetScissor(GPU_SCISSORMODE mode, u32 left, u32 top, u32 right, u32 bottom);

void C3D_DrawArrays(GPU_Primitive_t primitive, int first, int size);
void C3D_DrawElements(GPU_Primitive_t primitive, int count, int type, const void* indices);

// Immediate-mode vertex submission
void C3D_ImmDrawBegin(GPU_Primitive_t primitive);
void C3D_ImmSendAttrib(float x, float y, float z, float w);
void C3D_ImmDrawEnd(void);

static inline void C3D_ImmDrawRestartPrim(void)
{
	GPUCMD_AddWrite(GPUREG_RESTART_PRIMITIVE, 1);
}

// Fixed vertex attributes
C3D_FVec* C3D_FixedAttribGetWritePtr(int id);

static inline void C3D_FixedAttribSet(int id, float x, float y, float z, float w)
{
	C3D_FVec* ptr = C3D_FixedAttribGetWritePtr(id);
	ptr->x = x;
	ptr->y = y;
	ptr->z = z;
	ptr->w = w;
}
