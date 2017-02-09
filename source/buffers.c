#include "internal.h"

#define BUFFER_BASE_PADDR 0x18000000

void BufInfo_Init(C3D_BufInfo* info)
{
	memset(info, 0, sizeof(*info));
	info->base_paddr = BUFFER_BASE_PADDR;
}

int BufInfo_Add(C3D_BufInfo* info, const void* data, ptrdiff_t stride, int attribCount, u64 permutation)
{
	if (info->bufCount == 12) return -1;
	int id = info->bufCount++;

	u32 pa = osConvertVirtToPhys(data);
	if (pa < info->base_paddr) return -2;

	C3D_BufCfg* buf = &info->buffers[id];
	buf->offset = pa - info->base_paddr;
	buf->flags[0] = permutation & 0xFFFFFFFF;
	buf->flags[1] = (permutation >> 32) | (stride << 16) | (attribCount << 28);
	return id;
}

C3D_BufInfo* C3D_GetBufInfo(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return NULL;

	ctx->flags |= C3DiF_BufInfo;
	return &ctx->bufInfo;
}

void C3D_SetBufInfo(C3D_BufInfo* info)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	if (info != &ctx->bufInfo)
		memcpy(&ctx->bufInfo, info, sizeof(*info));
	ctx->flags |= C3DiF_BufInfo;
}

void C3Di_BufInfoBind(C3D_BufInfo* info)
{
	GPUCMD_AddWrite(GPUREG_ATTRIBBUFFERS_LOC, info->base_paddr >> 3);
	GPUCMD_AddIncrementalWrites(GPUREG_ATTRIBBUFFER0_OFFSET, (u32*)info->buffers, sizeof(info->buffers)/sizeof(u32));
}
