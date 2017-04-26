#include "internal.h"

void TexEnv_Init(C3D_TexEnv* env)
{
	env->srcRgb = GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0);
	env->srcAlpha = env->srcRgb;
	env->opRgb = GPU_TEVOPERANDS(0,0,0);
	env->opAlpha = env->opRgb;
	env->funcRgb = GPU_REPLACE;
	env->funcAlpha = env->funcRgb;
	env->color = 0xFFFFFFFF;
	env->scaleRgb = GPU_TEVSCALE_1;
	env->scaleAlpha = GPU_TEVSCALE_1;
}

C3D_TexEnv* C3D_GetTexEnv(int id)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return NULL;

	ctx->flags |= C3DiF_TexEnv(id);
	return &ctx->texEnv[id];
}

void C3D_SetTexEnv(int id, C3D_TexEnv* env)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	memcpy(&ctx->texEnv[id], env, sizeof(*env));
	ctx->flags |= C3DiF_TexEnv(id);
}

void C3D_DirtyTexEnv(C3D_TexEnv* env)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	u32 id = env-ctx->texEnv;
	if (id < 6)
		ctx->flags |= C3DiF_TexEnv(id);
}

void C3Di_TexEnvBind(int id, C3D_TexEnv* env)
{
	if (id >= 4) id += 2;
	GPUCMD_AddIncrementalWrites(GPUREG_TEXENV0_SOURCE + id*8, (u32*)env, sizeof(C3D_TexEnv)/sizeof(u32));
}

void C3D_TexEnvBufUpdate(int mode, int mask)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	u32 val = ctx->texEnvBuf;
	mask &= 0xF;

	if (mode & C3D_RGB)
	{
		val &= ~(0xF << 8);
		val |= mask << 8;
	}

	if (mode & C3D_Alpha)
	{
		val &= ~(0xF << 12);
		val |= mask << 12;
	}

	ctx->texEnvBuf = val;
	ctx->flags |= C3DiF_TexEnvBuf;
}

void C3D_TexEnvBufColor(u32 color)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->texEnvBufClr = color;
	ctx->flags |= C3DiF_TexEnvBuf;
}
