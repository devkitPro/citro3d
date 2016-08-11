#include <c3d/maths.h>

C3D_FQuat Quat_FromAxisAngle(C3D_FVec axis, float angle)
{
	float halfAngle = angle / 2.0f;
	float scale = sinf(halfAngle);
	axis = Quat_Normalize(axis);
	return Quat_New(axis.x * scale, axis.y * scale, axis.z * scale, cosf(halfAngle));
}
