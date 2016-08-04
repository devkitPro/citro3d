#include <c3d/maths.h>

void Mtx_Identity(C3D_Mtx* out)
{
	// http://www.wolframalpha.com/input/?i={{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}
	int i;
	for (i = 0; i < 16; ++i)
		out->m[i] = 0.0f;
	out->r[0].x = out->r[1].y = out->r[2].z = out->r[3].w = 1.0f;
}
