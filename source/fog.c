#include "internal.h"

void FogLut_FromArray(C3D_FogLut* lut, const float data[256])
{
	int i;
	for (i = 0; i < 128; i ++)
	{
		float in = data[i], diff = data[i+128];

		u32 val = 0;
		if (in > 0.0f)
		{
			in *= 0x800;
			val = (in < 0x800) ? (u32)in : 0x7FF;
		}

		u32 val2 = 0;
		if (diff != 0.0f)
		{
			diff *= 0x800;
			if (diff < -0x1000) diff = -0x1000;
			else if (diff > 0xFFF) diff = 0xFFF;
			val2 = (s32)diff & 0x1FFF;
		}

		lut->data[i] = val2 | (val << 13);
	}
}

void FogLut_Exp(C3D_FogLut* lut, float density, float gradient, float near, float far)
{
	int i;
	float data[256];
	for (i = 0; i <= 128; i ++)
	{
		float x = FogLut_CalcZ(i/128.0f, near, far);
		float val = expf(-powf(density*x, gradient));
		if (i < 128)
			data[i] = val;
		if (i > 0)
			data[i+127] = val-data[i-1];
	}
	FogLut_FromArray(lut, data);
}

void C3D_FogGasMode(GPU_FOGMODE fogMode, GPU_GASMODE gasMode, bool zFlip)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_TexEnvBuf;
	ctx->texEnvBuf &= ~0x100FF;
	ctx->texEnvBuf |= (fogMode&7) | ((gasMode&1)<<3) | (zFlip ? BIT(16) : 0);
}

void C3D_FogColor(u32 color)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_TexEnvBuf;
	ctx->fogClr = color;
}

void C3D_FogLutBind(C3D_FogLut* lut)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	if (lut)
	{
		ctx->flags |= C3DiF_FogLut;
		ctx->fogLut = lut;
	} else
		ctx->flags &= ~C3DiF_FogLut;
}
