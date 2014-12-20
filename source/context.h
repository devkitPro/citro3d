#pragma once
#include <c3d/base.h>
#include <c3d/uniforms.h>
#include <c3d/buffers.h>
#include <c3d/attribs.h>
#include <c3d/texenv.h>
#include <c3d/effect.h>
#include <c3d/texture.h>

typedef struct
{
	u32 drNear, drFar;
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

	u32 vboPos;
	C3D_AttrInfo attrInfo;
	C3D_Effect effect;

	C3D_Tex* tex[3];
	C3D_TexEnv texEnv[6];

} C3D_Context;

enum
{
	C3DiF_Active = BIT(0),
	C3DiF_NeedFinishDrawing = BIT(1),
	C3DiF_AttrBuf = BIT(2),
	C3DiF_Effect = BIT(3),

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

u32 C3Di_Float24(float f);
void C3Di_UpdateContext(void);
void C3Di_IBOBind(C3D_IBO* ibo);
void C3Di_AttrInfoBind(C3D_AttrInfo* info);
void C3Di_TexEnvBind(int id, C3D_TexEnv* env);
void C3Di_EffectBind(C3D_Effect* effect);
