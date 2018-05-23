#include "internal.h"

static inline u32 calc_diff(u32 a, u32 b, int pos)
{
	float fa = ((a>>pos)&0xFF)/255.0f;
	float fb = ((b>>pos)&0xFF)/255.0f;
	float x = fb-fa;
	u32 diff = 0;
	if (x < 0)
	{
		diff = 0x80;
		x = -x;
	}
	diff |= (u32)(x*0x7F);
	return diff<<pos;
}

static inline u32 conv_u8(float x, int pos)
{
	if (x < 0.0f) x = 0.0f;
	else if (x > 1.0f) x = 1.0f;
	return ((u32)x*255)<<pos;
}

static inline u32 color_diff(u32 a, u32 b)
{
	return calc_diff(a,b,0) | calc_diff(a,b,8) | calc_diff(a,b,16);
}

void GasLut_FromArray(C3D_GasLut* lut, const u32 data[9])
{
	int i;
	for (i = 0; i <= 8; i ++)
	{
		if (i < 8)
			lut->color[i] = data[i];
		if (i > 0)
			lut->diff[i-1] = color_diff(data[i-1], data[i]);
	}
}

void C3D_GasBeginAcc(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->gasFlags |= C3DiG_BeginAcc;
}

void C3D_GasDeltaZ(float value)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Gas;
	ctx->gasDeltaZ = (u32)(value*0x100);
	ctx->gasFlags |= C3DiG_AccStage;
}

void C3D_GasAccMax(float value)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Gas;
	ctx->gasAccMax = f32tof16(1.0f / value);
	ctx->gasFlags |= C3DiG_SetAccMax;
}

void C3D_GasAttn(float value)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Gas;
	ctx->gasAttn = f32tof16(value);
	ctx->gasFlags |= C3DiG_RenderStage;
}

void C3D_GasLightPlanar(float min, float max, float attn)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Gas;
	ctx->gasLightXY = conv_u8(min,0) | conv_u8(max,8) | conv_u8(attn,16);
	ctx->gasFlags |= C3DiG_RenderStage;
}

void C3D_GasLightView(float min, float max, float attn)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Gas;
	ctx->gasLightZ = conv_u8(min,0) | conv_u8(max,8) | conv_u8(attn,16);
	ctx->gasFlags |= C3DiG_RenderStage;
}

void C3D_GasLightDirection(float dotp)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Gas;
	ctx->gasLightZColor &= ~0xFF;
	ctx->gasLightZColor |= conv_u8(dotp,0);
	ctx->gasFlags |= C3DiG_RenderStage;
}

void C3D_GasLutInput(GPU_GASLUTINPUT input)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Gas;
	ctx->gasLightZColor &= ~0x100;
	ctx->gasLightZColor |= (input&1)<<8;
	ctx->gasFlags |= C3DiG_RenderStage;
}

void C3D_GasLutBind(C3D_GasLut* lut)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	if (lut)
	{
		ctx->flags |= C3DiF_GasLut;
		ctx->gasLut = lut;
	} else
		ctx->flags &= ~C3DiF_GasLut;
}

void C3Di_GasUpdate(C3D_Context* ctx)
{
	if (ctx->flags & C3DiF_Gas)
	{
		ctx->flags &= ~C3DiF_Gas;
		u32 gasFlags = ctx->gasFlags;
		ctx->gasFlags = 0;

		if (gasFlags & C3DiG_BeginAcc)
			GPUCMD_AddMaskedWrite(GPUREG_GAS_ACCMAX_FEEDBACK, 0x3, 0);
		if (gasFlags & C3DiG_AccStage)
			GPUCMD_AddMaskedWrite(GPUREG_GAS_DELTAZ_DEPTH, 0x7, ctx->gasDeltaZ);
		if (gasFlags & C3DiG_SetAccMax)
			GPUCMD_AddWrite(GPUREG_GAS_ACCMAX, ctx->gasAccMax);
		if (gasFlags & C3DiG_RenderStage)
		{
			GPUCMD_AddWrite(GPUREG_GAS_ATTENUATION, ctx->gasAttn);
			GPUCMD_AddWrite(GPUREG_GAS_LIGHT_XY, ctx->gasLightXY);
			GPUCMD_AddWrite(GPUREG_GAS_LIGHT_Z, ctx->gasLightZ);
			GPUCMD_AddWrite(GPUREG_GAS_LIGHT_Z_COLOR, ctx->gasLightZColor);
		}
	}
	if (ctx->flags & C3DiF_GasLut)
	{
		ctx->flags &= ~C3DiF_GasLut;
		if (ctx->gasLut)
		{
			GPUCMD_AddWrite(GPUREG_GAS_LUT_INDEX, 0);
			GPUCMD_AddWrites(GPUREG_GAS_LUT_DATA, (u32*)ctx->gasLut, 16);
		}
	}
}
