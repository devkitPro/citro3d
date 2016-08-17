#include <c3d/maths.h>

C3D_FQuat Quat_FromPitchYawRoll(float pitch, float yaw, float roll, bool bRightSide)
{
	float pitch_c = cosf(pitch / 2.0f);
	float pitch_s = sinf(pitch / 2.0f);
	float yaw_c = cosf(yaw / 2.0f);
	float yaw_s = sinf(yaw / 2.0f);
	float roll_c = cosf(roll / 2.0f);
	float roll_s = sinf(roll / 2.0f);

	if (bRightSide)
	{
		return Quat_New(
			pitch_s * yaw_c * roll_c - pitch_c * yaw_s * roll_s,
			pitch_c * yaw_s * roll_c + pitch_s * yaw_c * roll_s,
			pitch_c * yaw_c * roll_s - pitch_s * yaw_s * roll_c,
			pitch_c * yaw_c * roll_c + pitch_s * yaw_s * roll_s);
	}
	else
	{
		return Quat_New(
			pitch_s * yaw_c * roll_c + pitch_c * yaw_s * roll_s,
			pitch_c * yaw_s * roll_c - pitch_s * yaw_c * roll_s,
			pitch_c * yaw_c * roll_s + pitch_s * yaw_s * roll_c,
			pitch_c * yaw_c * roll_c - pitch_s * yaw_s * roll_s);
	}
}
