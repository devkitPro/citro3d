#include <c3d/maths.h>

void Mtx_LookAt(C3D_Mtx* out, C3D_FVec cameraPosition, C3D_FVec cameraTarget, C3D_FVec cameraUpVector, bool isLeftHanded)
{
	//Left-handed and Right-handed Look-At matrix functions are DirectX implementations.
	//Left-handed: https://msdn.microsoft.com/en-us/library/windows/desktop/bb281710
	//Right-handed: https://msdn.microsoft.com/en-us/library/windows/desktop/bb281711
	C3D_FVec xaxis, yaxis, zaxis;
	
	//Order of operations is crucial.
	if (isLeftHanded)
		zaxis = FVec3_Normalize(FVec3_Subtract(cameraTarget, cameraPosition));
	else
		zaxis = FVec3_Normalize(FVec3_Subtract(cameraPosition, cameraTarget));
	xaxis = FVec3_Normalize(FVec3_Cross(cameraUpVector, zaxis));
	yaxis = FVec3_Cross(zaxis, xaxis);
	
	out->r[0].x = xaxis.x;
	out->r[0].y = xaxis.y;
	out->r[0].z = xaxis.z;
	out->r[0].w = -FVec3_Dot(xaxis, cameraPosition);

	out->r[1].x = yaxis.x;
	out->r[1].y = yaxis.y;
	out->r[1].z = yaxis.z;
	out->r[1].w = -FVec3_Dot(yaxis, cameraPosition);

	out->r[2].x = zaxis.x;
	out->r[2].y = zaxis.y;
	out->r[2].z = zaxis.z;
	out->r[2].w = -FVec3_Dot(zaxis, cameraPosition);

	out->r[3].x = 0.0f;
	out->r[3].y = 0.0f;
	out->r[3].z = 0.0f;
	out->r[3].w = 1.0f;
}
