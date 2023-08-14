/**
 * @file texture.h
 * @brief Create and manipulate textures
 */
#pragma once
#include "types.h"

/// Cubemap texture data
typedef struct
{
	void* data[6];
} C3D_TexCube;

/// Texture data
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

/// Parameters for \ref C3D_TexInitWithParams()
typedef struct ALIGN(8)
{
	u16 width;                       ///< Width of texture in pixels. (must be a power of 2)
	u16 height;                      ///< Height of texture in pixels. (must be a power of 2)
	u8 maxLevel                 : 4; ///< Maximum mipmap level.
	GPU_TEXCOLOR format         : 4; ///< GPU texture format.
	GPU_TEXTURE_MODE_PARAM type : 3; ///< Texture type
	bool onVram                 : 1; ///< Specifies whether to allocate texture data in Vram or linearMemory
} C3D_TexInitParams;

/**
 * @brief Initializes texture with specified parameters
 * @param[out]    tex  Pointer to uninitialized \ref C3D_Tex.
 * @param[out]    cube Pointer to \ref C3D_TexCube. (Only used if texture type is cubemap)
 * @param[in]     p    Parameters. See \ref C3D_TexInitParams.
 * @return True if texture was initialized successfully, otherwise false.
 */
bool C3D_TexInitWithParams(C3D_Tex* tex, C3D_TexCube* cube, C3D_TexInitParams p);

/**
 * @brief Copies raw texture data into \ref C3D_Tex
 * @param[out] tex   Pointer to \ref C3D_Tex.
 * @param[in]  data  Pointer to raw texture data.
 * @param[in]  face  Specifies texture face.
 * @param[in]  level Specifies mipmap level.
 */
void C3D_TexLoadImage(C3D_Tex* tex, const void* data, GPU_TEXFACE face, int level);

/**
 * @brief Generates mipmaps for \ref C3D_Tex using previously specified max level
 * The max level should have been specified using \ref C3D_TexInitMipmap() or \ref C3D_TexInitWithParams()
 * @note Does not support generating mipmaps for VRAM textures.
 * @param[in,out] tex   Pointer to \ref C3D_Tex.
 * @param[in]     face  Specifies texture face.
 */
void C3D_TexGenerateMipmap(C3D_Tex* tex, GPU_TEXFACE face);

/**
 * @brief Binds C3D_Tex to texture unit
 * @note The 3DS has 3 normal texture units (IDs 0 through 2).
 * @param[in] unitId Specifies texture unit.
 * @param[in] tex    Pointer to \ref C3D_Tex.
 */
void C3D_TexBind(int unitId, C3D_Tex* tex);

/**
 * @brief Flushes texture data from cache into memory
 * @param[in] tex Pointer to \ref C3D_Tex.
 * @sa GSPGPU_FlushDataCache()
 */
void C3D_TexFlush(C3D_Tex* tex);

/**
 * @brief Deletes texture data
 * @param[in] tex Pointer to \ref C3D_Tex.
 */
void C3D_TexDelete(C3D_Tex* tex);

/**
 * @brief Configues texunit0 shadow texture properties
 * @param[in] perspective TODO
 * @param[in] bias        TODO
 */
void C3D_TexShadowParams(bool perspective, float bias);

/**
 * @brief Calculates maximum mipmap level for given texture size
 * @param[in] width  Width of texture.
 * @param[in] height Height of texture.
 * @returns Calculated maximum mipmap level.
 */
static inline int C3D_TexCalcMaxLevel(u32 width, u32 height)
{
	return (31-__builtin_clz(width < height ? width : height)) - 3; // avoid sizes smaller than 8
}

/**
 * @brief Calculates size of mipmap level
 * @param[in] size  Size of original texture.
 * @param[in] level Mipmap level.
 * @returns Calculated level size.
 */
static inline u32 C3D_TexCalcLevelSize(u32 size, int level)
{
	return size >> (2*level);
}

/**
 * @brief Calculates total size of mipmap texture data
 * @param[in] size     Size of original texture.
 * @param[in] maxLevel Maximum mipmap level.
 * @returns Calculated total size.
 */
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

/**
 * @brief Initializes standard 2D texture
 * @param[out] tex    Pointer to uninitialized \ref C3D_Tex.
 * @param[in]  width  Specifies width of texture. (must be a power of 2)
 * @param[in]  height Specifies height of texture. (must be a power of 2)
 * @param[in]  format Specifies texture format.
 * @return True if texture was initialized successfully, otherwise false.
 */
static inline bool C3D_TexInit(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_2D, false });
}

/**
 * @brief Initializes standard 2D texture with mipmap
 * Maximum miplevel is calculated using \ref C3D_TexCalcMaxLevel()
 * @param[out] tex    Pointer to uninitialized \ref C3D_Tex.
 * @param[in]  width  Specifies width of texture. (must be a power of 2)
 * @param[in]  height Specifies height of texture. (must be a power of 2)
 * @param[in]  format Specifies texture format.
 * @return True if texture was initialized successfully, otherwise false.
 */
static inline bool C3D_TexInitMipmap(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, (u8)C3D_TexCalcMaxLevel(width, height), format, GPU_TEX_2D, false });
}

/**
 * @brief Initializes cubemap texture
 * @param[out] tex    Pointer to uninitialized \ref C3D_Tex.
 * @param[out] cube   Pointer to \ref C3D_TexCube.
 * @param[in]  width  Specifies width of texture. (must be a power of 2)
 * @param[in]  height Specifies height of texture. (must be a power of 2)
 * @param[in]  format Specifies texture format.
 * @return True if texture was initialized successfully, otherwise false.
 */
static inline bool C3D_TexInitCube(C3D_Tex* tex, C3D_TexCube* cube, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, cube,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_CUBE_MAP, false });
}

/**
 * @brief Initializes 2D texture in VRAM
 * @param[out] tex    Pointer to uninitialized \ref C3D_Tex.
 * @param[in]  width  Specifies width of texture. (must be a power of 2)
 * @param[in]  height Specifies height of texture. (must be a power of 2)
 * @param[in]  format Specifies texture format.
 * @return True if texture was initialized successfully, otherwise false.
 */
static inline bool C3D_TexInitVRAM(C3D_Tex* tex, u16 width, u16 height, GPU_TEXCOLOR format)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, 0, format, GPU_TEX_2D, true });
}

/**
 * @brief Initializes 2D shadowmap texture
 * @param[out] tex    Pointer to uninitialized \ref C3D_Tex.
 * @param[in]  width  Specifies width of texture. (must be a power of 2)
 * @param[in]  height Specifies height of texture. (must be a power of 2)
 * @return True if texture was initialized successfully, otherwise false.
 */
static inline bool C3D_TexInitShadow(C3D_Tex* tex, u16 width, u16 height)
{
	return C3D_TexInitWithParams(tex, NULL,
		(C3D_TexInitParams){ width, height, 0, GPU_RGBA8, GPU_TEX_SHADOW_2D, true });
}

/**
 * @brief Initializes shadowmap cubemap texture
 * @param[out] tex    Pointer to uninitialized \ref C3D_Tex.
 * @param[out] cube   Pointer to \ref C3D_TexCube.
 * @param[in]  width  Specifies width of texture. (must be a power of 2)
 * @param[in]  height Specifies height of texture. (must be a power of 2)
 * @return True if texture was initialized successfully, otherwise false.
 */
static inline bool C3D_TexInitShadowCube(C3D_Tex* tex, C3D_TexCube* cube, u16 width, u16 height)
{
	return C3D_TexInitWithParams(tex, cube,
		(C3D_TexInitParams){ width, height, 0, GPU_RGBA8, GPU_TEX_SHADOW_CUBE, true });
}

/**
 * @brief Gets type of texture
 * @param[in] tex Pointer to \ref C3D_Tex.
 */
static inline GPU_TEXTURE_MODE_PARAM C3D_TexGetType(C3D_Tex* tex)
{
	return (GPU_TEXTURE_MODE_PARAM)((tex->param>>28)&0x7);
}

/**
 * @brief Gets pointer to texture image
 * @param[in]  tex   Pointer to \ref C3D_Tex.
 * @param[in]  data  Pointer texture face.
 * @param[in]  level Specifies mipmap level.
 * @param[out] size  Can be used to get the size of the image data.
 * @returns Pointer to raw image data.
 */
static inline void* C3D_TexGetImagePtr(C3D_Tex* tex, void* data, int level, u32* size)
{
	if (size) *size = level >= 0 ? C3D_TexCalcLevelSize(tex->size, level) : C3D_TexCalcTotalSize(tex->size, tex->maxLevel);
	if (!level) return data;
	return (u8*)data + (level > 0 ? C3D_TexCalcTotalSize(tex->size, level-1) : 0);
}

/**
 * @brief Gets pointer to 2D texture image
 * @param[in]  tex   Pointer to \ref C3D_Tex.
 * @param[in]  level Specifies mipmap level.
 * @param[out] size  Can be used to get the size of the image data.
 * @returns Pointer to raw image data.
 */
static inline void* C3D_Tex2DGetImagePtr(C3D_Tex* tex, int level, u32* size)
{
	return C3D_TexGetImagePtr(tex, tex->data, level, size);
}

/**
 * @brief Gets pointer to cubemap texture image
 * @param[in]  tex   Pointer to \ref C3D_Tex.
 * @param[in]  face  Specifies the cubemap texture face.
 * @param[in]  level Specifies mipmap level.
 * @param[out] size  Can be used to get the size of the image data.
 * @returns Pointer to raw image data.
 */
static inline void* C3D_TexCubeGetImagePtr(C3D_Tex* tex, GPU_TEXFACE face, int level, u32* size)
{
	return C3D_TexGetImagePtr(tex, tex->cube->data[face], level, size);
}

/**
 * @brief Copies raw texture data into standard 2D texture
 * @param[out] tex  Pointer to \ref C3D_Tex.
 * @param[in]  data Pointer to raw texture data.
 */
static inline void C3D_TexUpload(C3D_Tex* tex, const void* data)
{
	C3D_TexLoadImage(tex, data, GPU_TEXFACE_2D, 0);
}

/**
 * @brief Configures texture magnification and minification filters
 * @param[out] tex       Pointer to \ref C3D_Tex.
 * @param[in]  magFilter Specifies the filtering to use when magnifying the the texture.
 * @param[in]  minFilter Specifies the filtering to use when minifying the the texture.
 */
static inline void C3D_TexSetFilter(C3D_Tex* tex, GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter)
{
	tex->param &= ~(GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR));
	tex->param |= GPU_TEXTURE_MAG_FILTER(magFilter) | GPU_TEXTURE_MIN_FILTER(minFilter);
}

/**
 * @brief Configures texture mipmap minification filters
 * @param[out] tex    Pointer to \ref C3D_Tex.
 * @param[in]  filter Specifies the filtering to use when minifying the the mipmap.
 */
static inline void C3D_TexSetFilterMipmap(C3D_Tex* tex, GPU_TEXTURE_FILTER_PARAM filter)
{
	tex->param &= ~GPU_TEXTURE_MIP_FILTER(GPU_LINEAR);
	tex->param |= GPU_TEXTURE_MIP_FILTER(filter);
}

/**
 * @brief Configures texture wrapping options
 * @param[out] tex   Pointer to \ref C3D_Tex.
 * @param[in]  wrapS Specifies the texture wrapping mode for texture coordinate S (aka U).
 * @param[in]  wrapT Specifies the texture wrapping mode for texture coordinate T (aka V).
 */
static inline void C3D_TexSetWrap(C3D_Tex* tex, GPU_TEXTURE_WRAP_PARAM wrapS, GPU_TEXTURE_WRAP_PARAM wrapT)
{
	tex->param &= ~(GPU_TEXTURE_WRAP_S(3) | GPU_TEXTURE_WRAP_T(3));
	tex->param |= GPU_TEXTURE_WRAP_S(wrapS) | GPU_TEXTURE_WRAP_T(wrapT);
}

/**
 * @brief Configures texture level of detail bias used to select the correct mipmap during sampling
 * @param[out] tex     Pointer to \ref C3D_Tex.
 * @param[in]  lodBias Specifies the texture level of detail bias.
 */
static inline void C3D_TexSetLodBias(C3D_Tex* tex, float lodBias)
{
	int iLodBias = (int)(lodBias*0x100);
	if (iLodBias > 0xFFF)
		iLodBias = 0xFFF;
	else if (iLodBias < -0x1000)
		iLodBias = -0x1000;
	tex->lodBias = iLodBias & 0x1FFF;
}
