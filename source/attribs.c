#include <c3d/attribs.h>
#include <string.h>
#include "context.h"

void AttrInfo_Init(C3D_AttrInfo* info)
{
	memset(info, 0, sizeof(*info));
	info->cfg.flags[1] = 0xFFF << 16;
}

bool AttrInfo_AddParam(C3D_AttrInfo* info, GPU_FORMATS format, int count)
{
	if (info->attrCount == 12) return false;
	int id = info->attrCount++;
	if (id < 8)
		info->cfg.flags[0] |= GPU_ATTRIBFMT(id, count, format);
	else
		info->cfg.flags[1] |= GPU_ATTRIBFMT(id-8, count, format);
	info->cfg.flags[1] &= ~BIT(id+16);
	info->cfg.flags[1] = (info->cfg.flags[1] &~ 0xF0000000) | (id << 28);
	info->permutation |= id << (id*4);
	return true;
}

bool AttrInfo_AddBuffer(C3D_AttrInfo* info, ptrdiff_t offset, ptrdiff_t stride, int attribCount, u64 permutation)
{
	if (info->bufCount == 12) return false;
	int id = info->bufCount++;
	C3D_AttrBufCfg* buf = &info->cfg.buffers[id];
	buf->offset = offset;
	buf->flags[0] = permutation & 0xFFFFFFFF;
	buf->flags[1] = (permutation >> 32) | (stride << 16) | (attribCount << 28);
	return true;
}

C3D_AttrInfo* C3D_GetAttrInfo(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return NULL;

	ctx->flags |= C3DiF_AttrBuf;
	return &ctx->attrInfo;
}

void C3D_SetAttrInfo(C3D_AttrInfo* info)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	memcpy(&ctx->attrInfo, info, sizeof(*info));
	ctx->flags |= C3DiF_AttrBuf;
}

void C3Di_AttrInfoBind(C3D_AttrInfo* info)
{
	GPUCMD_Add(0x800F0201, (u32*)&info->cfg, sizeof(C3D_AttrCfg)/sizeof(u32));
	GPUCMD_AddSingleParam(0x000B02B9, 0xA0000000 | (info->attrCount - 1));
	GPUCMD_AddSingleParam(0x000F0242, info->attrCount - 1);
	GPUCMD_AddSingleParam(0x000F02BB, info->permutation & 0xFFFFFFFF);
	GPUCMD_AddSingleParam(0x000F02BC, info->permutation >> 32);
}
