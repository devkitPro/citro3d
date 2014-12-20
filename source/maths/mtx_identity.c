#include <c3d/maths.h>

void Mtx_Identity(C3D_Mtx* out)
{
	Mtx_Zeros(out);
	out->r[0].x = out->r[1].y = out->r[2].z = out->r[3].w = 1.0f;
}
