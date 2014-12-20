#pragma once
#include "types.h"
#define C3D_FVUNIF_COUNT 96
#define C3D_IVUNIF_COUNT 4

extern C3D_FVec C3D_FVUnif[C3D_FVUNIF_COUNT];
extern C3D_IVec C3D_IVUnif[C3D_IVUNIF_COUNT];
extern u16      C3D_BoolUnifs;

extern bool C3D_FVUnifDirty[C3D_FVUNIF_COUNT];
extern bool C3D_IVUnifDirty[C3D_IVUNIF_COUNT];
extern bool C3D_BoolUnifsDirty;

static inline C3D_FVec* C3D_FVUnifWritePtr(int id, int size)
{
	int i;
	for (i = 0; i < size; i ++)
		C3D_FVUnifDirty[id+i] = true;
	return &C3D_FVUnif[id];
}

static inline C3D_IVec* C3D_IVUnifWritePtr(int id)
{
	C3D_IVUnifDirty[id] = true;
	return &C3D_IVUnif[id];
}

static inline void C3D_FVUnifSet(int id, float x, float y, float z, float w)
{
	C3D_FVec* ptr = C3D_FVUnifWritePtr(id, 1);
	ptr->x = x;
	ptr->y = y;
	ptr->z = z;
	ptr->w = w;
}

static inline void C3D_IVUnifSet(int id, int x, int y, int z, int w)
{
	C3D_IVec* ptr = C3D_IVUnifWritePtr(id);
	*ptr = IVec_Pack(x, y, z, w);
}

void C3D_UpdateUniforms(void);
