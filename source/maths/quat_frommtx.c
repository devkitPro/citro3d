#include <c3d/maths.h>

C3D_FQuat Quat_FromMtx(const C3D_Mtx* m)
{
	// Original algorithm taken from here (with some optimizations):
	//   https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
	// Layout of the algorithm:
	//   First,  we select a large (non-zero!) component "P" in the output quaternion (q)
	//           (we can test this just by looking at the diagonal of the matrix)
	//   Second, we calculate q' which is our desired output quaternion (q) scaled by 4P
	//           (this can be done with simple additions; the 4P factor is large and non-zero thanks to above)
	//   Third,  we normalize q' to finally obtain q
	//           (this will work because normalize(kq) = q for any k scalar and q unit quaternion)

	C3D_FQuat q;
	C3D_FVec  diagonal = FVec4_New(m->r[0].x, m->r[1].y, m->r[2].z, 1.0f);

	// Check if x^2 + y^2 >= z^2 + w^2
	if (diagonal.z <= 0.0f)
	{
		// Check if |x| >= |y|
		if (diagonal.x >= diagonal.y)
		{
			// X case
			q.x = diagonal.w + diagonal.x - diagonal.y - diagonal.z;
			q.y = m->r[1].x + m->r[0].y;
			q.z = m->r[2].x + m->r[0].z;
			q.w = m->r[2].y - m->r[1].z;
		}
		else
		{
			// Y case
			q.x = m->r[1].x + m->r[0].y;
			q.y = diagonal.w - diagonal.x + diagonal.y - diagonal.z;
			q.z = m->r[2].y + m->r[1].z;
			q.w = m->r[0].z - m->r[2].x;
		}
	}
	else
	{
		// Check if |z| >= |w|
		if (-diagonal.x >= diagonal.y)
		{
			// Z case
			q.x = m->r[2].x + m->r[0].z;
			q.y = m->r[2].y + m->r[1].z;
			q.z = diagonal.w - diagonal.x - diagonal.y + diagonal.z;
			q.w = m->r[1].x - m->r[0].y;
		}
		else
		{
			// W case
			q.x = m->r[2].y - m->r[1].z;
			q.y = m->r[0].z - m->r[2].x;
			q.z = m->r[1].x - m->r[0].y;
			q.w = diagonal.w + diagonal.x + diagonal.y + diagonal.z;
		}
	}

	// Normalize the quaternion
	return Quat_Normalize(q);
}
