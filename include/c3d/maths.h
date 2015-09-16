#pragma once
#include "types.h"
#include <string.h>
#include <math.h>

// See http://tauday.com/tau-manifesto
//#define M_TAU 6.28318530717958647693
#define M_TAU (2*M_PI)

#define C3D_Angle(_angle) ((_angle)*M_TAU)
#define C3D_AspectRatioTop (400.0f / 240.0f)
#define C3D_AspectRatioBot (320.0f / 240.0f)

static inline float FVec_DP4(const C3D_FVec* a, const C3D_FVec* b)
{
	return a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w;
}

static inline float FVec_Mod4(const C3D_FVec* a)
{
	return sqrtf(FVec_DP4(a,a));
}

static inline void FVec_Norm4(C3D_FVec* vec)
{
	float m = FVec_Mod4(vec);
	vec->x /= m;
	vec->y /= m;
	vec->z /= m;
	vec->w /= m;
}

static inline float FVec_DP3(const C3D_FVec* a, const C3D_FVec* b)
{
	return a->x*b->x + a->y*b->y + a->z*b->z;
}

static inline float FVec_Mod3(const C3D_FVec* a)
{
	return sqrtf(FVec_DP3(a,a));
}

static inline void FVec_Norm3(C3D_FVec* vec)
{
	float m = FVec_Mod3(vec);
	vec->x /= m;
	vec->y /= m;
	vec->z /= m;
	vec->w  = 0.0f;
}

static inline void Mtx_Zeros(C3D_Mtx* out)
{
	memset(out, 0, sizeof(*out));
}

static inline void Mtx_Copy(C3D_Mtx* out, const C3D_Mtx* in)
{
	memcpy(out, in, sizeof(*out));
}

void Mtx_Identity(C3D_Mtx* out);
void Mtx_Multiply(C3D_Mtx* out, const C3D_Mtx* a, const C3D_Mtx* b);

void Mtx_Translate(C3D_Mtx* mtx, float x, float y, float z);
void Mtx_Scale(C3D_Mtx* mtx, float x, float y, float z);
void Mtx_RotateX(C3D_Mtx* mtx, float angle, bool bRightSide);
void Mtx_RotateY(C3D_Mtx* mtx, float angle, bool bRightSide);
void Mtx_RotateZ(C3D_Mtx* mtx, float angle, bool bRightSide);

// Special versions of the projection matrices that take the 3DS' screen orientation into account
void Mtx_OrthoTilt(C3D_Mtx* mtx, float left, float right, float bottom, float top, float near, float far);
void Mtx_PerspTilt(C3D_Mtx* mtx, float fovy, float aspect, float near, float far);
void Mtx_PerspStereoTilt(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, float iod, float screen);
