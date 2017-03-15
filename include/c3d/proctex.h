#pragma once
#include "types.h"

typedef struct
{
	u32 color[256];
	u32 diff[256];
} C3D_ProcTexColorLut;

typedef struct
{
	union
	{
		u32 proctex0;
		struct
		{
			u32 uClamp         : 3;
			u32 vClamp         : 3;
			u32 rgbFunc        : 4;
			u32 alphaFunc      : 4;
			bool alphaSeparate : 1;
			bool enableNoise   : 1;
			u32 uShift         : 2;
			u32 vShift         : 2;
			u32 lodBiasLow     : 8;
		};
	};
	union
	{
		u32 proctex1;
		struct
		{
			u16 uNoiseAmpl;
			u16 uNoisePhase;
		};
	};
	union
	{
		u32 proctex2;
		struct
		{
			u16 vNoiseAmpl;
			u16 vNoisePhase;
		};
	};
	union
	{
		u32 proctex3;
		struct
		{
			u16 uNoiseFreq;
			u16 vNoiseFreq;
		};
	};
	union
	{
		u32 proctex4;
		struct
		{
			u32 minFilter   : 3;
			u32 unknown1    : 8;
			u32 width       : 8;
			u32 lodBiasHigh : 8;
		};
	};
	union
	{
		u32 proctex5;
		struct
		{
			u32 offset   : 8;
			u32 unknown2 : 24;
		};
	};
} C3D_ProcTex;

enum
{
	C3D_ProcTex_U  = BIT(0),
	C3D_ProcTex_V  = BIT(1),
	C3D_ProcTex_UV = C3D_ProcTex_U | C3D_ProcTex_V,
};

void C3D_ProcTexInit(C3D_ProcTex* pt, int offset, int length);
void C3D_ProcTexNoiseCoefs(C3D_ProcTex* pt, int mode, float amplitude, float frequency, float phase);
void C3D_ProcTexLodBias(C3D_ProcTex* pt, float bias);
void C3D_ProcTexBind(int texCoordId, C3D_ProcTex* pt);

// GPU_LUT_NOISE, GPU_LUT_RGBMAP, GPU_LUT_ALPHAMAP
typedef u32 C3D_ProcTexLut[128];
void C3D_ProcTexLutBind(GPU_PROCTEX_LUTID id, C3D_ProcTexLut* lut);
void ProcTexLut_FromArray(C3D_ProcTexLut* lut, const float in[129]);

void C3D_ProcTexColorLutBind(C3D_ProcTexColorLut* lut);
void ProcTexColorLut_Write(C3D_ProcTexColorLut* out, const u32* in, int offset, int length);

static inline void C3D_ProcTexClamp(C3D_ProcTex* pt, GPU_PROCTEX_CLAMP u, GPU_PROCTEX_CLAMP v)
{
	pt->uClamp = u;
	pt->vClamp = v;
}

static inline void C3D_ProcTexCombiner(C3D_ProcTex* pt, bool separate, GPU_PROCTEX_MAPFUNC rgb, GPU_PROCTEX_MAPFUNC alpha)
{
	pt->alphaSeparate = separate;
	pt->rgbFunc = rgb;
	if (separate)
		pt->alphaFunc = alpha;
}

static inline void C3D_ProcTexNoiseEnable(C3D_ProcTex* pt, bool enable)
{
	pt->enableNoise = enable;
}

static inline void C3D_ProcTexShift(C3D_ProcTex* pt, GPU_PROCTEX_SHIFT u, GPU_PROCTEX_SHIFT v)
{
	pt->uShift = u;
	pt->vShift = v;
}

static inline void C3D_ProcTexFilter(C3D_ProcTex* pt, GPU_PROCTEX_FILTER min)
{
	pt->minFilter = min;
}
