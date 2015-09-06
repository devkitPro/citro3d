#include <c3d/buffers.h>
#include <string.h>
#include "context.h"

bool C3D_VBOInit(C3D_VBO* vbo, size_t capacity)
{
	if (vbo->data) return false;

	vbo->data = linearAlloc(capacity);
	if (!vbo->data) return false;
	vbo->capacity = capacity;
	vbo->size = 0;
	vbo->vertexCount = 0;
	return true;
}

bool C3D_VBOAddData(C3D_VBO* vbo, const void* data, size_t size, int vertexCount)
{
	size_t remaining = vbo->capacity - vbo->size;
	if (remaining < size) return false;
	memcpy((u8*)vbo->data + vbo->size, data, size);
	vbo->size += size;
	vbo->vertexCount += vertexCount;
	return true;
}

void C3D_VBOFlush(C3D_VBO* vbo)
{
	if (vbo->data)
		GSPGPU_FlushDataCache(vbo->data, vbo->size);
}

void C3D_VBOBind(C3D_VBO* vbo)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->vboPos = osConvertVirtToPhys((u32)vbo->data);
	GPUCMD_AddWrite(GPUREG_ATTRIBBUFFERS_LOC, ctx->vboPos >> 3);
}

void C3D_VBODelete(C3D_VBO* vbo)
{
	if (!vbo->data) return;
	linearFree(vbo->data);
	vbo->data = NULL;
}
