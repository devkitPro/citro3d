#include <c3d/maths.h>

C3D_FQuat Quat_RotateY(C3D_FQuat q, float r, bool bRightSide)
{
	float c = cosf(r/2.0f);
	float s = sinf(r/2.0f);

	if (bRightSide)
		return Quat_New(q.i*c + q.k*s, q.r*s + q.j*c, q.k*c - q.i*s, q.r*c - q.j*s);
	else
		return Quat_New(q.i*c - q.k*s, q.r*s + q.j*c, q.k*c + q.i*s, q.r*c - q.j*s);
}
