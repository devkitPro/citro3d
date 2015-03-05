#include "context.h"

C3D_Context __C3D_Context;

u32 C3Di_Float24(float f)
{
	if (!f) return 0;
	union { float t; u32 v; } u;
	u.t = f;
	u32 s = u.v >> 31;
	u32 exp = ((u.v >> 23) & 0xFF) - 0x40;
	u32 man = (u.v >> 7) & 0xFFFF;

	return (exp >= 0) ? (man | (exp << 16) | (s << 23)) : (s << 23);
}

static void C3Di_SetTex(GPU_TEXUNIT unit, C3D_Tex* tex)
{
	u32 reg[4];
	reg[0] = tex->fmt;
	reg[1] = osConvertVirtToPhys((u32)tex->data) >> 3;
	reg[2] = (u32)tex->width | ((u32)tex->height << 16);
	reg[3] = tex->param;

	switch (unit)
	{
		case GPU_TEXUNIT0:
			GPUCMD_AddWrite(GPUREG_TEXUNIT0_TYPE, reg[0]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT0_LOC, reg[1]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT0_DIM, reg[2]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT0_PARAM, reg[3]);
			break;
		case GPU_TEXUNIT1:
			GPUCMD_AddWrite(GPUREG_TEXUNIT1_TYPE, reg[0]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT1_LOC, reg[1]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT1_DIM, reg[2]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT1_PARAM, reg[3]);
			break;
		case GPU_TEXUNIT2:
			GPUCMD_AddWrite(GPUREG_TEXUNIT2_TYPE, reg[0]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT2_LOC, reg[1]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT2_DIM, reg[2]);
			GPUCMD_AddWrite(GPUREG_TEXUNIT2_PARAM, reg[3]);
			break;
	}
}

bool C3D_Init(size_t cmdBufSize)
{
	int i;
	C3D_Context* ctx = C3Di_GetContext();

	if (ctx->flags & C3DiF_Active)
		return false;

	ctx->cmdBufSize = cmdBufSize;
	ctx->cmdBuf = linearAlloc(cmdBufSize);
	if (!ctx->cmdBuf) return false;

	GPU_Reset(NULL, ctx->cmdBuf, ctx->cmdBufSize);
	GPUCMD_SetBuffer(ctx->cmdBuf, ctx->cmdBufSize, 0);

	ctx->flags = C3DiF_Active | C3DiF_TexEnvAll | C3DiF_Effect | C3DiF_TexAll;

	// TODO: replace with direct struct access
	C3D_DepthRange(-1.0f, 0.0f);
	C3D_CullFace(GPU_CULL_BACK_CCW);
	C3D_StencilTest(false, GPU_ALWAYS, 0x00, 0xFF, 0x00);
	C3D_StencilOp(GPU_KEEP, GPU_KEEP, GPU_KEEP);
	C3D_BlendingColor(0);
	C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
	C3D_AlphaTest(false, GPU_ALWAYS, 0x00);
	C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);

	for (i = 0; i < 3; i ++)
		ctx->tex[i] = NULL;

	for (i = 0; i < 6; i ++)
		TexEnv_Init(&ctx->texEnv[i]);

	return true;
}

void C3Di_UpdateContext(void)
{
	int i;
	C3D_Context* ctx = C3Di_GetContext();

	if (ctx->flags & C3DiF_NeedFinishDrawing)
	{
		ctx->flags &= ~C3DiF_NeedFinishDrawing;
		//GPU_FinishDrawing();
	}

	if (ctx->flags & C3DiF_AttrBuf)
	{
		ctx->flags &= ~C3DiF_AttrBuf;
		C3Di_AttrInfoBind(&ctx->attrInfo);
	}

	if (ctx->flags & C3DiF_Effect)
	{
		ctx->flags &= ~C3DiF_Effect;
		C3Di_EffectBind(&ctx->effect);
	}

	if (ctx->flags & C3DiF_TexAll)
	{
		GPU_TEXUNIT units = 0;

		for (i = 0; i < 3; i ++)
		{
			static const u8 parm[] = { GPU_TEXUNIT0, GPU_TEXUNIT1, GPU_TEXUNIT2 };

			if (ctx->tex[i])
			{
				units |= parm[i];
				if (ctx->flags & C3DiF_Tex(i))
					C3Di_SetTex(parm[i], ctx->tex[i]);
			}
		}

		ctx->flags &= ~C3DiF_TexAll;
		GPU_SetTextureEnable(units);
	}

	if (ctx->flags & C3DiF_TexEnvAll)
	{
		for (i = 0; i < 6; i ++)
		{
			if (!(ctx->flags & C3DiF_TexEnv(i))) continue;
			C3Di_TexEnvBind(i, &ctx->texEnv[i]);
		}
		ctx->flags &= ~C3DiF_TexEnvAll;
	}

	C3D_UpdateUniforms(GPU_VERTEX_SHADER);
	C3D_UpdateUniforms(GPU_GEOMETRY_SHADER);
}

void C3D_FlushAsync(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	if (ctx->flags & C3DiF_NeedFinishDrawing)
	{
		ctx->flags &= ~C3DiF_NeedFinishDrawing;
		GPU_FinishDrawing();
	}

	GPUCMD_Finalize();
	GPUCMD_FlushAndRun(NULL);
	GPUCMD_SetBuffer(ctx->cmdBuf, ctx->cmdBufSize, 0);
}

void C3D_Fini(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	linearFree(ctx->cmdBuf);
	ctx->flags = 0;
}
