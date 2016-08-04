#include <c3d/maths.h>

void Mtx_Rotate(C3D_Mtx* mtx, C3D_FVec axis, float angle, bool bRightSide)
{
	size_t  i;
	C3D_Mtx om;

	float s = sinf(angle);
	float c = cosf(angle);
	float t = 1.0f - c;

	axis = FVec3_Normalize(axis);

	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	float w;

	om.r[0].x = t*x*x + c;
	om.r[1].x = t*x*y + s*z;
	om.r[2].x = t*x*z - s*y;
	//om.r[3].x = 0.0f; //optimized out

	om.r[0].y = t*y*x - s*z;
	om.r[1].y = t*y*y + c;
	om.r[2].y = t*y*z + s*x;
	//om.r[3].y = 0.0f; //optimized out

	om.r[0].z = t*z*x + s*y;
	om.r[1].z = t*z*y - s*x;
	om.r[2].z = t*z*z + c;
	//om.r[3].z = 0.0f; //optimized out

	/* optimized out
	om.r[0].w = 0.0f;
	om.r[1].w = 0.0f;
	om.r[2].w = 0.0f;
	om.r[3].w = 1.0f;
	*/

	if (bRightSide)
	{
		for (i = 0; i < 4; ++i)
		{
			x = mtx->r[i].x*om.r[0].x + mtx->r[i].y*om.r[1].x + mtx->r[i].z*om.r[2].x;
			y = mtx->r[i].x*om.r[0].y + mtx->r[i].y*om.r[1].y + mtx->r[i].z*om.r[2].y;
			z = mtx->r[i].x*om.r[0].z + mtx->r[i].y*om.r[1].z + mtx->r[i].z*om.r[2].z;

			mtx->r[i].x = x;
			mtx->r[i].y = y;
			mtx->r[i].z = z;
		}
	}
	else
	{
		for (i = 0; i < 3; ++i)
		{
			x = mtx->r[0].x*om.r[i].x + mtx->r[1].x*om.r[i].y + mtx->r[2].x*om.r[i].z;
			y = mtx->r[0].y*om.r[i].x + mtx->r[1].y*om.r[i].y + mtx->r[2].y*om.r[i].z;
			z = mtx->r[0].z*om.r[i].x + mtx->r[1].z*om.r[i].y + mtx->r[2].z*om.r[i].z;
			w = mtx->r[0].w*om.r[i].x + mtx->r[1].w*om.r[i].y + mtx->r[2].w*om.r[i].z;

			om.r[i].x = x;
			om.r[i].y = y;
			om.r[i].z = z;
			om.r[i].w = w;
		}

		for (i = 0; i < 3; ++i)
			mtx->r[i] = om.r[i];
	}
}
