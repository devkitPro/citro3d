#pragma once
#include "types.h"
#include <math.h>

typedef struct
{
	u32 data[256];
} C3D_LightLut;

typedef float (* C3D_LightLutFunc)(float x, float param);

void LightLut_FromArray(C3D_LightLut* lut, float* data);
void LightLut_FromFunc(C3D_LightLut* lut, C3D_LightLutFunc func, float param, bool negative);

#define LightLut_Phong(lut, shininess) LightLut_FromFunc((lut), powf, (shininess), false)
