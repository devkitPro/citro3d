#pragma once
#include "types.h"

typedef struct
{
	u32 offset;
	u32 flags[2];
} C3D_BufCfg;

typedef struct
{
	u32 base_paddr;
	int bufCount;
	C3D_BufCfg buffers[12];
} C3D_BufInfo;

void BufInfo_Init(C3D_BufInfo* info);
int  BufInfo_Add(C3D_BufInfo* info, const void* data, ptrdiff_t stride, int attribCount, u64 permutation);

C3D_BufInfo* C3D_GetBufInfo(void);
void C3D_SetBufInfo(C3D_BufInfo* info);
