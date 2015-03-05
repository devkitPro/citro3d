#include "context.h"

void C3D_DrawElements(C3D_IBO* ibo, GPU_Primitive_t primitive)
{
	C3Di_UpdateContext();

	// Set primitive type
	GPUCMD_AddMaskedWrite(GPUREG_PRIMITIVE_CONFIG, 2, primitive);
	GPUCMD_AddMaskedWrite(GPUREG_025F, 2, 0x00000001);
	// Bind the IBO
	C3Di_IBOBind(ibo);
	// Number of vertices
	GPUCMD_AddWrite(GPUREG_NUMVERTICES, ibo->indexCount);

	// Unknown commands
	GPUCMD_AddMaskedWrite(GPUREG_GEOSTAGE_CONFIG, 2, 0x00000100);
	GPUCMD_AddMaskedWrite(GPUREG_0253, 2, 0x00000100);

	GPUCMD_AddMaskedWrite(GPUREG_0245, 1, 0x00000000);
	GPUCMD_AddWrite(GPUREG_DRAWELEMENTS, 0x00000001);
	GPUCMD_AddMaskedWrite(GPUREG_0245, 1, 0x00000001);
	GPUCMD_AddWrite(GPUREG_0231, 0x00000001);

	C3Di_GetContext()->flags |= C3DiF_NeedFinishDrawing;
}
