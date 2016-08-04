#include <c3d/maths.h>

C3D_FQuat Quat_RotateZ(C3D_FQuat q, float r, bool bRightSide)
{
	float c = cosf(r/2.0f);
	float s = sinf(r/2.0f);

	if (bRightSide)
		return Quat_New(q.i*c - q.j*s, q.j*c + q.i*s, q.r*s + q.k*c, q.r*c - q.k*s);
	else
		return Quat_New(q.i*c + q.j*s, q.j*c - q.i*s, q.r*s + q.k*c, q.r*c - q.k*s);
}
