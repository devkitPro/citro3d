#include <c3d/maths.h>

void Mtx_OrthoTilt(C3D_Mtx* mtx, float top, float bottom, float left, float right, float near, float far)
{
	// Notes:
	// The 3DS screens are sideways and so are the top/bottom/left/right parameters
	// that the caller passed. Hence why the parameter names are swapped in the implementation.

	C3D_Mtx mp;
	Mtx_Zeros(&mp);

	// Build standard orthogonal projection matrix
	mp.r[0].x = 2.0f / (right - left);
	mp.r[0].w = (left + right) / (left - right);
	mp.r[1].y = 2.0f / (top - bottom);
	mp.r[1].w = (bottom + top) / (bottom - top);
	mp.r[2].z = 2.0f / (far - near);
	mp.r[2].w = (near + far) / (near - far);
	mp.r[3].w = 1.0f;

	// Fix depth range to [-1, 0]
	C3D_Mtx mp2;
	Mtx_Identity(&mp2);
	mp2.r[2].z = 0.5;
	mp2.r[2].w = -0.5;
	Mtx_Multiply(mtx, &mp2, &mp);

	// Rotate the matrix one quarter of a turn CCW in order to fix the 3DS screens' orientation
	Mtx_RotateZ(mtx, M_TAU/4, true);
}
