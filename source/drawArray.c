#include "context.h"

void C3D_DrawArray(C3D_VBO* vbo, GPU_Primitive_t primitive)
{
	C3D_VBOBind(vbo);
	C3Di_UpdateContext();

	// Set primitive type
	GPUCMD_AddSingleParam(0x0002025E, primitive);
	GPUCMD_AddSingleParam(0x0002025F, 0x00000001);
	// The index buffer is not used, but 0x000F0227 is still required
	GPUCMD_AddSingleParam(0x000F0227, 0x80000000);
	// Number of vertices
	GPUCMD_AddSingleParam(0x000F0228, vbo->vertexCount);

	// Unknown commands
	GPUCMD_AddSingleParam(0x00010253, 0x00000001);

	GPUCMD_AddSingleParam(0x00010245, 0x00000000);
	GPUCMD_AddSingleParam(0x000F022E, 0x00000001);
	GPUCMD_AddSingleParam(0x00010245, 0x00000001);
	GPUCMD_AddSingleParam(0x000F0231, 0x00000001);

	C3Di_GetContext()->flags |= C3DiF_NeedFinishDrawing;
}
