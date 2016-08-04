#include <c3d/maths.h>

void Mtx_LookAtLH(C3D_Mtx* out, C3D_FVec cameraPosition, C3D_FVec cameraTarget, C3D_FVec cameraUpVector)
{
	//Left-handed Look-At matrix is a DirectX implementation.
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb281710
	C3D_FVec xaxis, yaxis, zaxis;
	
	//Order of operations is crucial.
	zaxis = FVec3_Normalize(FVec3_New(cameraPosition.x - cameraTarget.x, cameraPosition.y - cameraTarget.y, cameraPosition.z - cameraTarget.z));
	xaxis = FVec3_Normalize(FVec3_Cross(cameraUpVector, zaxis));
	yaxis = FVec3_Cross(zaxis, xaxis);
	
	out->r[0].x = xaxis.x;
	out->r[0].y = yaxis.x;
	out->r[0].z = zaxis.x;
	out->r[0].w = 0.0f;
	
	//Notice in LH matrix, the Y is in Row 2 of 4. In RH matrix, the Z is in Row 2 of 4. 
	out->r[1].x = xaxis.y;
	out->r[1].y = yaxis.y;
	out->r[1].z = zaxis.y;
	out->r[1].w = 0.0f;
	
	//Notice in LH matrix, the Z is in Row 3 of 4. In RH matrix, the Y is in Row 3 of 4.
	out->r[2].x = xaxis.z;
	out->r[2].y = yaxis.z;
	out->r[2].z = zaxis.z;
	out->r[2].w = 0.0f;
	
	out->r[3].x = -FVec3_Dot(xaxis, cameraPosition);
	out->r[3].y = -FVec3_Dot(yaxis, cameraPosition);
	out->r[3].z = -FVec3_Dot(zaxis, cameraPosition);
	out->r[3].w = 1.0f;
}
