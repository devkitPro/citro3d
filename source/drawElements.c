#include "context.h"

void C3D_DrawElements(GPU_Primitive_t primitive, int count, int type, const void* indices)
{
	C3D_Context* ctx = C3Di_GetContext();
	u32 pa = osConvertVirtToPhys((u32)indices);
	u32 base = ctx->bufInfo.base_paddr;
	if (pa < base) return;

	C3Di_UpdateContext();

	// Set primitive type
	GPUCMD_AddMaskedWrite(GPUREG_PRIMITIVE_CONFIG, 2, primitive);
	GPUCMD_AddMaskedWrite(GPUREG_RESTART_PRIMITIVE, 2, 0x00000001);
	// Configure the index buffer
	GPUCMD_AddWrite(GPUREG_INDEXBUFFER_CONFIG, (pa - base) | (type << 31));
	// Number of vertices
	GPUCMD_AddWrite(GPUREG_NUMVERTICES, count);
	// First vertex
	GPUCMD_AddWrite(GPUREG_VERTEX_OFFSET, 0);

	// Unknown commands
	//GPUCMD_AddMaskedWrite(GPUREG_GEOSTAGE_CONFIG, 2, 0x00000100);
	GPUCMD_AddMaskedWrite(GPUREG_0253, 2, 0x00000100);

	GPUCMD_AddMaskedWrite(GPUREG_0245, 1, 0x00000000);
	GPUCMD_AddWrite(GPUREG_DRAWELEMENTS, 0x00000001);
	GPUCMD_AddMaskedWrite(GPUREG_0245, 1, 0x00000001);
	GPUCMD_AddWrite(GPUREG_0231, 0x00000001);

	C3Di_GetContext()->flags |= C3DiF_DrawUsed;
}
