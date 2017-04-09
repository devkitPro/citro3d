#include <c3d/maths.h>

void Mtx_Persp(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, bool isLeftHanded)
{
	float fovy_tan = tanf(fovy/2.0f);

	Mtx_Zeros(mtx);

	// Standard perspective projection matrix, with a fixed depth range of [-1,0] (required by PICA)
	// http://www.wolframalpha.com/input/?i=%7B%7B1,0,0,0%7D,%7B0,1,0,0%7D,%7B0,0,0.5,-0.5%7D,%7B0,0,0,1%7D%7D%7B%7B1%2F(a*tan(v)),0,0,0%7D,%7B0,1%2Ftan(v),0,0%7D,%7B0,0,(n%2Bf)%2F(n-f),(2fn)%2F(n-f)%7D,%7B0,0,-1,0%7D%7D

	mtx->r[0].x = 1.0f / (aspect * fovy_tan);
	mtx->r[1].y = 1.0f / fovy_tan;
	mtx->r[2].w = far*near / (near - far);
	mtx->r[3].z = isLeftHanded ? 1.0f : -1.0f;
	mtx->r[2].z = -mtx->r[3].z * near / (near - far);
}
