#include <c3d/maths.h>

C3D_FVec Mtx_MultiplyFVec4(const C3D_Mtx* mtx, C3D_FVec v)
{
	// http://www.wolframalpha.com/input/?i={{a,b,c,d},{e,f,g,h},{i,j,k,l},{m,n,o,p}}{x,y,z,w}
	float x = FVec4_Dot(mtx->r[0], v);
	float y = FVec4_Dot(mtx->r[1], v);
	float z = FVec4_Dot(mtx->r[2], v);
	float w = FVec4_Dot(mtx->r[3], v);

	return FVec4_New(x, y, z, w);
}
