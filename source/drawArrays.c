#include "context.h"

void C3D_DrawArrays(GPU_Primitive_t primitive, int first, int size)
{
	C3Di_UpdateContext();

	// Set primitive type
	GPUCMD_AddMaskedWrite(GPUREG_PRIMITIVE_CONFIG, 2, primitive);
	GPUCMD_AddMaskedWrite(GPUREG_RESTART_PRIMITIVE, 2, 0x00000001);
	// The index buffer is not used, but 0x000F0227 is still required
	GPUCMD_AddWrite(GPUREG_INDEXBUFFER_CONFIG, 0x80000000);
	// Number of vertices
	GPUCMD_AddWrite(GPUREG_NUMVERTICES, size);
	// First vertex
	GPUCMD_AddWrite(GPUREG_VERTEX_OFFSET, first);

	// Unknown commands
	GPUCMD_AddMaskedWrite(GPUREG_0253, 1, 0x00000001);

	GPUCMD_AddMaskedWrite(GPUREG_0245, 1, 0x00000000);
	GPUCMD_AddWrite(GPUREG_DRAWARRAYS, 0x00000001);
	GPUCMD_AddMaskedWrite(GPUREG_0245, 1, 0x00000001);
	GPUCMD_AddWrite(GPUREG_0231, 0x00000001);

	C3Di_GetContext()->flags |= C3DiF_DrawUsed;
}
