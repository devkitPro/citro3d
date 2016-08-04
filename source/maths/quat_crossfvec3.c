#include <c3d/maths.h>

C3D_FVec Quat_CrossFVec3(C3D_FQuat q, C3D_FVec  v)
{
	C3D_FVec qv  = FVec3_New(q.i, q.j, q.k);
	C3D_FVec uv  = FVec3_Cross(qv, v);
	C3D_FVec uuv = FVec3_Cross(qv, uv);

	uv  = FVec3_Scale(uv,  2.0f * q.r);
	uuv = FVec3_Scale(uuv, 2.0f);

	return FVec3_Add(v, FVec3_Add(uv, uuv));
}
