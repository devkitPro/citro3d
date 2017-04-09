#include <c3d/maths.h>

void Mtx_PerspStereoTilt(C3D_Mtx* mtx, float fovx, float invaspect, float near, float far, float iod, float screen, bool isLeftHanded)
{
	// Notes:
	// Once again, we are passed "fovy" and the "aspect ratio"; however the 3DS screens are sideways,
	// and the formula had to be tweaked. With stereo, left/right separation becomes top/bottom separation.
	// The detailed mathematical explanation is in mtx_persptilt.c.

	float fovx_tan = tanf(fovx/2.0f);
	float fovx_tan_invaspect = fovx_tan*invaspect;
	float shift = iod / (2.0f*screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

	Mtx_Zeros(mtx);

	mtx->r[0].y = 1.0f / fovx_tan;
	mtx->r[1].x = -1.0f / fovx_tan_invaspect;
	mtx->r[1].w = iod / 2.0f;
	mtx->r[2].w = near * far / (near - far);
	mtx->r[3].z = isLeftHanded ? 1.0f : -1.0f;
	mtx->r[1].z = -mtx->r[3].z * shift / fovx_tan_invaspect;
	mtx->r[2].z = -mtx->r[3].z * near / (near - far);
}
