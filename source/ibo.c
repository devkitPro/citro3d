#include <c3d/buffers.h>
#include <string.h>
#include "context.h"

bool C3D_IBOCreate(C3D_IBO* ibo, int capacity, int format)
{
	if (ibo->data) return false;

	ibo->data = linearAlloc(capacity * (format+1));
	if (!ibo->data) return false;
	ibo->capacity = capacity;
	ibo->indexCount = 0;
	ibo->format = format;
	return true;
}

bool C3D_IBOAddData(C3D_IBO* ibo, const void* data, int indexCount)
{
	int remaining = ibo->capacity - ibo->indexCount;
	if (remaining < indexCount) return false;
	int stride = ibo->format+1;
	memcpy((u8*)ibo->data + ibo->indexCount*stride, data, indexCount*stride);
	ibo->indexCount += indexCount;
	return true;
}

void C3D_IBOFlush(C3D_IBO* ibo)
{
	int stride = ibo->format+1;
	GSPGPU_FlushDataCache(ibo->data, ibo->indexCount*stride);
}

void C3Di_IBOBind(C3D_IBO* ibo)
{
	C3D_Context* ctx = C3Di_GetContext();

	u32 pa = osConvertVirtToPhys((u32)ibo->data);
	u32 offset = (pa - ctx->vboPos) &~ BIT(31);
	GPUCMD_AddWrite(GPUREG_INDEXBUFFER_CONFIG, offset | (ibo->format << 31));
}

void C3D_IBODelete(C3D_IBO* ibo)
{
	if (!ibo->data) return;
	linearFree(ibo->data);
	ibo->data = NULL;
}
