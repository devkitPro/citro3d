#include <c3d/maths.h>

C3D_FQuat Quat_RotateX(C3D_FQuat q, float r, bool bRightSide)
{
	float c = cosf(r/2.0f);
	float s = sinf(r/2.0f);

	if (bRightSide)
		return Quat_New(q.r*s + q.i*c, q.j*c - q.k*s, q.k*c + q.j*s, q.r*c - q.i*s);
	else
		return Quat_New(q.r*s + q.i*c, q.j*c + q.k*s, q.k*c - q.j*s, q.r*c - q.i*s);
}
