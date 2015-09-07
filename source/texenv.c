#include <c3d/texenv.h>
#include <string.h>
#include "context.h"

void TexEnv_Init(C3D_TexEnv* env)
{
	env->srcRgb = GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0);
	env->srcAlpha = env->srcRgb;
	env->opRgb = GPU_TEVOPERANDS(0,0,0);
	env->opAlpha = env->opRgb;
	env->funcRgb = GPU_REPLACE;
	env->funcAlpha = env->funcRgb;
	env->color = 0xFFFFFFFF;
	env->unknown = 0;
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

void C3Di_TexEnvBind(int id, C3D_TexEnv* env)
{
	if (id >= 4) id += 2;
	GPUCMD_AddIncrementalWrites(GPUREG_TEXENV0_SOURCE + id*8, (u32*)env, sizeof(C3D_TexEnv)/sizeof(u32));
}
