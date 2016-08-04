#include <c3d/maths.h>

void Mtx_Scale(C3D_Mtx* mtx, float x, float y, float z)
{
	int i;
	for (i = 0; i < 4; ++i)
	{
		mtx->r[i].x *= x;
		mtx->r[i].y *= y;
		mtx->r[i].z *= z;
	}
}
