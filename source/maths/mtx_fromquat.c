#include <c3d/maths.h>

void Mtx_FromQuat(C3D_Mtx* m, C3D_FQuat q)
{
	float ii = q.i*q.i;
	float ij = q.i*q.j;
	float ik = q.i*q.k;
	float jj = q.j*q.j;
	float jk = q.j*q.k;
	float kk = q.k*q.k;
	float ri = q.r*q.i;
	float rj = q.r*q.j;
	float rk = q.r*q.k;

	m->r[0].x = 1.0f - (2.0f * (jj + kk));
	m->r[1].x = 2.0f * (ij + rk);
	m->r[2].x = 2.0f * (ik - rj);
	m->r[3].x = 0.0f;

	m->r[0].y = 2.0f * (ij - rk);
	m->r[1].y = 1.0f - (2.0f * (ii + kk));
	m->r[2].y = 2.0f * (jk + ri);
	m->r[3].y = 0.0f;

	m->r[0].z = 2.0f * (ik + rj);
	m->r[1].z = 2.0f * (jk - ri);
	m->r[2].z = 1.0f - (2.0f * (ii + jj));
	m->r[3].z = 0.0f;

	m->r[0].w = 0.0f;
	m->r[1].w = 0.0f;
	m->r[2].w = 0.0f;
	m->r[3].w = 1.0f;
}
