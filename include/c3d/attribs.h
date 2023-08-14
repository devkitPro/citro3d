/**
 * @file attribs.h
 * @brief Configure vertex attributes
 */
#pragma once
#include "types.h"

/// Vertex attribute info
typedef struct
{
	u32 flags[2];
	u64 permutation;
	int attrCount;
} C3D_AttrInfo;

/**
 * @brief Resets and initializes \ref C3D_AttrInfo structure to default values.
 * @param[out] info Pointer to attribute info structure.
 */
void AttrInfo_Init(C3D_AttrInfo* info);

/**
 * @brief Defines an array of vertex attribute data.
 * @note The attribute index returned should be the same as the order used
 *       when specifying \ref AttrInfo_AddLoader() and \ref AttrInfo_AddFixed().
 * @param[out] info   Attribute info structure.
 * @param[in]  regId  Specifies the attribute register in the vertex shader that will be modified.
 * @param[in]  format Specifies the data type of the array.
 * @param[in]  count  Specifies the length of the array.
 * @return Attribute index if successful, negative value on failure.
 */
int  AttrInfo_AddLoader(C3D_AttrInfo* info, int regId, GPU_FORMATS format, int count);

/**
 * @brief Defines a fixed vertex attribute.
 * @note The attribute index returned should be the same as the order used
 *       when specifying \ref AttrInfo_AddLoader() and \ref AttrInfo_AddFixed().
 * @param[out] info  Attribute info structure.
 * @param[in]  regId Specifies the attribute register in the vertex shader that will be modified.
 * @return Attribute index if successful, negative value on failure.
 */
int  AttrInfo_AddFixed(C3D_AttrInfo* info, int regId);

/**
 * @brief Gets pointer to the global \ref C3D_AttrInfo structure.
 * @return Pointer to global \ref C3D_AttrInfo. This should not be freed.
 */
C3D_AttrInfo* C3D_GetAttrInfo(void);

/**
 * @brief Sets global \ref C3D_AttrInfo structure.
 * Copies values from the specified \ref C3D_AttrInfo structure to the
 *  global \ref C3D_AttrInfo structure.
 * @param[in] info Pointer to user \ref C3D_AttrInfo.
 */
void C3D_SetAttrInfo(C3D_AttrInfo* info);
