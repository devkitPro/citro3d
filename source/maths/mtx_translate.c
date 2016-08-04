#include <c3d/maths.h>

void Mtx_Translate(C3D_Mtx* mtx, float x, float y, float z, bool bRightSide)
{

	C3D_FVec v = FVec4_New(x, y, z, 1.0f);
	int i, j;

	if (bRightSide)
	{
		for (i = 0; i < 4; ++i)
			mtx->r[i].w = FVec4_Dot(mtx->r[i], v);
	}
	else
	{
		for (j = 0; j < 3; ++j)
			for (i = 0; i < 4; ++i)
				mtx->r[j].c[i] += mtx->r[3].c[i] * v.c[3-j];
	}

}
