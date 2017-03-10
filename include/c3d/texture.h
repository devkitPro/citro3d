#pragma once
#include "types.h"

typedef struct
{
	void* data[6];
} C3D_TexCube;

typedef struct
{
	union
	{
		void* data;
		C3D_TexCube* cube;
	};

	GPU_TEXCOLOR fmt : 4;
	size_t size : 28;

	union
	{
		u32 dim;
		struct
		{
			u16 height;
			u16 width;
		};
	};

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

bool C3D_TexInitWithParams(C3D_Tex* tex, C3D_TexCube* cube, C3D_TexInitParams p);
void C3D_TexLoadImage(C3D_Tex* tex, const void* data, GPU_TEXFACE face, int level);
void C3D_TexGenerateMipmap(C3D_Tex* tex, GPU_TEXFACE face);
void C3D_TexBind(int unitId, C3D_Tex* tex);
void C3D_TexFlush(C3D_Tex* tex);
void C3D_TexDelete(C3D_Tex* tex);

void C3D_TexShadowParams(bool perspective, float bias);

static inline int C3D_TexCalcMaxLevel(u32 width, u32 height)
{
	return (31-__builtin_clz(width < height ? width : height)) - 3; // avoid sizes smaller than 8
}

static inline u32 C3D_TexCalcLevelSize(u32 size, int level)
{
	return size >> (2*level);
}

static inline u32 C3D_TexCalcTotalSize(u32 size, int maxLevel)
{
	/*
	S  = s + sr + sr^2 + sr^3 + ... + sr^n
	Sr = sr + sr^2 + sr^3 + ... + sr^(n+1)
	S-Sr = s - sr^(n+1)
	S(1-r) = s(1 - r^(n+1))
	S = s (1 - r^(n+1)) / (1-r)

	r = 1/4
	1-r = 3/4

	S = 4s (1 - (1/4)^(n+1)) / 3
	S = 4s (1 - 1/4^(n+1)) / 3
	S = (4/3) (s - s/4^(n+1))
	S = (4/3) (s - s/(1<<(2n+2)))
	S = (4/3) (s - s>>(2n+2))
	*/
	return (size - C3D_TexCalcLevelSize(size,maxLevel+1)) * 4 / 3;
}

static inline bool C3D_TexInit(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_2D, false });
}

static inline bool C3D_TexInitMipmap(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, (u8)C3D_TexCalcMaxLevel(width, height), format, GPU_TEX_2D, false });
}

static inline bool C3D_TexInitCube(C3D_Tex* tex, C3D_TexCube* cube, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, cube,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_CUBE_MAP, false });
}

static inline bool C3D_TexInitVRAM(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_2D, true });
}

static inline bool C3D_TexInitShadow(C3D_Tex* tex, u16 width, u16 height)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, 0, GPU_RGBA8, GPU_TEX_SHADOW_2D, true });
}

static inline bool C3D_TexInitShadowCube(C3D_Tex* tex, C3D_TexCube* cube, u16 width, u16 height)
{
	return C3D_TexInitWithParams(tex, cube,
		(C3D_TexInitParams){ width, height, 0, GPU_RGBA8, GPU_TEX_SHADOW_CUBE, true });
}

static inline GPU_TEXTURE_MODE_PARAM C3D_TexGetType(C3D_Tex* tex)
{
	return (GPU_TEXTURE_MODE_PARAM)((tex->param>>28)&0x7);
}

static inline void* C3D_TexGetImagePtr(C3D_Tex* tex, void* data, int level, u32* size)
{
	if (size) *size = level >= 0 ? C3D_TexCalcLevelSize(tex->size, level) : C3D_TexCalcTotalSize(tex->size, tex->maxLevel);
	if (!level) return data;
	return (u8*)data + (level > 0 ? C3D_TexCalcTotalSize(tex->size, level-1) : 0);
}

static inline void* C3D_Tex2DGetImagePtr(C3D_Tex* tex, int level, u32* size)
{
	return C3D_TexGetImagePtr(tex, tex->data, level, size);
}

static inline void* C3D_TexCubeGetImagePtr(C3D_Tex* tex, GPU_TEXFACE face, int level, u32* size)
{
	return C3D_TexGetImagePtr(tex, tex->cube->data[face], level, size);
}

static inline void C3D_TexUpload(C3D_Tex* tex, const void* data)
{
	C3D_TexLoadImage(tex, data, GPU_TEXFACE_2D, 0);
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
