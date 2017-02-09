#pragma once
#include "maths.h"

#define C3D_MTXSTACK_SIZE 8

typedef struct
{
	C3D_Mtx m[C3D_MTXSTACK_SIZE];
	int pos;
	u8 unifType, unifPos, unifLen;
	bool isDirty;
} C3D_MtxStack;

static inline C3D_Mtx* MtxStack_Cur(C3D_MtxStack* stk)
{
	stk->isDirty = true;
	return &stk->m[stk->pos];
}

void MtxStack_Init(C3D_MtxStack* stk);
void MtxStack_Bind(C3D_MtxStack* stk, GPU_SHADER_TYPE unifType, int unifPos, int unifLen);
C3D_Mtx* MtxStack_Push(C3D_MtxStack* stk);
C3D_Mtx* MtxStack_Pop(C3D_MtxStack* stk);
void MtxStack_Update(C3D_MtxStack* stk);
