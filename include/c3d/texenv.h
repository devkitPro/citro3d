#pragma once
#include "types.h"

typedef struct
{
	u16 srcRgb, srcAlpha;
	u16 opRgb, opAlpha;
	u16 funcRgb, funcAlpha;
	u32 color;
	u32 unknown;
} C3D_TexEnv;

enum
{
	C3D_RGB = BIT(0),
	C3D_Alpha = BIT(1),
	C3D_Both = C3D_RGB | C3D_Alpha,
};

void TexEnv_Init(C3D_TexEnv* env);

C3D_TexEnv* C3D_GetTexEnv(int id);
void C3D_SetTexEnv(int id, C3D_TexEnv* env);

static inline void C3D_TexEnvSrc(C3D_TexEnv* env, int mode, int s1, int s2, int s3)
{
	int param = GPU_TEVSOURCES(s1, s2, s3);
	if (mode & C3D_RGB)
		env->srcRgb = param;
	if (mode & C3D_Alpha)
		env->srcAlpha = param;
}

static inline void C3D_TexEnvOp(C3D_TexEnv* env, int mode, int o1, int o2, int o3)
{
	int param = GPU_TEVOPERANDS(o1, o2, o3);
	if (mode & C3D_RGB)
		env->opRgb = param;
	if (mode & C3D_Alpha)
		env->opAlpha = param;
}

static inline void C3D_TexEnvFunc(C3D_TexEnv* env, int mode, int param)
{
	if (mode & C3D_RGB)
		env->funcRgb = param;
	if (mode & C3D_Alpha)
		env->funcAlpha = param;
}

static inline void C3D_TexEnvColor(C3D_TexEnv* env, u32 color)
{
	env->color = color;
}
