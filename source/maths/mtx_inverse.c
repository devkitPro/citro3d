#include <float.h>
#include <c3d/maths.h>

float Mtx_Inverse(C3D_Mtx* out)
{
	//Mtx_Inverse can be used to calculate the determinant and the inverse of the matrix.

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
		//Returns 0.0f if we find the determinant is less than +/- FLT_EPSILON.
		return 0.0f;

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

	for (i = 0; i < 16; i++)
		out->m[i] = inv.m[i] / det;

	return det;
}
