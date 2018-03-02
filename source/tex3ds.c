/*------------------------------------------------------------------------------
 * Copyright (c) 2017
 *     Michael Theall (mtheall)
 *
 * This file is part of citro3d.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *----------------------------------------------------------------------------*/
/** @file tex3ds.c
 *  @brief Tex3DS routines
 */
#include <tex3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/** @brief Tex3DS texture
 */
struct Tex3DS_Texture_s
{
	u16 numSubTextures;               ///< Number of subtextures
	u16 width;                        ///< Texture width
	u16 height;                       ///< Texture height
	u8  format;                       ///< Texture format
	u8  mipmapLevels;                 ///< Number of mipmaps
	Tex3DS_SubTexture subTextures[];  ///< Subtextures
};

typedef struct __attribute__((packed))
{
	u16 numSubTextures;
	u8  width_log2  : 3;
	u8  height_log2 : 3;
	u8  type        : 1;
	u8  format;
	u8  mipmapLevels;
} Tex3DSi_Header;

typedef struct
{
	u16 width, height;
	u16 left, top, right, bottom;
} Tex3DSi_SubTexture;

static inline bool Tex3DSi_ReadData(decompressCallback callback, void** userdata, void* buffer, size_t size, size_t* insize)
{
	if (callback)
		return callback(*userdata, buffer, size) == size;
	if (size > *insize)
		return false;

	memcpy(buffer, *userdata, size);
	*userdata = (u8*)*userdata + size;
	*insize -= size;
	return true;
}

static Tex3DS_Texture
Tex3DSi_ImportCommon(C3D_Tex* tex, C3D_TexCube* texcube, bool vram, decompressCallback callback, void* userdata, size_t insize)
{
	// Read header
	Tex3DSi_Header hdr;
	if (!Tex3DSi_ReadData(callback, &userdata, &hdr, sizeof(hdr), &insize))
		return NULL;

	// Allocate space for header + subtextures
	Tex3DS_Texture texture = (Tex3DS_Texture)malloc(sizeof(struct Tex3DS_Texture_s) + hdr.numSubTextures*sizeof(Tex3DS_SubTexture));
	if (!texture)
		return NULL;

	// Fill texture metadata structure
	texture->numSubTextures = hdr.numSubTextures;
	texture->width          = 1 << (hdr.width_log2  + 3);
	texture->height         = 1 << (hdr.height_log2 + 3);
	texture->format         = hdr.format;
	texture->mipmapLevels   = hdr.mipmapLevels;

	// Read subtexture info
	for (size_t i = 0; i < hdr.numSubTextures; i ++)
	{
		Tex3DSi_SubTexture subtex;
		if (!Tex3DSi_ReadData(callback, &userdata, &subtex, sizeof(Tex3DSi_SubTexture), &insize))
		{
			free(texture);
			return NULL;
		}
		texture->subTextures[i].width  = subtex.width;
		texture->subTextures[i].height = subtex.height;
		texture->subTextures[i].left   = subtex.left   / 1024.0f;
		texture->subTextures[i].top    = subtex.top    / 1024.0f;
		texture->subTextures[i].right  = subtex.right  / 1024.0f;
		texture->subTextures[i].bottom = subtex.bottom / 1024.0f;
	}

	// Allocate texture memory
	C3D_TexInitParams params;
	params.width    = texture->width;
	params.height   = texture->height;
	params.maxLevel = texture->mipmapLevels;
	params.format   = texture->format;
	params.type     = (GPU_TEXTURE_MODE_PARAM)hdr.type;
	params.onVram   = vram;
	if (!C3D_TexInitWithParams(tex, texcube, params))
	{
		free(texture);
		return NULL;
	}

	// Get texture size, including mipmaps
	size_t base_texsize = C3D_TexCalcTotalSize(tex->size, texture->mipmapLevels);
	size_t texsize = base_texsize;

	// If this is a cubemap/skybox, there are 6 textures
	if (params.type == GPU_TEX_CUBE_MAP)
		texsize *= 6;

	if (vram)
	{
		// Allocate staging buffer in linear memory
		void* texdata = linearAlloc(texsize);
		if (!texdata)
		{
			C3D_TexDelete(tex);
			free(texture);
			return NULL;
		}

		// Decompress into staging buffer for VRAM upload
		if (!decompress(texdata, texsize, callback, userdata, insize))
		{
			linearFree(texdata);
			C3D_TexDelete(tex);
			free(texture);
			return NULL;
		}

		// Flush buffer to prepare DMA to VRAM
		GSPGPU_FlushDataCache(texdata, texsize);

		size_t texcount = 1;
		if (params.type == GPU_TEX_CUBE_MAP)
			texcount = 6;

		// Upload texture(s) to VRAM
		for (size_t i = 0; i < texcount; ++i)
			C3D_TexLoadImage(tex, (u8*)texdata + i * base_texsize, i, -1);

		linearFree(texdata);
	} else if (params.type == GPU_TEX_CUBE_MAP)
	{
		decompressIOVec iov[6];

		// Setup IO vectors
		for (size_t i = 0; i < 6; ++i)
		{
			u32 size;
			iov[i].data = C3D_TexCubeGetImagePtr(tex, i, -1, &size);
			iov[i].size = size;
		}

		// Decompress into texture memory
		if (!decompressV(iov, 6, callback, userdata, insize))
		{
			C3D_TexDelete(tex);
			free(texture);
			return NULL;
		}
	} else
	{
		u32 size;
		void* data = C3D_Tex2DGetImagePtr(tex, -1, &size);

		// Decompress into texture memory
		if (!decompress(data, size, callback, userdata, insize))
		{
			C3D_TexDelete(tex);
			free(texture);
			return NULL;
		}
	}

	return texture;
}

Tex3DS_Texture
Tex3DS_TextureImport(const void* input, size_t insize, C3D_Tex* tex, C3D_TexCube* texcube, bool vram)
{
	return Tex3DSi_ImportCommon(tex, texcube, vram, NULL, (void*)input, insize);
}

Tex3DS_Texture
Tex3DS_TextureImportCallback(C3D_Tex* tex, C3D_TexCube* texcube, bool vram, decompressCallback callback, void* userdata)
{
	return Tex3DSi_ImportCommon(tex, texcube, vram, callback, userdata, 0);
}

Tex3DS_Texture
Tex3DS_TextureImportFD(int fd, C3D_Tex* tex, C3D_TexCube* texcube, bool vram)
{
	return Tex3DSi_ImportCommon(tex, texcube, vram, decompressCallback_FD, &fd, 0);
}

Tex3DS_Texture
Tex3DS_TextureImportStdio(FILE* fp, C3D_Tex* tex, C3D_TexCube* texcube, bool vram)
{
	return Tex3DSi_ImportCommon(tex, texcube, vram, decompressCallback_Stdio, fp, 0);
}

size_t
Tex3DS_GetNumSubTextures(const Tex3DS_Texture texture)
{
	return texture->numSubTextures;
}

const Tex3DS_SubTexture*
Tex3DS_GetSubTexture(const Tex3DS_Texture texture, size_t index)
{
	if (index < texture->numSubTextures)
		return &texture->subTextures[index];
	return NULL;
}

void Tex3DS_TextureFree(Tex3DS_Texture texture)
{
	free(texture);
}
