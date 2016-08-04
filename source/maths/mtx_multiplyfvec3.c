#include <c3d/maths.h>

C3D_FVec Mtx_MultiplyFVec3(const C3D_Mtx* mtx, C3D_FVec v)
{
	// http://www.wolframalpha.com/input/?i={{a,b,c},{d,e,f},{g,h,i}}{x,y,z}
	float x = FVec3_Dot(mtx->r[0], v);
	float y = FVec3_Dot(mtx->r[1], v);
	float z = FVec3_Dot(mtx->r[2], v);

	return FVec3_New(x, y, z);
}
