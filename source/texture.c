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
		case GPU_LA8:
		case GPU_HILO8:
			return 2;
		case GPU_L8:
		case GPU_A8:
		case GPU_LA4:
			return 1;
		//case GPU_L4: // is actually 0.5
		//TODO: ETC is ??
		default:
			return 0;
	}
}

static inline bool addrIsVRAM(const void* addr)
{
	u32 vaddr = (u32)addr;
	return vaddr >= 0x1F000000 && vaddr < 0x1F600000;
}

static bool C3Di_TexInitCommon(C3D_Tex* tex, int width, int height, GPU_TEXCOLOR format, void* (*texAlloc)(size_t))
{
	if (tex->data) return false;

	u32 size = fmtSize(format);
	if (!size) return false;
	size *= width * height;

	tex->data = texAlloc(size);
	if (!tex->data) return false;

	tex->width = width;
	tex->height = height;
	tex->param = GPU_TEXTURE_MAG_FILTER(GPU_NEAREST) | GPU_TEXTURE_MIN_FILTER(GPU_NEAREST);
	tex->fmt = format;
	tex->size = size;
	return true;
}

bool C3D_TexInit(C3D_Tex* tex, int width, int height, GPU_TEXCOLOR format)
{
	return C3Di_TexInitCommon(tex, width, height, format, linearAlloc);
}

bool C3D_TexInitVRAM(C3D_Tex* tex, int width, int height, GPU_TEXCOLOR format)
{
	return C3Di_TexInitCommon(tex, width, height, format, vramAlloc);
}

void C3D_TexUpload(C3D_Tex* tex, const void* data)
{
	if (tex->data && !addrIsVRAM(tex->data))
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
	if (tex->data && !addrIsVRAM(tex->data))
		GSPGPU_FlushDataCache(tex->data, tex->size);
}

void C3D_TexDelete(C3D_Tex* tex)
{
	if (!tex->data) return;

	if (addrIsVRAM(tex->data))
		vramFree(tex->data);
	else
		linearFree(tex->data);

	tex->data = NULL;
}
