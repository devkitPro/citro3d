#include "internal.h"

C3D_FrameBuf* C3D_GetFrameBuf(void)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return NULL;

	ctx->flags |= C3DiF_FrameBuf;
	return &ctx->fb;
}

void C3D_SetFrameBuf(C3D_FrameBuf* fb)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	if (fb != &ctx->fb)
		memcpy(&ctx->fb, fb, sizeof(*fb));
	ctx->flags |= C3DiF_FrameBuf;
}

void C3D_FrameBufTex(C3D_FrameBuf* fb, C3D_Tex* tex, GPU_TEXFACE face, int level)
{
	C3D_FrameBufAttrib(fb, tex->width, tex->height, false);
	C3D_FrameBufColor(fb, C3D_TexGetImagePtr(tex,
		C3Di_TexIs2D(tex) ? tex->data : tex->cube->data[face], level, NULL),
		(GPU_COLORBUF)tex->fmt);
}

void C3Di_FrameBufBind(C3D_FrameBuf* fb)
{
	static const u8 colorFmtSizes[] = {2,1,0,0,0};
	u32 param[4] = { 0, 0, 0, 0 };

	GPUCMD_AddWrite(GPUREG_FRAMEBUFFER_INVALIDATE, 1);

	param[0] = osConvertVirtToPhys(fb->depthBuf) >> 3;
	param[1] = osConvertVirtToPhys(fb->colorBuf) >> 3;
	param[2] = 0x01000000 | (((u32)(fb->height-1) & 0xFFF) << 12) | (fb->width & 0xFFF);
	GPUCMD_AddIncrementalWrites(GPUREG_DEPTHBUFFER_LOC, param, 3);

	GPUCMD_AddWrite(GPUREG_RENDERBUF_DIM,       param[2]);
	GPUCMD_AddWrite(GPUREG_DEPTHBUFFER_FORMAT,  fb->depthFmt);
	GPUCMD_AddWrite(GPUREG_COLORBUFFER_FORMAT,  colorFmtSizes[fb->colorFmt] | ((u32)fb->colorFmt << 16));
	GPUCMD_AddWrite(GPUREG_FRAMEBUFFER_BLOCK32, fb->block32 ? 1 : 0);

	// Enable or disable color/depth buffers
	param[0] = param[1] = fb->colorBuf ? fb->colorMask : 0;
	param[2] = param[3] = fb->depthBuf ? fb->depthMask : 0;
	GPUCMD_AddIncrementalWrites(GPUREG_COLORBUFFER_READ, param, 4);
}
