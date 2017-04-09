#include <c3d/maths.h>

void Mtx_PerspStereo(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, float iod, float screen, bool isLeftHanded)
{
	float fovy_tan = tanf(fovy/2.0f);
	float fovy_tan_aspect = fovy_tan*aspect;
	float shift = iod / (2.0f*screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

	Mtx_Zeros(mtx);

	mtx->r[0].x = 1.0f / fovy_tan_aspect;
	mtx->r[0].w = -iod / 2.0f;
	mtx->r[1].y = 1.0f / fovy_tan;
	mtx->r[2].w = near * far / (near - far);
	mtx->r[3].z = isLeftHanded ? 1.0f : -1.0f;
	mtx->r[0].z = mtx->r[3].z * shift / fovy_tan_aspect;
	mtx->r[2].z = -mtx->r[3].z * near / (near - far);
}
