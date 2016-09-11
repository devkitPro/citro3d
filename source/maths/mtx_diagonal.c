#include <c3d/maths.h>

void Mtx_Diagonal(C3D_Mtx* out, float x, float y, float z, float w)
{
	Mtx_Zeros(out);
	out->r[0].x = x;
	out->r[1].y = y;
	out->r[2].z = z;
	out->r[3].w = w;
}
