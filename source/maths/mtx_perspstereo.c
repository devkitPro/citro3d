#include <c3d/maths.h>

void Mtx_PerspStereoTilt(C3D_Mtx* mtx, float fovx, float invaspect, float near, float far, float iod, float screen)
{
	// Notes:
	// Once again, we are passed "fovy" and the "aspect ratio"; however the 3DS screens are sideways,
	// and the formula had to be tweaked. With stereo, left/right separation becomes top/bottom separation.
	// The detailed mathematical explanation is in mtx_persptilt.c.

	float fovx_tan = tanf(fovx/2);
	float fovx_tan_invaspect = fovx_tan*invaspect;
	float shift = iod / (2*screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

	C3D_Mtx mp;
	Mtx_Zeros(&mp);

	// Build asymmetric perspective projection matrix
	mp.r[0].x = 1.0f / fovx_tan;
	mp.r[1].y = 1.0f / fovx_tan_invaspect;
	mp.r[1].z = shift / fovx_tan_invaspect;
	mp.r[2].z = (near + far) / (near - far);
	mp.r[2].w = (2 * near * far) / (near - far);
	mp.r[3].z = -1.0f;

	// Fix depth range to [-1, 0]
	C3D_Mtx mp2;
	Mtx_Identity(&mp2);
	mp2.r[2].z = 0.5;
	mp2.r[2].w = -0.5;
	Mtx_Multiply(mtx, &mp2, &mp);

	// Translate to screen plane
	Mtx_Translate(mtx, 0, iod/2, 0);

	// Rotate the matrix one quarter of a turn CCW in order to fix the 3DS screens' orientation
	Mtx_RotateZ(mtx, M_TAU/4, true);
}
