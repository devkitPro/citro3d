#pragma once
#include <3ds.h>

typedef u32 C3D_IVec;

typedef union
{
	struct { float w, z, y, x; };
	float c[4];
} C3D_FVec;

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
