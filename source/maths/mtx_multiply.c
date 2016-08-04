#include <c3d/maths.h>

void Mtx_Multiply(C3D_Mtx* out, const C3D_Mtx* a, const C3D_Mtx* b)
{
	// if out is a or b, then we need to avoid overwriting them
	if(out == a || out == b)
	{
		C3D_Mtx tmp;
		Mtx_Multiply(&tmp, a, b);
		Mtx_Copy(out, &tmp);
		return;
	}

	// http://www.wolframalpha.com/input/?i={{a,b,c,d},{e,f,g,h},{i,j,k,l},{m,n,o,p}}{{α,β,γ,δ},{ε,θ,ι,κ},{λ,μ,ν,ξ},{ο,π,ρ,σ}}
	int i, j;
	for (j = 0; j < 4; ++j)
		for (i = 0; i < 4; ++i)
			out->r[j].c[i] = a->r[j].x*b->r[0].c[i] + a->r[j].y*b->r[1].c[i] + a->r[j].z*b->r[2].c[i] + a->r[j].w*b->r[3].c[i];
}
