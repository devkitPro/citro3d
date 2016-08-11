#include <c3d/maths.h>
#include <float.h>

C3D_FQuat Quat_LookAt(C3D_FVec source, C3D_FVec target, C3D_FVec forwardVector, C3D_FVec upVector)
{
	C3D_FVec forward = FVec3_Normalize(FVec3_Subtract(target, source));
	float dot = FVec3_Dot(forwardVector, forward);
	
	//Instead of checking at very high precision, this check includes margin of rounding errors for floats.
	if (dot + 1.0f < 0.0001f)    //If dot product is -1.0f, the resulting quaternion is rotated 180 degrees on the Up axis.
		return Quat_FromAxisAngle(upVector, M_TAU/2.0f);
	if (dot - 1.0f > -0.0001f)    //If dot product is 1.0f, the resulting quaternion is an identity quaternion; there's no rotation.
		return Quat_Identity();
	
	float rotationAngle = acosf(dot);
	C3D_FVec rotationAxis = FVec3_Cross(forwardVector, forward);
	return Quat_FromAxisAngle(rotationAxis, rotationAngle);
}
