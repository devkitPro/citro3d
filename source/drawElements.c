#include "context.h"

void C3D_DrawElements(GPU_Primitive_t primitive, int count, int type, const void* indices)
{
	C3D_Context* ctx = C3Di_GetContext();
	u32 pa = osConvertVirtToPhys(indices);
	u32 base = ctx->bufInfo.base_paddr;
	if (pa < base) return;

	C3Di_UpdateContext();

	// Set primitive type
	GPUCMD_AddMaskedWrite(GPUREG_PRIMITIVE_CONFIG, 2, primitive);
	// Start a new primitive (breaks off a triangle strip/fan)
	GPUCMD_AddWrite(GPUREG_RESTART_PRIMITIVE, 1);
	// Configure the index buffer
	GPUCMD_AddWrite(GPUREG_INDEXBUFFER_CONFIG, (pa - base) | (type << 31));
	// Number of vertices
	GPUCMD_AddWrite(GPUREG_NUMVERTICES, count);
	// First vertex
	GPUCMD_AddWrite(GPUREG_VERTEX_OFFSET, 0);
	// Disable "triangles" mode (otherwise stuff breaks)
	GPUCMD_AddMaskedWrite(GPUREG_GEOSTAGE_CONFIG,  2, 0x000);
	GPUCMD_AddMaskedWrite(GPUREG_GEOSTAGE_CONFIG2, 3, 0x000);
	// Enable drawing mode
	GPUCMD_AddMaskedWrite(GPUREG_START_DRAW_FUNC0, 1, 0);
	// Trigger element drawing
	GPUCMD_AddWrite(GPUREG_DRAWELEMENTS, 1);
	// Go back to configuration mode
	GPUCMD_AddMaskedWrite(GPUREG_START_DRAW_FUNC0, 1, 1);
	// Clear the post-vertex cache
	GPUCMD_AddWrite(GPUREG_VTX_FUNC, 1);

	C3Di_GetContext()->flags |= C3DiF_DrawUsed;
}
