#include "internal.h"

static const u8 colorFmtSizes[] = {2,1,0,0,0};
static const u8 depthFmtSizes[] = {0,0,1,2};

u32 C3D_CalcColorBufSize(u32 width, u32 height, GPU_COLORBUF fmt)
{
	u32 size = width*height;
	return size*(2+colorFmtSizes[fmt]);
}

u32 C3D_CalcDepthBufSize(u32 width, u32 height, GPU_DEPTHBUF fmt)
{
	u32 size = width*height;
	return size*(2+depthFmtSizes[fmt]);
}

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

void C3D_FrameBufClear(C3D_FrameBuf* frameBuf, C3D_ClearBits clearBits, u32 clearColor, u32 clearDepth)
{
	u32 size = (u32)frameBuf->width * frameBuf->height;
	u32 cfs = colorFmtSizes[frameBuf->colorFmt];
	u32 dfs = depthFmtSizes[frameBuf->depthFmt];
	void* colorBufEnd = (u8*)frameBuf->colorBuf + size*(2+cfs);
	void* depthBufEnd = (u8*)frameBuf->depthBuf + size*(2+dfs);

	if (clearBits & C3D_CLEAR_COLOR)
	{
		if (clearBits & C3D_CLEAR_DEPTH)
			GX_MemoryFill(
				(u32*)frameBuf->colorBuf, clearColor, (u32*)colorBufEnd, BIT(0) | (cfs << 8),
				(u32*)frameBuf->depthBuf, clearDepth, (u32*)depthBufEnd, BIT(0) | (dfs << 8));
		else
			GX_MemoryFill(
				(u32*)frameBuf->colorBuf, clearColor, (u32*)colorBufEnd, BIT(0) | (cfs << 8),
				NULL, 0, NULL, 0);
	} else
		GX_MemoryFill(
			(u32*)frameBuf->depthBuf, clearDepth, (u32*)depthBufEnd, BIT(0) | (dfs << 8),
			NULL, 0, NULL, 0);
}

void C3D_FrameBufTransfer(C3D_FrameBuf* frameBuf, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags)
{
	u32* outputFrameBuf = (u32*)gfxGetFramebuffer(screen, side, NULL, NULL);
	u32 dim = GX_BUFFER_DIM((u32)frameBuf->width, (u32)frameBuf->height);
	GX_DisplayTransfer((u32*)frameBuf->colorBuf, dim, outputFrameBuf, dim, transferFlags);
}
