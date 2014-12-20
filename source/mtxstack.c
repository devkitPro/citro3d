#include <c3d/mtxstack.h>
#include <c3d/uniforms.h>

void MtxStack_Init(C3D_MtxStack* stk)
{
	stk->pos = 0;
	stk->unifPos = 0xFF;
	stk->isDirty = true;
	Mtx_Identity(&stk->m[0]);
}

void MtxStack_Bind(C3D_MtxStack* stk, int unifPos, int unifLen)
{
	stk->unifPos = unifPos;
	stk->unifLen = unifLen;
	stk->isDirty = true;
}

void MtxStack_Push(C3D_MtxStack* stk)
{
	if (stk->pos == (C3D_MTXSTACK_SIZE-1)) return;
	stk->pos ++;
	Mtx_Copy(&stk->m[stk->pos], &stk->m[stk->pos-1]);
}

void MtxStack_Pop(C3D_MtxStack* stk)
{
	if (stk->pos == 0) return;
	stk->pos --;
	stk->isDirty = true;
}

void MtxStack_Update(C3D_MtxStack* stk)
{
	if (!stk->isDirty) return;

	if (stk->unifPos != 0xFF)
	{
		C3D_FVec* out = C3D_FVUnifWritePtr(stk->unifPos, stk->unifLen);
		memcpy(out, &stk->m[stk->pos], (u32)stk->unifLen * sizeof(C3D_FVec));
	}

	stk->isDirty = false;
}
