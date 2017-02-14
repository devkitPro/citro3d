#pragma once
#ifdef _3DS
#include <3ds.h>
#else
#include <stdbool.h>
#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
#endif

#ifndef CITRO3D_NO_DEPRECATION
#define C3D_DEPRECATED __attribute__ ((deprecated))
#else
#define C3D_DEPRECATED
#endif

typedef u32 C3D_IVec;

static inline C3D_IVec IVec_Pack(u8 x, u8 y, u8 z, u8 w)
{
	return (u32)x | ((u32)y << 8) | ((u32)z << 16) | ((u32)w << 24);
}

/**
 * @defgroup math_support Math Support Library
 * @brief Implementations of matrix, vector, and quaternion operations.
 * @{
 */

/**
 * @struct C3D_FVec
 * @brief Float vector
 *
 * Matches PICA layout
 */
typedef union
{
	/**
	 * @brief Vector access
	 */
	struct
	{
		float w; ///< W-component
		float z; ///< Z-component
		float y; ///< Y-component
		float x; ///< X-component
	};

	/**
	 * @brief Quaternion access
	 */
	struct
	{
		float r; ///< Real component
		float k; ///< K-component
		float j; ///< J-component
		float i; ///< I-component
	};

	/**
	 * @brief Raw access
	 */
	float c[4];
} C3D_FVec;

/**
 * @struct C3D_FQuat
 * @brief Float quaternion. See @ref C3D_FVec.
 */
typedef C3D_FVec C3D_FQuat;

/**
 * @struct C3D_Mtx
 * @brief Row-major 4x4 matrix
 */
typedef union
{
	C3D_FVec r[4]; ///< Rows are vectors
	float m[4*4]; ///< Raw access
} C3D_Mtx;
/** @} */
