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
/** @file tex3ds.h
 *  @brief tex3ds support
 */
#pragma once
#ifdef CITRO3D_BUILD
#include "c3d/texture.h"
#else
#include <citro3d.h>
#endif

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Subtexture
 *  @note If top > bottom, the subtexture is rotated 1/4 revolution counter-clockwise
 */
typedef struct Tex3DS_SubTexture
{
	u16   width;  ///< Sub-texture width (pixels)
	u16   height; ///< Sub-texture height (pixels)
	float left;   ///< Left u-coordinate
	float top;    ///< Top v-coordinate
	float right;  ///< Right u-coordinate
	float bottom; ///< Bottom v-coordinate
} Tex3DS_SubTexture;

/** @brief Texture */
typedef struct Tex3DS_Texture_s* Tex3DS_Texture;

/** @brief Import Tex3DS texture
 *  @param[in]  input   Input data
 *  @param[in]  insize  Size of the input data
 *  @param[out] tex     citro3d texture
 *  @param[out] texcube citro3d texcube
 *  @param[in]  vram    Whether to store textures in VRAM
 *  @returns Tex3DS texture
 */
Tex3DS_Texture Tex3DS_TextureImport(const void* input, size_t insize, C3D_Tex* tex, C3D_TexCube* texcube, bool vram);

/** @brief Import Tex3DS texture
 *
 *  @description
 *  For example, use this if you want to import from a large file without
 *  pulling the entire file into memory.
 *
 *  @param[out] tex      citro3d texture
 *  @param[out] texcube  citro3d texcube
 *  @param[in]  vram     Whether to store textures in VRAM
 *  @param[in]  callback Data callback
 *  @param[in]  userdata User data passed to callback
 *  @returns Tex3DS texture
 */
Tex3DS_Texture Tex3DS_TextureImportCallback(C3D_Tex* tex, C3D_TexCube* texcube, bool vram, decompressCallback callback, void* userdata);

/** @brief Import Tex3DS texture
 *
 *  Starts reading at the current file descriptor's offset. The file
 *  descriptor's position is left at the end of the decoded data. On error, the
 *  file descriptor's position is indeterminate.
 *
 *  @param[in]  fd       Open file descriptor
 *  @param[out] tex      citro3d texture
 *  @param[out] texcube  citro3d texcube
 *  @param[in]  vram     Whether to store textures in VRAM
 *  @returns Tex3DS texture
 */
Tex3DS_Texture Tex3DS_TextureImportFD(int fd, C3D_Tex* tex, C3D_TexCube* texcube, bool vram);

/** @brief Import Tex3DS texture
 *
 *  Starts reading at the current file stream's offset. The file stream's
 *  position is left at the end of the decoded data. On error, the file
 *  stream's position is indeterminate.
 *
 *  @param[in]  fp       Open file stream
 *  @param[out] tex      citro3d texture
 *  @param[out] texcube  citro3d texcube
 *  @param[in]  vram     Whether to store textures in VRAM
 *  @returns Tex3DS texture
 */
Tex3DS_Texture Tex3DS_TextureImportStdio(FILE* fp, C3D_Tex* tex, C3D_TexCube* texcube, bool vram);

/** @brief Get number of subtextures
 *  @param[in] texture Tex3DS texture
 *  @returns Number of subtextures
 */
size_t Tex3DS_GetNumSubTextures(const Tex3DS_Texture texture);

/** @brief Get subtexture
 *  @param[in] texture Tex3DS texture
 *  @param[in] index   Subtexture index
 *  @returns Subtexture info
 */
const Tex3DS_SubTexture* Tex3DS_GetSubTexture(const Tex3DS_Texture texture, size_t index);

/** @brief Check if subtexture is rotated
 *  @param[in] subtex Subtexture to check
 *  @returns whether subtexture is rotated
 */
static inline bool
Tex3DS_SubTextureRotated(const Tex3DS_SubTexture* subtex)
{
	return subtex->top < subtex->bottom;
}

/** @brief Get bottom-left texcoords
 *  @param[in]  subtex Subtexture
 *  @param[out] u      u-coordinate
 *  @param[out] v      v-coordinate
 */
static inline void
Tex3DS_SubTextureBottomLeft(const Tex3DS_SubTexture* subtex, float* u, float* v)
{
	if (!Tex3DS_SubTextureRotated(subtex))
	{
		*u = subtex->left;
		*v = subtex->bottom;
	} else
	{
		*u = subtex->bottom;
		*v = subtex->left;
	}
}

/** @brief Get bottom-right texcoords
 *  @param[in]  subtex Subtexture
 *  @param[out] u      u-coordinate
 *  @param[out] v      v-coordinate
 */
static inline void
Tex3DS_SubTextureBottomRight(const Tex3DS_SubTexture* subtex, float* u, float* v)
{
	if (!Tex3DS_SubTextureRotated(subtex))
	{
		*u = subtex->right;
		*v = subtex->bottom;
	} else
	{
		*u = subtex->bottom;
		*v = subtex->right;
	}
}

/** @brief Get top-left texcoords
 *  @param[in]  subtex Subtexture
 *  @param[out] u      u-coordinate
 *  @param[out] v      v-coordinate
 */
static inline void
Tex3DS_SubTextureTopLeft(const Tex3DS_SubTexture* subtex, float* u, float* v)
{
	if (!Tex3DS_SubTextureRotated(subtex))
	{
		*u = subtex->left;
		*v = subtex->top;
	} else
	{
		*u = subtex->top;
		*v = subtex->left;
	}
}

/** @brief Get top-right texcoords
 *  @param[in]  subtex Subtexture
 *  @param[out] u      u-coordinate
 *  @param[out] v      v-coordinate
 */
static inline void
Tex3DS_SubTextureTopRight(const Tex3DS_SubTexture* subtex, float* u, float* v)
{
	if (!Tex3DS_SubTextureRotated(subtex))
	{
		*u = subtex->right;
		*v = subtex->top;
	} else
	{
		*u = subtex->top;
		*v = subtex->right;
	}
}

/** @brief Free Tex3DS texture
 *  @param[in] texture Tex3DS texture to free
 */
void Tex3DS_TextureFree(Tex3DS_Texture texture);

#ifdef __cplusplus
}
#endif
