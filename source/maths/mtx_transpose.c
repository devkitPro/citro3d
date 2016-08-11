#include <c3d/maths.h>

void Mtx_Transpose(C3D_Mtx* out)
{
	float swap;
	for (int i = 0; i < 4; i++)
	{
		for (int j = i+1; j < 4; j++)
		{
			swap = out->r[j].c[i];
			out->r[j].c[i] = out->r[i].c[j];
			out->r[i].c[j] = swap;
		}
	}
}
