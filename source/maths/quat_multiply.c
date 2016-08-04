#include <c3d/maths.h>

C3D_FQuat Quat_Multiply(C3D_FQuat lhs, C3D_FQuat rhs)
{
	float i = lhs.r*rhs.i + lhs.i*rhs.r + lhs.j*rhs.k - lhs.k*rhs.j;
	float j = lhs.r*rhs.j + lhs.j*rhs.r + lhs.k*rhs.i - lhs.i*rhs.k;
	float k = lhs.r*rhs.k + lhs.k*rhs.r + lhs.i*rhs.j - lhs.j*rhs.i;
	float r = lhs.r*rhs.r - lhs.i*rhs.i - lhs.j*rhs.j - lhs.k*rhs.k;

	return Quat_New(i, j, k, r);
}
