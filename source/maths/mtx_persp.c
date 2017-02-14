#include <c3d/maths.h>

void Mtx_Persp(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, bool isLeftHanded)
{
	float fovy_tan = tanf(fovy/2.0f);

	Mtx_Zeros(mtx);

	// Standard perspective projection matrix, with a fixed depth range of [-1,0] (required by PICA)
	// http://www.wolframalpha.com/input/?i={{1,0,0,0},{0,1,0,0},{0,0,0.5,-0.5},{0,0,0,1}}{{1/(a*tan(v)),0,0,0},{0,1/tan(v),0,0},{0,0,(2%2Bn%2Bf)/(n-f),(fn)/(n-f)},{0,0,-1,0}}

	mtx->r[0].x = 1.0f / (aspect * fovy_tan);
	mtx->r[1].y = 1.0f / fovy_tan;
	mtx->r[2].w = far*near / (near - far);

	if (isLeftHanded)
	{
		mtx->r[2].z = 0.5f*(far + near) / (far - near) - 0.5f;
		mtx->r[3].z = 1.0f;
	}
	else
	{
		mtx->r[2].z = 0.5f*(far + near) / (near - far) + 0.5f;
		mtx->r[3].z = -1.0f;
	}
}
