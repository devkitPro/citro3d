#pragma once
#include "types.h"

typedef struct
{
	void* data;

	GPU_TEXCOLOR fmt : 4;
	size_t size : 28;

	u16 width, height;
	u32 param;
} C3D_Tex;

bool C3D_TexInit(C3D_Tex* tex, int width, int height, GPU_TEXCOLOR format);
bool C3D_TexInitVRAM(C3D_Tex* tex, int width, int height, GPU_TEXCOLOR format);
void C3D_TexUpload(C3D_Tex* tex, const void* data);
void C3D_TexSetFilter(C3D_Tex* tex, GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter);
void C3D_TexSetWrap(C3D_Tex* tex, GPU_TEXTURE_WRAP_PARAM wrapS, GPU_TEXTURE_WRAP_PARAM wrapT);
void C3D_TexBind(int unitId, C3D_Tex* tex);
void C3D_TexFlush(C3D_Tex* tex);
void C3D_TexDelete(C3D_Tex* tex);
