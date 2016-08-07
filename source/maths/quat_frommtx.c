#include <c3d/maths.h>

void Quat_FromMtx(C3D_FQuat* q, C3D_Mtx m)
{
	//Taken from Gamasutra:
	//http://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php
	//Expanded upon from:
	//http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	
	//Variables we need.
	float trace, sqrtTrace;
	
	//Check the main diagonal of the passed-in matrix for positive/negative signs.
	trace = m.r[0].c[0] + m.r[1].c[1] + m.r[2].c[2];
	if (trace > 0.0f)
	{
		//Diagonal is positive.
		sqrtTrace = sqrtf(trace + 1.0f);
		q->w = sqrtTrace / 2.0f;
		sqrtTrace = 0.5 / sqrtTrace;
		q->x = (m.r[1].c[2] - m.r[2].c[1]) * sqrtTrace;
		q->y = (m.r[2].c[0] - m.r[0].c[2]) * sqrtTrace;
		q->z = (m.r[0].c[1] - m.r[1].c[0]) * sqrtTrace;
	}
	else 
	{
		//Diagonal is negative or equals to zero. We need to identify which major diagonal element has the greatest value.
		if (m.r[0].c[0] > m.r[1].c[1] && m.r[0].c[0] > m.r[2].c[2])
		{
			sqrtTrace = 2.0f * sqrtf(1.0f + m.r[0].c[0] - m.r[1].c[1] - m.r[2].c[2]);
			q->w = (m.r[2].c[1] - m.r[1].c[2]) / sqrtTrace;
			q->x = 0.25f * sqrtTrace;
			q->y = (m.r[0].c[1] - m.r[1].c[0]) / sqrtTrace;
			q->z = (m.r[0].c[2] - m.r[2].c[0]) / sqrtTrace;
		}
		else if (m.r[1].c[1] > m.r[2].c[2])
		{
			sqrtTrace = 2.0f * sqrtf(1.0f + m.r[1].c[1] - m.r[0].c[0] - m.r[2].c[2]);
			q->w = (m.r[0].c[2] - m.r[2].c[0]) / sqrtTrace;
			q->x = (m.r[0].c[1] - m.r[1].c[0]) / sqrtTrace;
			q->y = 0.25f * sqrtTrace;
			q->z = (m.r[1].c[2] - m.r[2].c[1]) / sqrtTrace;
		}
		else 
		{
			sqrtTrace = 2.0f * sqrtf(1.0f + m.r[2].c[2] - m.r[0].c[0] - m.r[1].c[1]);
			q->w = (m.r[1].c[0] - m.r[0].c[1]) / sqrtTrace;
			q->x = (m.r[0].c[2] - m.r[2].c[0]) / sqrtTrace;
			q->y = (m.r[1].c[2] - m.r[2].c[1]) / sqrtTrace;
			q->z = 0.25f * sqrtTrace;
		}
	}
}
