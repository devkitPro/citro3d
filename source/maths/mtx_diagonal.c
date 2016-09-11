#include <c3d/maths.h>

void Mtx_Diagonal(C3D_Mtx* out, float x, float y, float z, float w)
{
	Mtx_Identity(out);
	FVec4_Scale(out->r[0], x);
	FVec4_Scale(out->r[1], y);
	FVec4_Scale(out->r[2], z);
	FVec4_Scale(out->r[3], w);
}
