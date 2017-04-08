#pragma once
#include "types.h"
#include <math.h>

typedef struct
{
	u32 data[128];
} C3D_FogLut;

static inline float FogLut_CalcZ(float depth, float near, float far)
{
	return far*near/(depth*(far-near)+near);
}

void FogLut_FromArray(C3D_FogLut* lut, const float data[256]);
void FogLut_Exp(C3D_FogLut* lut, float density, float gradient, float near, float far);

void C3D_FogGasMode(GPU_FOGMODE fogMode, GPU_GASMODE gasMode, bool zFlip);
void C3D_FogColor(u32 color);
void C3D_FogLutBind(C3D_FogLut* lut);
