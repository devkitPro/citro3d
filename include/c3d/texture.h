#pragma once
#include "types.h"

typedef struct
{
	void* data;

	GPU_TEXCOLOR fmt : 4;
	size_t size : 28;

	u16 width, height;
	u32 param;
	u32 border;
	union
	{
		u32 lodParam;
		struct
		{
			u16 lodBias;
			u8 maxLevel;
			u8 minLevel;
		};
	};
} C3D_Tex;

typedef struct ALIGN(8)
{
	u16 width;
	u16 height;
	u8 maxLevel                 : 4;
	GPU_TEXCOLOR format         : 4;
	GPU_TEXTURE_MODE_PARAM type : 3;
	bool onVram                 : 1;
} C3D_TexInitParams;

bool C3D_TexInitWithParams(C3D_Tex* tex, C3D_TexInitParams p);
void* C3D_TexGetLevel(C3D_Tex* tex, int level, u32* size);
void C3D_TexUploadLevel(C3D_Tex* tex, const void* data, int level);
void C3D_TexGenerateMipmap(C3D_Tex* tex);
void C3D_TexBind(int unitId, C3D_Tex* tex);
void C3D_TexFlush(C3D_Tex* tex);
void C3D_TexDelete(C3D_Tex* tex);

static inline int C3D_TexCalcMaxLevel(u32 width, u32 height)
{
	return (31-__builtin_clz(width < height ? width : height)) - 3; // avoid sizes smaller than 8
}

static inline bool C3D_TexInit(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_2D, false });
}

static inline bool C3D_TexInitMipmap(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex,
		(C3D_TexInitParams){ width, height, (u8)C3D_TexCalcMaxLevel(width, height), format, GPU_TEX_2D, false });
}

static inline bool C3D_TexInitVRAM(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_2D, true });
}

static inline void C3D_TexUpload(C3D_Tex* tex, const void* data)
{
	C3D_TexUploadLevel(tex, data, 0);
}

static inline void C3D_TexSetFilter(C3D_Tex* tex, GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter)
{
	tex->param &= ~(GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR));
	tex->param |= GPU_TEXTURE_MAG_FILTER(magFilter) | GPU_TEXTURE_MIN_FILTER(minFilter);
}

static inline void C3D_TexSetFilterMipmap(C3D_Tex* tex, GPU_TEXTURE_FILTER_PARAM filter)
{
	tex->param &= ~GPU_TEXTURE_MIP_FILTER(GPU_LINEAR);
	tex->param |= GPU_TEXTURE_MIP_FILTER(filter);
}

static inline void C3D_TexSetWrap(C3D_Tex* tex, GPU_TEXTURE_WRAP_PARAM wrapS, GPU_TEXTURE_WRAP_PARAM wrapT)
{
	tex->param &= ~(GPU_TEXTURE_WRAP_S(3) | GPU_TEXTURE_WRAP_T(3));
	tex->param |= GPU_TEXTURE_WRAP_S(wrapS) | GPU_TEXTURE_WRAP_T(wrapT);
}

static inline void C3D_TexSetLodBias(C3D_Tex* tex, float lodBias)
{
	int iLodBias = (int)(lodBias*0x100);
	if (iLodBias > 0xFFF)
		iLodBias = 0xFFF;
	else if (iLodBias < -0x1000)
		iLodBias = -0x1000;
	tex->lodBias = iLodBias & 0x1FFF;
}
