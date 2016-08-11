#include <c3d/maths.h>

void Mtx_Transpose(C3D_Mtx* out)
{
	float swap;
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 2-i; j >= 0; j--)
		{
			swap = out->r[i].c[j];
			out->r[i].c[j] = out->r[3-j].c[3-i];
			out->r[3-j].c[3-i] = swap;
		}
	}
}
