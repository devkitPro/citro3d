#include <c3d/maths.h>

void Mtx_RotateZ(C3D_Mtx* mtx, float angle, bool bRightSide)
{
	float  a, b;
	float  cosAngle = cosf(angle);
	float  sinAngle = sinf(angle);
	size_t i;

	if (bRightSide)
	{
		for (i = 0; i < 4; ++i)
		{
			a = mtx->r[i].x*cosAngle + mtx->r[i].y*sinAngle;
			b = mtx->r[i].y*cosAngle - mtx->r[i].x*sinAngle;
			mtx->r[i].x = a;
			mtx->r[i].y = b;
		}
	}
	else
	{
		for (i = 0; i < 4; ++i)
		{
			a = mtx->r[0].c[i]*cosAngle - mtx->r[1].c[i]*sinAngle;
			b = mtx->r[1].c[i]*cosAngle + mtx->r[0].c[i]*sinAngle;
			mtx->r[0].c[i] = a;
			mtx->r[1].c[i] = b;
		}
	}
}
