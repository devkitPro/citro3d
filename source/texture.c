#include "context.h"
#include <string.h>

static inline size_t fmtSize(GPU_TEXCOLOR fmt)
{
	switch (fmt)
	{
		case GPU_RGBA8:
			return 4;
		case GPU_RGB8:
			return 3;
		case GPU_RGBA5551:
		case GPU_RGB565:
		case GPU_RGBA4:
			return 2;
		default:
			return 0;
	}
}

bool C3D_TexInit(C3D_Tex* tex, int width, int height, GPU_TEXCOLOR format)
{
	if (tex->data) return false;

	tex->size = fmtSize(format);
	if (!tex->size) return false;
	tex->size *= width * height;

	tex->data = linearMemAlign(tex->size, 0x80);
	if (!tex->data) return false;

	tex->width = width;
	tex->height = height;
	tex->param = GPU_TEXTURE_MAG_FILTER(GPU_NEAREST) | GPU_TEXTURE_MIN_FILTER(GPU_NEAREST);
	tex->fmt = format;
	return true;
}

void C3D_TexUpload(C3D_Tex* tex, const void* data)
{
	if (tex->data)
		memcpy(tex->data, data, tex->size);
}

void C3D_TexSetFilter(C3D_Tex* tex, GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter)
{
	tex->param &= ~(GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR));
	tex->param |= GPU_TEXTURE_MAG_FILTER(magFilter) | GPU_TEXTURE_MIN_FILTER(minFilter);
}

void C3D_TexSetWrap(C3D_Tex* tex, GPU_TEXTURE_WRAP_PARAM wrapS, GPU_TEXTURE_WRAP_PARAM wrapT)
{
	tex->param &= ~(GPU_TEXTURE_WRAP_S(3) | GPU_TEXTURE_WRAP_T(3));
	tex->param |= GPU_TEXTURE_WRAP_S(wrapS) | GPU_TEXTURE_WRAP_T(wrapT);
}

void C3D_TexBind(int unitId, C3D_Tex* tex)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	ctx->flags |= C3DiF_Tex(unitId);
	ctx->tex[unitId] = tex;
}

void C3D_TexFlush(C3D_Tex* tex)
{
	if (tex->data)
		GSPGPU_FlushDataCache(NULL, tex->data, tex->size);
}

void C3D_TexDelete(C3D_Tex* tex)
{
	if (!tex->data) return;

	linearFree(tex->data);
	tex->data = NULL;
}
