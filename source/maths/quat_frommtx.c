#include <c3d/maths.h>

C3D_FQuat Quat_FromMtx(const C3D_Mtx* m)
{
	//Taken from Gamasutra:
	//http://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php
	//Expanded upon from:
	//http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	
	//Variables we need.
	float trace, sqrtTrace;
	C3D_FQuat q;
	
	//Check the main diagonal of the passed-in matrix for positive/negative signs.
	trace = m->r[0].x + m->r[1].y + m->r[2].z;
	if (trace > 0.0f)
	{
		//Diagonal is positive.
		sqrtTrace = sqrtf(trace + 1.0f);
		q.w = sqrtTrace / 2.0f;
		sqrtTrace = 0.5 / sqrtTrace;
		q.x = (m->r[1].z - m->r[2].y) * sqrtTrace;
		q.y = (m->r[2].x - m->r[0].z) * sqrtTrace;
		q.z = (m->r[0].y - m->r[1].x) * sqrtTrace;
	}
	else 
	{
		//Diagonal is negative or equals to zero. We need to identify which major diagonal element has the greatest value.
		if (m->r[0].x > m->r[1].y && m->r[0].x > m->r[2].z)
		{
			sqrtTrace = 2.0f * sqrtf(1.0f + m->r[0].x - m->r[1].y - m->r[2].z);
			q.w = (m->r[2].y - m->r[1].z) / sqrtTrace;
			q.x = 0.25f * sqrtTrace;
			q.y = (m->r[0].y - m->r[1].x) / sqrtTrace;
			q.z = (m->r[0].z - m->r[2].x) / sqrtTrace;
		}
		else if (m->r[1].y > m->r[2].z)
		{
			sqrtTrace = 2.0f * sqrtf(1.0f + m->r[1].y - m->r[0].x - m->r[2].z);
			q.w = (m->r[0].z - m->r[2].x) / sqrtTrace;
			q.x = (m->r[0].y - m->r[1].x) / sqrtTrace;
			q.y = 0.25f * sqrtTrace;
			q.z = (m->r[1].z - m->r[2].y) / sqrtTrace;
		}
		else 
		{
			sqrtTrace = 2.0f * sqrtf(1.0f + m->r[2].z - m->r[0].x - m->r[1].y);
			q.w = (m->r[1].x - m->r[0].y) / sqrtTrace;
			q.x = (m->r[0].z - m->r[2].x) / sqrtTrace;
			q.y = (m->r[1].z - m->r[2].y) / sqrtTrace;
			q.z = 0.25f * sqrtTrace;
		}
	}
	return q;
}
