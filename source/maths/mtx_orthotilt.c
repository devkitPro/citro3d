#include <c3d/maths.h>

void Mtx_OrthoTilt(C3D_Mtx* mtx, float left, float right, float bottom, float top, float near, float far, bool isLeftHanded)
{
	Mtx_Zeros(mtx);

	// Standard orthogonal projection matrix, with a fixed depth range of [-1,0] (required by PICA) and rotated τ/4 radians counterclockwise around the Z axis (due to 3DS screen orientation)
	// http://www.wolframalpha.com/input/?i={{0,1,0,0},{-1,0,0,0},{0,0,1,0},{0,0,0,1}}{{1,0,0,0},{0,1,0,0},{0,0,0.5,-0.5},{0,0,0,1}}
	// http://www.wolframalpha.com/input/?i={{0,1,0,0},{-1,0,0,0},{0,0,0.5,-0.5},{0,0,0,1}}{{2/(r-l),0,0,(l%2Br)/(l-r)},{0,2/(t-b),0,(b%2Bt)/(b-t)},{0,0,2/(n-f),(n%2Bf)/(n-f)},{0,0,0,1}}

	mtx->r[0].y = 2.0f / (top - bottom);
	mtx->r[0].w = (bottom + top) / (bottom - top);
	mtx->r[1].x = 2.0f / (left - right);
	mtx->r[1].w = (left + right) / (right - left);
	if (isLeftHanded)
		mtx->r[2].z = 1.0f / (far - near);
	else
		mtx->r[2].z = 1.0f / (near - far);
	mtx->r[2].w = 0.5f*(near + far) / (near - far) - 0.5f;
	mtx->r[3].w = 1.0f;
}
