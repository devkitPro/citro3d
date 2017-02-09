#include "internal.h"

void C3D_ImmDrawBegin(GPU_Primitive_t primitive)
{
	C3Di_UpdateContext();

	// Set primitive type
	GPUCMD_AddMaskedWrite(GPUREG_PRIMITIVE_CONFIG, 2, primitive);
	// Start a new primitive (breaks off a triangle strip/fan)
	GPUCMD_AddWrite(GPUREG_RESTART_PRIMITIVE, 1);
	// Not sure if this command is necessary
	GPUCMD_AddWrite(GPUREG_INDEXBUFFER_CONFIG, 0x80000000);
	// Enable vertex submission mode
	GPUCMD_AddMaskedWrite(GPUREG_GEOSTAGE_CONFIG2, 1, 1);
	// Enable drawing mode
	GPUCMD_AddMaskedWrite(GPUREG_START_DRAW_FUNC0, 1, 0);
	// Begin immediate-mode vertex submission
	GPUCMD_AddWrite(GPUREG_FIXEDATTRIB_INDEX, 0xF);
}

static inline void write24(u8* p, u32 val)
{
	p[0] = val;
	p[1] = val>>8;
	p[2] = val>>16;
}

void C3D_ImmSendAttrib(float x, float y, float z, float w)
{
	union
	{
		u32 packed[3];
		struct
		{
			u8 x[3];
			u8 y[3];
			u8 z[3];
			u8 w[3];
		};
	} param;

	// Convert the values to float24
	write24(param.x, f32tof24(x));
	write24(param.y, f32tof24(y));
	write24(param.z, f32tof24(z));
	write24(param.w, f32tof24(w));

	// Reverse the packed words
	u32 p = param.packed[0];
	param.packed[0] = param.packed[2];
	param.packed[2] = p;

	// Send the attribute
	GPUCMD_AddIncrementalWrites(GPUREG_FIXEDATTRIB_DATA0, param.packed, 3);
}

void C3D_ImmDrawEnd(void)
{
	// Go back to configuration mode
	GPUCMD_AddMaskedWrite(GPUREG_START_DRAW_FUNC0, 1, 1);
	// Disable vertex submission mode
	GPUCMD_AddMaskedWrite(GPUREG_GEOSTAGE_CONFIG2, 1, 0);
	// Clear the post-vertex cache
	GPUCMD_AddWrite(GPUREG_VTX_FUNC, 1);

	C3Di_GetContext()->flags |= C3DiF_DrawUsed;
}
