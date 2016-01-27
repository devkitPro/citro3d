#include <c3d/maths.h>

void Mtx_PerspTilt(C3D_Mtx* mtx, float fovx, float invaspect, float near, float far)
{
	// Notes:
	// We are passed "fovy" and the "aspect ratio". However, the 3DS screens are sideways,
	// and so are these parameters -- in fact, they are actually the fovx and the inverse
	// of the aspect ratio. Therefore the formula for the perspective projection matrix
	// had to be modified to be expressed in these terms instead.

	// Notes:
	// fovx = 2 atan(tan(fovy/2)*w/h)
	// fovy = 2 atan(tan(fovx/2)*h/w)
	// invaspect = h/w

	// a0,0 = h / (w*tan(fovy/2)) =
	//      = h / (w*tan(2 atan(tan(fovx/2)*h/w) / 2)) =
	//      = h / (w*tan( atan(tan(fovx/2)*h/w) )) =
	//      = h / (w * tan(fovx/2)*h/w) =
	//      = 1 / tan(fovx/2)

	// a1,1 = 1 / tan(fovy/2) = (...) = w / (h*tan(fovx/2))

	float fovx_tan = tanf(fovx/2);
	C3D_Mtx mp;
	Mtx_Zeros(&mp);

	// Build standard perspective projection matrix
	mp.r[0].x = 1.0f / fovx_tan;
	mp.r[1].y = 1.0f / (fovx_tan*invaspect);
	mp.r[2].z = (near + far) / (near - far);
	mp.r[2].w = (2 * near * far) / (near - far);
	mp.r[3].z = -1.0f;

	// Fix depth range to [-1, 0]
	C3D_Mtx mp2;
	Mtx_Identity(&mp2);
	mp2.r[2].z = 0.5;
	mp2.r[2].w = -0.5;
	Mtx_Multiply(mtx, &mp2, &mp);

	// Rotate the matrix one quarter of a turn clockwise in order to fix the 3DS screens' orientation
	Mtx_RotateZ(mtx, -M_TAU/4, true);
}
