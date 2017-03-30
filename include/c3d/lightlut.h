#pragma once
#include "types.h"
#include <math.h>

typedef struct
{
	u32 data[256];
} C3D_LightLut;

typedef struct
{
	C3D_LightLut lut;
	float bias, scale;
} C3D_LightLutDA;

typedef float (* C3D_LightLutFunc)(float x, float param);
typedef float (* C3D_LightLutFuncDA)(float dist, float arg0, float arg1);

static inline float quadratic_dist_attn(float dist, float linear, float quad)
{
	return 1.0f / (1.0f + linear*dist + quad*dist*dist);
}

static inline float spot_step(float angle, float cutoff)
{
	return angle >= cutoff ? 1.0f : 0.0f;
}

void LightLut_FromArray(C3D_LightLut* lut, float* data);
void LightLut_FromFunc(C3D_LightLut* lut, C3D_LightLutFunc func, float param, bool negative);
void LightLutDA_Create(C3D_LightLutDA* lut, C3D_LightLutFuncDA func, float from, float to, float arg0, float arg1);

#define LightLut_Phong(lut, shininess) LightLut_FromFunc((lut), powf, (shininess), false)
#define LightLut_Spotlight(lut, angle) LightLut_FromFunc((lut), spot_step, cosf(angle), true)
#define LightLutDA_Quadratic(lut, from, to, linear, quad) LightLutDA_Create((lut), quadratic_dist_attn, (from), (to), (linear), (quad))
