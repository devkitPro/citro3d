#include "internal.h"

void C3D_ProcTexInit(C3D_ProcTex* pt, int offset, int width)
{
	memset(pt, 0, sizeof(*pt));
	pt->offset = offset;
	pt->width = width;
	pt->unknown1 = 0x60;
	pt->unknown2 = 0xE0C080;
}

void C3D_ProcTexNoiseCoefs(C3D_ProcTex* pt, int mode, float amplitude, float frequency, float phase)
{
	u16 f16_ampl = (s32)(amplitude*0x1000);
	u16 f16_freq = f32tof16(frequency);
	u16 f16_phase = f32tof16(phase);
	pt->enableNoise = true;
	if (mode & C3D_ProcTex_U)
	{
		pt->uNoiseAmpl  = f16_ampl;
		pt->uNoiseFreq  = f16_freq;
		pt->uNoisePhase = f16_phase;
	}
	if (mode & C3D_ProcTex_V)
	{
		pt->vNoiseAmpl  = f16_ampl;
		pt->vNoiseFreq  = f16_freq;
		pt->vNoisePhase = f16_phase;
	}
}

void C3D_ProcTexLodBias(C3D_ProcTex* pt, float bias)
{
	u32 f16_bias = f32tof16(bias);
	pt->lodBiasLow  = f16_bias;
	pt->lodBiasHigh = f16_bias>>8;
}

void C3D_ProcTexBind(int texCoordId, C3D_ProcTex* pt)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_TexStatus;
	ctx->texConfig &= ~(7<<8);
	ctx->procTex = pt;
	if (pt)
	{
		ctx->flags |= C3DiF_ProcTex;
		ctx->texConfig |= BIT(10) | ((texCoordId&3)<<8);
	} else
		ctx->flags &= ~C3DiF_ProcTex;
}

static inline int lutid2idx(GPU_PROCTEX_LUTID id)
{
	switch (id)
	{
		case GPU_LUT_NOISE:    return 0;
		case GPU_LUT_RGBMAP:   return 1;
		case GPU_LUT_ALPHAMAP: return 2;
		default:               return -1;
	}
}

void C3D_ProcTexLutBind(GPU_PROCTEX_LUTID id, C3D_ProcTexLut* lut)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	int idx = lutid2idx(id);
	if (idx < 0)
		return;

	ctx->procTexLut[idx] = lut;
	if (lut)
		ctx->flags |= C3DiF_ProcTexLut(idx);
	else
		ctx->flags &= ~C3DiF_ProcTexLut(idx);
}

static inline float clampLut(float val)
{
	if (val < 0.0f) return 0.0f;
	if (val > 1.0f) return 1.0f;
	return val;
}

void ProcTexLut_FromArray(C3D_ProcTexLut* lut, const float in[129])
{
	int i;
	for (i = 0; i < 128; i ++)
	{
		u32 cur  = 0xFFF*clampLut(in[i]);
		u32 next = 0xFFF*clampLut(in[i+1]);
		u32 diff = (next-cur)&0xFFF;
		(*lut)[i] = cur | (diff<<12);
	}
}

void C3D_ProcTexColorLutBind(C3D_ProcTexColorLut* lut)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->procTexColorLut = lut;
	if (lut)
		ctx->flags |= C3DiF_ProcTexColorLut;
	else
		ctx->flags &= ~C3DiF_ProcTexColorLut;
}

static inline u32 calc_diff(u32 cur, u32 next, int pos)
{
	cur = (cur>>pos)&0xFF;
	next = (next>>pos)&0xFF;
	u32 diff = (((s32)next-(s32)cur)>>1)&0xFF;
	return diff<<pos;
}

void ProcTexColorLut_Write(C3D_ProcTexColorLut* out, const u32* in, int offset, int width)
{
	int i;
	memcpy(&out->color[offset], in, 4*width);
	for (i = 0; i < (width-1); i ++)
	{
		u32 cur = in[i];
		u32 next = in[i+1];
		out->diff[offset+i] =
			calc_diff(cur,next,0)  |
			calc_diff(cur,next,8)  |
			calc_diff(cur,next,16) |
			calc_diff(cur,next,24);
	}
	out->diff[offset+width-1] = 0;
}

void C3Di_ProcTexUpdate(C3D_Context* ctx)
{
	if (!(ctx->texConfig & BIT(10)))
		return;

	if (ctx->flags & C3DiF_ProcTex)
	{
		ctx->flags &= ~C3DiF_ProcTex;
		if (ctx->procTex)
			GPUCMD_AddIncrementalWrites(GPUREG_TEXUNIT3_PROCTEX0, (u32*)ctx->procTex, 6);
	}
	if (ctx->flags & C3DiF_ProcTexLutAll)
	{
		int i;
		for (i = 0; i < 3; i ++)
		{
			int j = i ? (i+1) : 0;
			if (!(ctx->flags & C3DiF_ProcTexLut(i)) || !ctx->procTexLut[i])
				continue;

			GPUCMD_AddWrite(GPUREG_PROCTEX_LUT, j<<8);
			GPUCMD_AddWrites(GPUREG_PROCTEX_LUT_DATA0, *ctx->procTexLut[i], 128);
		}
		ctx->flags &= ~C3DiF_ProcTexLutAll;
	}
	if (ctx->flags & C3DiF_ProcTexColorLut)
	{
		ctx->flags &= ~C3DiF_ProcTexColorLut;
		if (ctx->procTexColorLut)
		{
			GPUCMD_AddWrite(GPUREG_PROCTEX_LUT, GPU_LUT_COLOR<<8);
			GPUCMD_AddWrites(GPUREG_PROCTEX_LUT_DATA0, ctx->procTexColorLut->color, 256);
			GPUCMD_AddWrite(GPUREG_PROCTEX_LUT, GPU_LUT_COLORDIF<<8);
			GPUCMD_AddWrites(GPUREG_PROCTEX_LUT_DATA0, ctx->procTexColorLut->diff, 256);
		}
	}
}

void C3Di_ProcTexDirty(C3D_Context* ctx)
{
	int i;
	if (!ctx->procTex)
		return;

	ctx->flags |= C3DiF_ProcTex;
	if (ctx->procTexColorLut)
		ctx->flags |= C3DiF_ProcTexColorLut;
	for (i = 0; i < 3; i ++)
		if (ctx->procTexLut[i])
			ctx->flags |= C3DiF_ProcTexLut(i);
}
