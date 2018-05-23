#include "internal.h"

static inline C3D_Effect* getEffect()
{
	C3D_Context* ctx = C3Di_GetContext();
	ctx->flags |= C3DiF_Effect;
	return &ctx->effect;
}

void C3D_DepthMap(bool bIsZBuffer, float zScale, float zOffset)
{
	C3D_Effect* e = getEffect();
	e->zBuffer = bIsZBuffer;
	e->zScale  = f32tof24(zScale);
	e->zOffset = f32tof24(zOffset);
}

void C3D_CullFace(GPU_CULLMODE mode)
{
	C3D_Effect* e = getEffect();
	e->cullMode = mode;
}

void C3D_StencilTest(bool enable, GPU_TESTFUNC function, int ref, int inputMask, int writeMask)
{
	C3D_Effect* e = getEffect();
	e->stencilMode = (!!enable) | ((function & 7) << 4) | (writeMask << 8) | (ref << 16) | (inputMask << 24);
}

void C3D_StencilOp(GPU_STENCILOP sfail, GPU_STENCILOP dfail, GPU_STENCILOP pass)
{
	C3D_Effect* e = getEffect();
	e->stencilOp = sfail | (dfail << 4) | (pass << 8);
}

void C3D_BlendingColor(u32 color)
{
	C3D_Effect* e = getEffect();
	e->blendClr = color;
}

void C3D_EarlyDepthTest(bool enable, GPU_EARLYDEPTHFUNC function, u32 ref)
{
	C3D_Effect* e = getEffect();
	e->earlyDepth = enable;
	e->earlyDepthFunc = function;
	e->earlyDepthRef = ref;
}

void C3D_DepthTest(bool enable, GPU_TESTFUNC function, GPU_WRITEMASK writemask)
{
	C3D_Effect* e = getEffect();
	e->depthTest = (!!enable) | ((function & 7) << 4) | (writemask << 8);
}

void C3D_AlphaTest(bool enable, GPU_TESTFUNC function, int ref)
{
	C3D_Effect* e = getEffect();
	e->alphaTest = (!!enable) | ((function & 7) << 4) | (ref << 8);
}

void C3D_AlphaBlend(GPU_BLENDEQUATION colorEq, GPU_BLENDEQUATION alphaEq, GPU_BLENDFACTOR srcClr, GPU_BLENDFACTOR dstClr, GPU_BLENDFACTOR srcAlpha, GPU_BLENDFACTOR dstAlpha)
{
	C3D_Effect* e = getEffect();
	e->alphaBlend = colorEq | (alphaEq << 8) | (srcClr << 16) | (dstClr << 20) | (srcAlpha << 24) | (dstAlpha << 28);
	e->fragOpMode &= ~0xFF00;
	e->fragOpMode |= 0x0100;
}

void C3D_ColorLogicOp(GPU_LOGICOP op)
{
	C3D_Effect* e = getEffect();
	e->fragOpMode &= ~0xFF00;
	e->clrLogicOp = op;
}

void C3D_FragOpMode(GPU_FRAGOPMODE mode)
{
	C3D_Effect* e = getEffect();
	e->fragOpMode &= ~0xFF00FF;
	e->fragOpMode |= 0xE40000 | mode;
}

void C3D_FragOpShadow(float scale, float bias)
{
	C3D_Effect* e = getEffect();
	e->fragOpShadow = f32tof16(scale+bias) | (f32tof16(-scale)<<16);
}

void C3Di_EffectBind(C3D_Effect* e)
{
	GPUCMD_AddWrite(GPUREG_DEPTHMAP_ENABLE, e->zBuffer ? 1 : 0);
	GPUCMD_AddWrite(GPUREG_FACECULLING_CONFIG, e->cullMode & 0x3);
	GPUCMD_AddIncrementalWrites(GPUREG_DEPTHMAP_SCALE, (u32*)&e->zScale, 2);
	GPUCMD_AddIncrementalWrites(GPUREG_FRAGOP_ALPHA_TEST, (u32*)&e->alphaTest, 4);
	GPUCMD_AddMaskedWrite(GPUREG_GAS_DELTAZ_DEPTH, 0x8, (u32)GPU_MAKEGASDEPTHFUNC((e->depthTest>>4)&7) << 24);
	GPUCMD_AddWrite(GPUREG_BLEND_COLOR, e->blendClr);
	GPUCMD_AddWrite(GPUREG_BLEND_FUNC, e->alphaBlend);
	GPUCMD_AddWrite(GPUREG_LOGIC_OP, e->clrLogicOp);
	GPUCMD_AddMaskedWrite(GPUREG_COLOR_OPERATION, 7, e->fragOpMode);
	GPUCMD_AddWrite(GPUREG_FRAGOP_SHADOW, e->fragOpShadow);
	GPUCMD_AddMaskedWrite(GPUREG_EARLYDEPTH_TEST1, 1, e->earlyDepth ? 1 : 0);
	GPUCMD_AddWrite(GPUREG_EARLYDEPTH_TEST2, e->earlyDepth ? 1 : 0);
	GPUCMD_AddMaskedWrite(GPUREG_EARLYDEPTH_FUNC, 1, e->earlyDepthFunc);
	GPUCMD_AddMaskedWrite(GPUREG_EARLYDEPTH_DATA, 0x7, e->earlyDepthRef);
}
