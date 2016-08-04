#include <c3d/maths.h>

C3D_FQuat Quat_Rotate(C3D_FQuat q, C3D_FVec axis, float r, bool bRightSide)
{
	float halfAngle = r/2.0f;
	float s         = sinf(halfAngle);

	axis = FVec3_Normalize(axis);

	C3D_FQuat tmp = Quat_New(axis.x*s, axis.y*s, axis.z*s, cosf(halfAngle));

	if (bRightSide)
		return Quat_Multiply(tmp, q);
	else
		return Quat_Multiply(q, tmp);
}
