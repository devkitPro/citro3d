#include <c3d/maths.h>

void Mtx_RotateX(C3D_Mtx* mtx, float angle, bool bRightSide)
{
	float  a, b;
	float  cosAngle = cosf(angle);
	float  sinAngle = sinf(angle);
	size_t i;

	if (bRightSide)
	{
		for (i = 0; i < 4; ++i)
		{
			a = mtx->r[i].y*cosAngle + mtx->r[i].z*sinAngle;
			b = mtx->r[i].z*cosAngle - mtx->r[i].y*sinAngle;
			mtx->r[i].y = a;
			mtx->r[i].z = b;
		}
	}
	else
	{
		for (i = 0; i < 4; ++i)
		{
			a = mtx->r[1].c[i]*cosAngle - mtx->r[2].c[i]*sinAngle;
			b = mtx->r[2].c[i]*cosAngle + mtx->r[1].c[i]*sinAngle;
			mtx->r[1].c[i] = a;
			mtx->r[2].c[i] = b;
		}
	}
}
