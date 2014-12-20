#include "context.h"

void C3D_DrawElements(C3D_IBO* ibo, GPU_Primitive_t primitive)
{
	C3Di_UpdateContext();

	// Set primitive type
	GPUCMD_AddSingleParam(0x0002025E, primitive);
	GPUCMD_AddSingleParam(0x0002025F, 0x00000001);
	// Bind the IBO
	C3Di_IBOBind(ibo);
	// Number of vertices
	GPUCMD_AddSingleParam(0x000F0228, ibo->indexCount);

	// Unknown commands
	GPUCMD_AddSingleParam(0x00020229, 0x00000100);
	GPUCMD_AddSingleParam(0x00020253, 0x00000100);

	GPUCMD_AddSingleParam(0x00010245, 0x00000000);
	GPUCMD_AddSingleParam(0x000F022F, 0x00000001);
	GPUCMD_AddSingleParam(0x00010245, 0x00000001);
	GPUCMD_AddSingleParam(0x000F0231, 0x00000001);

	C3Di_GetContext()->flags |= C3DiF_NeedFinishDrawing;
}
