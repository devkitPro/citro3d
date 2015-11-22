#pragma once
#include <c3d/base.h>
#include <c3d/uniforms.h>
#include <c3d/attribs.h>
#include <c3d/buffers.h>
#include <c3d/texenv.h>
#include <c3d/effect.h>
#include <c3d/texture.h>
#include <c3d/light.h>
#include <c3d/renderbuffer.h>

typedef struct
{
	u32 zScale, zOffset;
	GPU_CULLMODE cullMode;

	u32 alphaTest;
	u32 stencilMode, stencilOp;
	u32 depthTest;

	u32 blendClr;
	u32 alphaBlend;
} C3D_Effect;

typedef struct
{
	void* cmdBuf;
	size_t cmdBufSize;

	u32 flags;
	shaderProgram_s* program;

	C3D_AttrInfo attrInfo;
	C3D_BufInfo bufInfo;
	C3D_Effect effect;
	C3D_LightEnv* lightEnv;

	C3D_Tex* tex[3];
	C3D_TexEnv texEnv[6];

	u32 texEnvBuf, texEnvBufClr;

	C3D_RenderBuf* rb;
	u32 viewport[5];
	u32 scissor[3];

	u16 fixedAttribDirty, fixedAttribEverDirty;
	C3D_FVec fixedAttribs[12];

} C3D_Context;

enum
{
	C3DiF_Active = BIT(0),
	C3DiF_DrawUsed = BIT(1),
	C3DiF_AttrInfo = BIT(2),
	C3DiF_BufInfo = BIT(3),
	C3DiF_Effect = BIT(4),
	C3DiF_RenderBuf = BIT(5),
	C3DiF_Viewport = BIT(6),
	C3DiF_Scissor = BIT(7),
	C3DiF_Program = BIT(8),
	C3DiF_TexEnvBuf = BIT(9),
	C3DiF_LightEnv = BIT(10),
	C3DiF_VshCode = BIT(11),
	C3DiF_GshCode = BIT(12),

#define C3DiF_Tex(n) BIT(23+(n))
	C3DiF_TexAll = 7 << 23,
#define C3DiF_TexEnv(n) BIT(26+(n))
	C3DiF_TexEnvAll = 0x3F << 26,
};

static inline C3D_Context* C3Di_GetContext(void)
{
	extern C3D_Context __C3D_Context;
	return &__C3D_Context;
}

void C3Di_UpdateContext(void);
void C3Di_AttrInfoBind(C3D_AttrInfo* info);
void C3Di_BufInfoBind(C3D_BufInfo* info);
void C3Di_TexEnvBind(int id, C3D_TexEnv* env);
void C3Di_EffectBind(C3D_Effect* effect);
void C3Di_RenderBufBind(C3D_RenderBuf* rb);

void C3Di_LightMtlBlend(C3D_Light* light);

void C3Di_DirtyUniforms(GPU_SHADER_TYPE type);
void C3Di_LoadShaderUniforms(shaderInstance_s* si);
void C3Di_ClearShaderUniforms(GPU_SHADER_TYPE type);
