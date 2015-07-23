#include <c3d/maths.h>

void Mtx_Translate(C3D_Mtx* mtx, float x, float y, float z)
{
	C3D_Mtx tm, om;

	Mtx_Identity(&tm);
	tm.r[0].w = x;
	tm.r[1].w = y;
	tm.r[2].w = z;

	Mtx_Multiply(&om, mtx, &tm);
	Mtx_Copy(mtx, &om);
}
