#pragma once
#include "types.h"

typedef struct
{
	u32 flags[2];
	u64 permutation;
	int attrCount;
} C3D_AttrInfo;

void AttrInfo_Init(C3D_AttrInfo* info);
int  AttrInfo_AddLoader(C3D_AttrInfo* info, int regId, GPU_FORMATS format, int count);
int  AttrInfo_AddFixed(C3D_AttrInfo* info, int regId);

C3D_AttrInfo* C3D_GetAttrInfo(void);
void C3D_SetAttrInfo(C3D_AttrInfo* info);
