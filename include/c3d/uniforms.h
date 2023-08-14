/**
 * @file uniforms.h
 * @brief Write to shader uniforms
 */
#pragma once
#include "maths.h"

#define C3D_FVUNIF_COUNT 96
#define C3D_IVUNIF_COUNT 4

extern C3D_FVec C3D_FVUnif[2][C3D_FVUNIF_COUNT];
extern C3D_IVec C3D_IVUnif[2][C3D_IVUNIF_COUNT];
extern u16      C3D_BoolUnifs[2];

extern bool C3D_FVUnifDirty[2][C3D_FVUNIF_COUNT];
extern bool C3D_IVUnifDirty[2][C3D_IVUNIF_COUNT];
extern bool C3D_BoolUnifsDirty[2];

/**
 * @brief Provides a writable pointer for a floating-point uniform.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] size Number of registers allocated for this uniform.
 * @return Writable pointer. This should not be freed.
 */
static inline C3D_FVec* C3D_FVUnifWritePtr(GPU_SHADER_TYPE type, int id, int size)
{
	int i;
	for (i = 0; i < size; i ++)
		C3D_FVUnifDirty[type][id+i] = true;
	return &C3D_FVUnif[type][id];
}

/**
 * @brief Provides a writable pointer for an integer uniform.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @return Writable pointer. This should not be freed.
 */
static inline C3D_IVec* C3D_IVUnifWritePtr(GPU_SHADER_TYPE type, int id)
{
	id -= 0x60;
	C3D_IVUnifDirty[type][id] = true;
	return &C3D_IVUnif[type][id];
}

/**
 * @brief Writes an Nx4 matrix to the uniform registers.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] mtx  Matrix to be written.
 * @param[in] num  Row count of the matrix.
 * @remark Usually, one should use the helper functions for 4x4, 3x4, and 2x4 matrices listed below.
 */
static inline void C3D_FVUnifMtxNx4(GPU_SHADER_TYPE type, int id, const C3D_Mtx* mtx, int num)
{
	int i;
	C3D_FVec* ptr = C3D_FVUnifWritePtr(type, id, num);
	for (i = 0; i < num; i ++)
		ptr[i] = mtx->r[i]; // Struct copy.
}

/**
 * @brief Writes a 4x4 matrix to the uniform registers.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] mtx  Matrix to be written.
 */
static inline void C3D_FVUnifMtx4x4(GPU_SHADER_TYPE type, int id, const C3D_Mtx* mtx)
{
	C3D_FVUnifMtxNx4(type, id, mtx, 4);
}

/**
 * @brief Writes a 3x4 matrix to the uniform registers.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] mtx  Matrix to be written.
 */
static inline void C3D_FVUnifMtx3x4(GPU_SHADER_TYPE type, int id, const C3D_Mtx* mtx)
{
	C3D_FVUnifMtxNx4(type, id, mtx, 3);
}

/**
 * @brief Writes a 2x4 matrix to the uniform registers.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] mtx  Matrix to be written.
 */
static inline void C3D_FVUnifMtx2x4(GPU_SHADER_TYPE type, int id, const C3D_Mtx* mtx)
{
	C3D_FVUnifMtxNx4(type, id, mtx, 2);
}

/**
 * @brief Writes a 4-component floating-point vector to the uniform registers.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] x    X component of the vector.
 * @param[in] y    Y component of the vector.
 * @param[in] z    Z component of the vector.
 * @param[in] w    W component of the vector.
 */
static inline void C3D_FVUnifSet(GPU_SHADER_TYPE type, int id, float x, float y, float z, float w)
{
	C3D_FVec* ptr = C3D_FVUnifWritePtr(type, id, 1);
	ptr->x = x;
	ptr->y = y;
	ptr->z = z;
	ptr->w = w;
}

/**
 * @brief Writes a 4-component integer vector to the uniform registers.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id   ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] x    X component of the vector.
 * @param[in] y    Y component of the vector.
 * @param[in] z    Z component of the vector.
 * @param[in] w    W component of the vector.
 */
static inline void C3D_IVUnifSet(GPU_SHADER_TYPE type, int id, int x, int y, int z, int w)
{
	C3D_IVec* ptr = C3D_IVUnifWritePtr(type, id);
	*ptr = IVec_Pack(x, y, z, w);
}

/**
 * @brief Writes a boolean value to the uniform registers.
 * @param[in] type  \ref GPU_SHADER_TYPE of the uniform to be set.
 * @param[in] id    ID of the uniform (retrievable using libctru's \ref shaderInstanceGetUniformLocation())
 * @param[in] value Boolean value to write.
 */
static inline void C3D_BoolUnifSet(GPU_SHADER_TYPE type, int id, bool value)
{
	id -= 0x68;
	C3D_BoolUnifsDirty[type] = true;
	if (value)
		C3D_BoolUnifs[type] |= BIT(id);
	else
		C3D_BoolUnifs[type] &= ~BIT(id);
}

/**
 * @brief Flushes newly-updated uniforms to the uniform registers.
 * @param[in] type \ref GPU_SHADER_TYPE of the uniforms to be flushed.
 * @remark This is called internally, and generally does not need to be handled by the user.
 */
void C3D_UpdateUniforms(GPU_SHADER_TYPE type);
