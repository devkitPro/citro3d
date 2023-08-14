/**
 * @file buffers.h
 * @brief Configure vertex array buffers
 */
#pragma once
#include "types.h"

/// Vertex buffer config
typedef struct
{
	u32 offset;
	u32 flags[2];
} C3D_BufCfg;

/// Vertex buffer info
typedef struct
{
	u32 base_paddr;
	int bufCount;
	C3D_BufCfg buffers[12];
} C3D_BufInfo;

/**
 * @brief Resets and initializes \ref C3D_BufInfo structure to default values.
 * @param[out] info Pointer to vertex buffer info structure.
 */
void BufInfo_Init(C3D_BufInfo* info);

/**
 * @brief Adds a buffer to the vertex buffer info struct.
 * @note The attribute indices specified in \p permutation should be the same as the ones returned by \ref AttrInfo_AddLoader()
 * @param[out] info         Pointer to a \ref C3D_BufInfo struct.
 * @param[in]  data         Pointer to buffer.
 * @param[in]  stride       Distance in bytes between vertex entries in the buffer (usually set to the size of the vertex structure).
 * @param[in]  attribCount  Number of attributes to load from this buffer.
 * @param[in]  permutation  Specifies the order of attributes in the buffer. Each attribute index is a nibble (4 bits), and they are ordered from least significant to most significant.
 *       Padding can be specified with 0xC, 0xD, 0xE and 0xF. (See https://3dbrew.org/wiki/GPU/Internal_Registers#GPUREG_ATTRIBBUFFERi_CONFIG1 for more info)
 * @remark Using this, one can rearrange the positions of attributes in the struct.
 *       For example, with three attributes in this buffer, setting this to 0x210 would configure the permutation to be the first attribute index,
 *       then the second, then the third. 0x120 would configure the permutation to be the first attribute index, then the third, then the second.
 * @return Buffer index if successful, negative value on failure.
 */
int  BufInfo_Add(C3D_BufInfo* info, const void* data, ptrdiff_t stride, int attribCount, u64 permutation);

/**
 * @brief Gets pointer to the global \ref C3D_BufInfo structure.
 * @return Pointer to \ref C3D_BufInfo. This should not be freed.
 */
C3D_BufInfo* C3D_GetBufInfo(void);

/**
 * @brief Sets global \ref C3D_BufInfo structure.
 * Copies values from the specified \ref C3D_BufInfo structure to the
 *  global \ref C3D_BufInfo structure.
 * @param[in] info Pointer to user \ref C3D_BufInfo.
 */
void C3D_SetBufInfo(C3D_BufInfo* info);
