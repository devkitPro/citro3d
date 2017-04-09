#include <c3d/maths.h>

void Mtx_PerspTilt(C3D_Mtx* mtx, float fovx, float invaspect, float near, float far, bool isLeftHanded)
{
	// Notes:
	// We are passed "fovy" and the "aspect ratio". However, the 3DS screens are sideways,
	// and so are these parameters -- in fact, they are actually the fovx and the inverse
	// of the aspect ratio. Therefore the formula for the perspective projection matrix
	// had to be modified to be expressed in these terms instead.

	// Notes:
	// Includes adjusting depth range from [-1,1] to [-1,0]
	// Includes rotation of the matrix one quarter of a turn clockwise in order to fix the 3DS screens' orientation

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

	float fovx_tan = tanf(fovx/2.0f);

	Mtx_Zeros(mtx);

	mtx->r[0].y = 1.0f / fovx_tan;
	mtx->r[1].x = -1.0f / (fovx_tan*invaspect);
	mtx->r[2].w = far*near / (near - far);
	mtx->r[3].z = isLeftHanded ? 1.0f : -1.0f;
	mtx->r[2].z = -mtx->r[3].z * near / (near - far);
}
