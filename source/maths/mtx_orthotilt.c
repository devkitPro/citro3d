#include <c3d/maths.h>

void Mtx_OrthoTilt(C3D_Mtx* mtx, float left, float right, float bottom, float top, float near, float far)
{
	C3D_Mtx mp;
	Mtx_Zeros(&mp);

	// Build standard orthogonal projection matrix
	mp.r[0].x = 2.0f / (right - left);
	mp.r[0].w = (left + right) / (left - right);
	mp.r[1].y = 2.0f / (top - bottom);
	mp.r[1].w = (bottom + top) / (bottom - top);
	mp.r[2].z = 2.0f / (near - far);
	mp.r[2].w = (far + near) / (far - near);
	mp.r[3].w = 1.0f;

	// Fix depth range to [-1, 0]
	C3D_Mtx mp2, mp3;
	Mtx_Identity(&mp2);
	mp2.r[2].z = 0.5;
	mp2.r[2].w = -0.5;
	Mtx_Multiply(&mp3, &mp2, &mp);

	// Fix the 3DS screens' orientation by swapping the X and Y axis
	Mtx_Identity(&mp2);
	mp2.r[0].x = 0.0;
	mp2.r[0].y = 1.0;
	mp2.r[1].x = -1.0; // flipped
	mp2.r[1].y = 0.0;
	Mtx_Multiply(mtx, &mp2, &mp3);
}
