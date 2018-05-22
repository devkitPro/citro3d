#pragma once
#include "types.h"

typedef struct
{
	u16 srcRgb, srcAlpha;
	union
	{
		u32 opAll;
		struct { u32 opRgb:12, opAlpha:12; };
	};
	u16 funcRgb, funcAlpha;
	u32 color;
	u16 scaleRgb, scaleAlpha;
} C3D_TexEnv;

typedef enum
{
	C3D_RGB = BIT(0),
	C3D_Alpha = BIT(1),
	C3D_Both = C3D_RGB | C3D_Alpha,
} C3D_TexEnvMode;

C3D_TexEnv* C3D_GetTexEnv(int id);
void C3D_SetTexEnv(int id, C3D_TexEnv* env);
void C3D_DirtyTexEnv(C3D_TexEnv* env);

void C3D_TexEnvBufUpdate(int mode, int mask);
void C3D_TexEnvBufColor(u32 color);

static inline void C3D_TexEnvInit(C3D_TexEnv* env)
{
	env->srcRgb     = GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0);
	env->srcAlpha   = env->srcRgb;
	env->opAll      = 0;
	env->funcRgb    = GPU_REPLACE;
	env->funcAlpha  = env->funcRgb;
	env->color      = 0xFFFFFFFF;
	env->scaleRgb   = GPU_TEVSCALE_1;
	env->scaleAlpha = GPU_TEVSCALE_1;
}

#ifdef __cplusplus
#define _C3D_DEFAULT(x) = x
#else
#define _C3D_DEFAULT(x)
#endif

static inline void C3D_TexEnvSrc(C3D_TexEnv* env, C3D_TexEnvMode mode,
	GPU_TEVSRC s1,
	GPU_TEVSRC s2 _C3D_DEFAULT(GPU_PRIMARY_COLOR),
	GPU_TEVSRC s3 _C3D_DEFAULT(GPU_PRIMARY_COLOR))
{
	int param = GPU_TEVSOURCES((int)s1, (int)s2, (int)s3);
	if ((int)mode & C3D_RGB)
		env->srcRgb = param;
	if ((int)mode & C3D_Alpha)
		env->srcAlpha = param;
}

static inline void C3D_TexEnvOpRgb(C3D_TexEnv* env,
	GPU_TEVOP_RGB o1,
	GPU_TEVOP_RGB o2 _C3D_DEFAULT(GPU_TEVOP_RGB_SRC_COLOR),
	GPU_TEVOP_RGB o3 _C3D_DEFAULT(GPU_TEVOP_RGB_SRC_COLOR))
{
	env->opRgb = GPU_TEVOPERANDS((int)o1, (int)o2, (int)o3);
}

static inline void C3D_TexEnvOpAlpha(C3D_TexEnv* env,
	GPU_TEVOP_A o1,
	GPU_TEVOP_A o2 _C3D_DEFAULT(GPU_TEVOP_A_SRC_ALPHA),
	GPU_TEVOP_A o3 _C3D_DEFAULT(GPU_TEVOP_A_SRC_ALPHA))
{
	env->opAlpha = GPU_TEVOPERANDS((int)o1, (int)o2, (int)o3);
}

static inline void C3D_TexEnvFunc(C3D_TexEnv* env, C3D_TexEnvMode mode, GPU_COMBINEFUNC param)
{
	if ((int)mode & C3D_RGB)
		env->funcRgb = param;
	if ((int)mode & C3D_Alpha)
		env->funcAlpha = param;
}

static inline void C3D_TexEnvColor(C3D_TexEnv* env, u32 color)
{
	env->color = color;
}

static inline void C3D_TexEnvScale(C3D_TexEnv* env, int mode, GPU_TEVSCALE param)
{
	if (mode & C3D_RGB)
		env->scaleRgb = param;
	if (mode & C3D_Alpha)
		env->scaleAlpha = param;
}

#undef _C3D_DEFAULT
