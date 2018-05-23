#pragma once
#include "types.h"
#include <math.h>

typedef struct
{
	u32 data[128];
} C3D_FogLut;

typedef struct
{
	u32 diff[8];
	u32 color[8];
} C3D_GasLut;

static inline float FogLut_CalcZ(float depth, float near, float far)
{
	return far*near/(depth*(far-near)+near);
}

void FogLut_FromArray(C3D_FogLut* lut, const float data[256]);
void FogLut_Exp(C3D_FogLut* lut, float density, float gradient, float near, float far);

void C3D_FogGasMode(GPU_FOGMODE fogMode, GPU_GASMODE gasMode, bool zFlip);
void C3D_FogColor(u32 color);
void C3D_FogLutBind(C3D_FogLut* lut);

void GasLut_FromArray(C3D_GasLut* lut, const u32 data[9]);

void C3D_GasBeginAcc(void);
void C3D_GasDeltaZ(float value);

void C3D_GasAccMax(float value);
void C3D_GasAttn(float value);
void C3D_GasLightPlanar(float min, float max, float attn);
void C3D_GasLightView(float min, float max, float attn);
void C3D_GasLightDirection(float dotp);
void C3D_GasLutInput(GPU_GASLUTINPUT input);
void C3D_GasLutBind(C3D_GasLut* lut);
