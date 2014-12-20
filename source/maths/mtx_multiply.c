#include <c3d/maths.h>

void Mtx_Multiply(C3D_Mtx* out, const C3D_Mtx* a, const C3D_Mtx* b)
{
	int i, j;
	for (i = 0; i < 4; i ++)
		for (j = 0; j < 4; j ++)
			out->r[j].c[i] = a->r[j].x*b->r[0].c[i] + a->r[j].y*b->r[1].c[i] + a->r[j].z*b->r[2].c[i] + a->r[j].w*b->r[3].c[i];
}
