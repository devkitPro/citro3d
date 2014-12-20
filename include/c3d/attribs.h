#pragma once
#include "types.h"

typedef struct
{
	u32 offset;
	u32 flags[2];
} C3D_AttrBufCfg;

typedef struct
{
	// Base physical address intentionally left out
	// Write to 0x0201 instead of 0x0200
	u32 flags[2];
	C3D_AttrBufCfg buffers[12];
} C3D_AttrCfg;

typedef struct
{
	C3D_AttrCfg cfg;
	u64 permutation;
	int attrCount;
	int bufCount;
} C3D_AttrInfo;

void AttrInfo_Init(C3D_AttrInfo* info);
bool AttrInfo_AddParam(C3D_AttrInfo* info, GPU_FORMATS format, int count);
bool AttrInfo_AddBuffer(C3D_AttrInfo* info, ptrdiff_t offset, ptrdiff_t stride, int attribCount, u64 permutation);

C3D_AttrInfo* C3D_GetAttrInfo(void);
void C3D_SetAttrInfo(C3D_AttrInfo* info);
