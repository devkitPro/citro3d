#include <c3d/maths.h>

bool Mtx_Inverse(C3D_Mtx* out)
{
	C3D_Mtx inv;
	float   det;
	int     i;

	inv.r[0].x =  out->r[1].y * out->r[2].z * out->r[3].w -
	              out->r[1].y * out->r[2].w * out->r[3].z -
	              out->r[2].y * out->r[1].z * out->r[3].w +
	              out->r[2].y * out->r[1].w * out->r[3].z +
	              out->r[3].y * out->r[1].z * out->r[2].w -
	              out->r[3].y * out->r[1].w * out->r[2].z;

	inv.r[1].x = -out->r[1].x * out->r[2].z * out->r[3].w +
	              out->r[1].x * out->r[2].w * out->r[3].z +
	              out->r[2].x * out->r[1].z * out->r[3].w -
	              out->r[2].x * out->r[1].w * out->r[3].z -
	              out->r[3].x * out->r[1].z * out->r[2].w +
	              out->r[3].x * out->r[1].w * out->r[2].z;

	inv.r[2].x =  out->r[1].x * out->r[2].y * out->r[3].w -
	              out->r[1].x * out->r[2].w * out->r[3].y -
	              out->r[2].x * out->r[1].y * out->r[3].w +
	              out->r[2].x * out->r[1].w * out->r[3].y +
	              out->r[3].x * out->r[1].y * out->r[2].w -
	              out->r[3].x * out->r[1].w * out->r[2].y;

	inv.r[3].x = -out->r[1].x * out->r[2].y * out->r[3].z +
	              out->r[1].x * out->r[2].z * out->r[3].y +
	              out->r[2].x * out->r[1].y * out->r[3].z -
	              out->r[2].x * out->r[1].z * out->r[3].y -
	              out->r[3].x * out->r[1].y * out->r[2].z +
	              out->r[3].x * out->r[1].z * out->r[2].y;

	det = out->r[0].x * inv.r[0].x + out->r[0].y * inv.r[1].x +
	      out->r[0].z * inv.r[2].x + out->r[0].w * inv.r[3].x;

	if (fabsf(det) < FLT_EPSILON)
		return false;

	inv.r[0].y = -out->r[0].y * out->r[2].z * out->r[3].w +
	              out->r[0].y * out->r[2].w * out->r[3].z +
	              out->r[2].y * out->r[0].z * out->r[3].w -
	              out->r[2].y * out->r[0].w * out->r[3].z -
	              out->r[3].y * out->r[0].z * out->r[2].w +
	              out->r[3].y * out->r[0].w * out->r[2].z;

	inv.r[1].y =  out->r[0].x * out->r[2].z * out->r[3].w -
	              out->r[0].x * out->r[2].w * out->r[3].z -
	              out->r[2].x * out->r[0].z * out->r[3].w +
	              out->r[2].x * out->r[0].w * out->r[3].z +
	              out->r[3].x * out->r[0].z * out->r[2].w -
	              out->r[3].x * out->r[0].w * out->r[2].z;

	inv.r[2].y = -out->r[0].x * out->r[2].y * out->r[3].w +
	              out->r[0].x * out->r[2].w * out->r[3].y +
	              out->r[2].x * out->r[0].y * out->r[3].w -
	              out->r[2].x * out->r[0].w * out->r[3].y -
	              out->r[3].x * out->r[0].y * out->r[2].w +
	              out->r[3].x * out->r[0].w * out->r[2].y;

	inv.r[3].y =  out->r[0].x * out->r[2].y * out->r[3].z -
	              out->r[0].x * out->r[2].z * out->r[3].y -
	              out->r[2].x * out->r[0].y * out->r[3].z +
	              out->r[2].x * out->r[0].z * out->r[3].y +
	              out->r[3].x * out->r[0].y * out->r[2].z -
	              out->r[3].x * out->r[0].z * out->r[2].y;

	inv.r[0].z =  out->r[0].y * out->r[1].z * out->r[3].w -
	              out->r[0].y * out->r[1].w * out->r[3].z -
	              out->r[1].y * out->r[0].z * out->r[3].w +
	              out->r[1].y * out->r[0].w * out->r[3].z +
	              out->r[3].y * out->r[0].z * out->r[1].w -
	              out->r[3].y * out->r[0].w * out->r[1].z;

	inv.r[1].z = -out->r[0].x * out->r[1].z * out->r[3].w +
	              out->r[0].x * out->r[1].w * out->r[3].z +
	              out->r[1].x * out->r[0].z * out->r[3].w -
	              out->r[1].x * out->r[0].w * out->r[3].z -
	              out->r[3].x * out->r[0].z * out->r[1].w +
	              out->r[3].x * out->r[0].w * out->r[1].z;

	inv.r[2].z =  out->r[0].x * out->r[1].y * out->r[3].w -
	              out->r[0].x * out->r[1].w * out->r[3].y -
	              out->r[1].x * out->r[0].y * out->r[3].w +
	              out->r[1].x * out->r[0].w * out->r[3].y +
	              out->r[3].x * out->r[0].y * out->r[1].w -
	              out->r[3].x * out->r[0].w * out->r[1].y;

	inv.r[3].z = -out->r[0].x * out->r[1].y * out->r[3].z +
	              out->r[0].x * out->r[1].z * out->r[3].y +
	              out->r[1].x * out->r[0].y * out->r[3].z -
	              out->r[1].x * out->r[0].z * out->r[3].y -
	              out->r[3].x * out->r[0].y * out->r[1].z +
	              out->r[3].x * out->r[0].z * out->r[1].y;

	inv.r[0].w = -out->r[0].y * out->r[1].z * out->r[2].w +
	              out->r[0].y * out->r[1].w * out->r[2].z +
	              out->r[1].y * out->r[0].z * out->r[2].w -
	              out->r[1].y * out->r[0].w * out->r[2].z -
	              out->r[2].y * out->r[0].z * out->r[1].w +
	              out->r[2].y * out->r[0].w * out->r[1].z;

	inv.r[1].w =  out->r[0].x * out->r[1].z * out->r[2].w -
	              out->r[0].x * out->r[1].w * out->r[2].z -
	              out->r[1].x * out->r[0].z * out->r[2].w +
	              out->r[1].x * out->r[0].w * out->r[2].z +
	              out->r[2].x * out->r[0].z * out->r[1].w -
	              out->r[2].x * out->r[0].w * out->r[1].z;

	inv.r[2].w = -out->r[0].x * out->r[1].y * out->r[2].w +
	              out->r[0].x * out->r[1].w * out->r[2].y +
	              out->r[1].x * out->r[0].y * out->r[2].w -
	              out->r[1].x * out->r[0].w * out->r[2].y -
	              out->r[2].x * out->r[0].y * out->r[1].w +
	              out->r[2].x * out->r[0].w * out->r[1].y;

	inv.r[3].w =  out->r[0].x * out->r[1].y * out->r[2].z -
	              out->r[0].x * out->r[1].z * out->r[2].y -
	              out->r[1].x * out->r[0].y * out->r[2].z +
	              out->r[1].x * out->r[0].z * out->r[2].y +
	              out->r[2].x * out->r[0].y * out->r[1].z -
	              out->r[2].x * out->r[0].z * out->r[1].y;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		out->m[i] = inv.m[i] * det;

	return true;
}
