#pragma once
#ifdef _3DS
#include <3ds.h>
#else
#include <stdbool.h>
#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
#endif

typedef u32 C3D_IVec;

typedef union
{
	struct { float w, z, y, x; };
	struct { float r, k, j, i; };
	float c[4];
} C3D_FVec;

typedef C3D_FVec C3D_FQuat;

// Row-major 4x4 matrix
typedef union
{
	C3D_FVec r[4]; // Rows are vectors
	float m[4*4];
} C3D_Mtx;

static inline C3D_IVec IVec_Pack(u8 x, u8 y, u8 z, u8 w)
{
	return (u32)x | ((u32)y << 8) | ((u32)z << 16) | ((u32)w << 24);
}
