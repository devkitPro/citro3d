#pragma once
#include "types.h"
#include <math.h>
#include <string.h>

/**
 * @addtogroup math_support
 * @brief Implementations of matrix, vector, and quaternion operations.
 * @{
 */

/**
 * The one true circumference-to-radius ratio.
 * See http://tauday.com/tau-manifesto
 */
#define M_TAU (2*M_PI)

/**
 * @brief Convert an angle from revolutions to radians
 * @param[in] _angle Proportion of a full revolution
 * @return Angle in radians
 */
#define C3D_Angle(_angle) ((_angle)*M_TAU)

/**
 * @brief Convert an angle from degrees to radians
 * @param[in] _angle Angle in degrees
 * @return Angle in radians
 */
#define C3D_AngleFromDegrees(_angle) ((_angle)*M_TAU/360.0f)

#define C3D_AspectRatioTop (400.0f / 240.0f) ///< Aspect ratio for 3DS top screen
#define C3D_AspectRatioBot (320.0f / 240.0f) ///< Aspect ratio for 3DS bottom screen

/**
 * @name Vector Math
 * @{
 */

/**
 * @brief Create a new FVec4
 * @param[in] x X-component
 * @param[in] y Y-component
 * @param[in] z Z-component
 * @param[in] w W-component
 * @return New FVec4
 */
static inline C3D_FVec FVec4_New(float x, float y, float z, float w)
{
	return (C3D_FVec){{ w, z, y, x }};
}

/**
 * @brief Add two FVec4s
 * @param[in] lhs Augend
 * @param[in] rhs Addend
 * @return lhs+rhs (sum)
 */
static inline C3D_FVec FVec4_Add(C3D_FVec lhs, C3D_FVec rhs)
{
	// component-wise addition
	return FVec4_New(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z, lhs.w+rhs.w);
}

/**
 * @brief Subtract two FVec4s
 * @param[in] lhs Minuend
 * @param[in] rhs Subtrahend
 * @return lhs-rhs (difference)
 */
static inline C3D_FVec FVec4_Subtract(C3D_FVec lhs, C3D_FVec rhs)
{
	// component-wise subtraction
	return FVec4_New(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z, lhs.w-rhs.w);
}

/**
 * @brief Negate a FVec4
 * @note This is equivalent to `FVec4_Scale(v, -1)`
 * @param[in] v Vector to negate
 * @return -v
 */
static inline C3D_FVec FVec4_Negate(C3D_FVec v)
{
	// component-wise negation
	return FVec4_New(-v.x, -v.y, -v.z, -v.w);
}

/**
 * @brief Scale a FVec4
 * @param[in] v Vector to scale
 * @param[in] s Scale factor
 * @return v*s
 */
static inline C3D_FVec FVec4_Scale(C3D_FVec v, float s)
{
	// component-wise scaling
	return FVec4_New(v.x*s, v.y*s, v.z*s, v.w*s);
}

/**
 * @brief Perspective divide
 * @param[in] v Vector to divide
 * @return v/v.w
 */
static inline C3D_FVec FVec4_PerspDivide(C3D_FVec v)
{
	// divide by w
	return FVec4_New(v.x/v.w, v.y/v.w, v.z/v.w, 1.0f);
}

/**
 * @brief Dot product of two FVec4s
 * @param[in] lhs Left-side FVec4
 * @param[in] rhs Right-side FVec4
 * @return lhs∙rhs
 */
static inline float FVec4_Dot(C3D_FVec lhs, C3D_FVec rhs)
{
	// A∙B = sum of component-wise products
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
}

/**
 * @brief Magnitude of a FVec4
 * @param[in] v Vector
 * @return ‖v‖
 */
static inline float FVec4_Magnitude(C3D_FVec v)
{
	// ‖v‖ = √(v∙v)
	return sqrtf(FVec4_Dot(v,v));
}

/**
 * @brief Normalize a FVec4
 * @param[in] v FVec4 to normalize
 * @return v/‖v‖
 */
static inline C3D_FVec FVec4_Normalize(C3D_FVec v)
{
	// get vector magnitude
	float m = FVec4_Magnitude(v);

	// scale by inverse magnitude to get a unit vector
	return FVec4_New(v.x/m, v.y/m, v.z/m, v.w/m);
}

/**
 * @brief Create a new FVec3
 * @param[in] x X-component
 * @param[in] y Y-component
 * @param[in] z Z-component
 * @return New FVec3
 */
static inline C3D_FVec FVec3_New(float x, float y, float z)
{
	return FVec4_New(x, y, z, 0.0f);
}

/**
 * @brief Dot product of two FVec3s
 * @param[in] lhs Left-side FVec3
 * @param[in] rhs Right-side FVec3
 * @return lhs∙rhs
 */
static inline float FVec3_Dot(C3D_FVec lhs, C3D_FVec rhs)
{
	// A∙B = sum of component-wise products
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

/**
 * @brief Magnitude of a FVec3
 * @param[in] v Vector
 * @return ‖v‖
 */
static inline float FVec3_Magnitude(C3D_FVec v)
{
	// ‖v‖ = √(v∙v)
	return sqrtf(FVec3_Dot(v,v));
}

/**
 * @brief Normalize a FVec3
 * @param[in] v FVec3 to normalize
 * @return v/‖v‖
 */
static inline C3D_FVec FVec3_Normalize(C3D_FVec v)
{
	// get vector magnitude
	float m = FVec3_Magnitude(v);

	// scale by inverse magnitude to get a unit vector
	return FVec3_New(v.x/m, v.y/m, v.z/m);
}

/**
 * @brief Add two FVec3s
 * @param[in] lhs Augend
 * @param[in] rhs Addend
 * @return lhs+rhs (sum)
 */
static inline C3D_FVec FVec3_Add(C3D_FVec lhs, C3D_FVec rhs)
{
	// component-wise addition
	return FVec3_New(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
}

/**
 * @brief Subtract two FVec3s
 * @param[in] lhs Minuend
 * @param[in] rhs Subtrahend
 * @return lhs-rhs (difference)
 */
static inline C3D_FVec FVec3_Subtract(C3D_FVec lhs, C3D_FVec rhs)
{
	// component-wise subtraction
	return FVec3_New(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z);
}

/**
 * @brief Distance between two 3D points
 * @param[in] lhs Relative origin
 * @param[in] rhs Relative point of interest
 * @return ‖lhs-rhs‖
 */
static inline float FVec3_Distance(C3D_FVec lhs, C3D_FVec rhs)
{
        // distance = ‖lhs-rhs‖
	return FVec3_Magnitude(FVec3_Subtract(lhs, rhs));
}

/**
 * @brief Scale a FVec3
 * @param[in] v Vector to scale
 * @param[in] s Scale factor
 * @return v*s
 */
static inline C3D_FVec FVec3_Scale(C3D_FVec v, float s)
{
	// component-wise scaling
	return FVec3_New(v.x*s, v.y*s, v.z*s);
}

/**
 * @brief Negate a FVec3
 * @note This is equivalent to `FVec3_Scale(v, -1)`
 * @param[in] v Vector to negate
 * @return -v
 */
static inline C3D_FVec FVec3_Negate(C3D_FVec v)
{
	// component-wise negation
	return FVec3_New(-v.x, -v.y, -v.z);
}

/**
 * @brief Cross product of two FVec3s
 * @note This returns a pseudo-vector which is perpendicular to the plane
 *       spanned by the two input vectors.
 * @param[in] lhs Left-side FVec3
 * @param[in] rhs Right-side FVec3
 * @return lhs×rhs
 */
static inline C3D_FVec FVec3_Cross(C3D_FVec lhs, C3D_FVec rhs)
{
	// A×B = (AyBz - AzBy, AzBx - AxBz, AxBy - AyBx)
	return FVec3_New(lhs.y*rhs.z - lhs.z*rhs.y, lhs.z*rhs.x - lhs.x*rhs.z, lhs.x*rhs.y - lhs.y*rhs.x);
}
/** @} */

/**
 * @name Matrix Math
 * @note All matrices are 4x4 unless otherwise noted.
 * @{
 */

/**
 * @brief Zero matrix
 * @param[out] out Matrix to zero
 */
static inline void Mtx_Zeros(C3D_Mtx* out)
{
	memset(out, 0, sizeof(*out));
}

/**
 * @brief Copy a matrix
 * @param[out] out Output matrix
 * @param[in]  in  Input matrix
 */
static inline void Mtx_Copy(C3D_Mtx* out, const C3D_Mtx* in)
{
	*out = *in;
}

/**
 * @brief Creates a matrix with the diagonal using the given parameters.
 * @param[out]  out    Output matrix.
 * @param[in]   x      The X component.
 * @param[in]   y      The Y component.
 * @param[in]   z      The Z component.
 * @param[in]   w      The W component.
 */
static inline void Mtx_Diagonal(C3D_Mtx* out, float x, float y, float z, float w)
{
	Mtx_Zeros(out);
	out->r[0].x = x;
	out->r[1].y = y;
	out->r[2].z = z;
	out->r[3].w = w;
}

/**
 * @brief Identity matrix
 * @param[out] out Matrix to fill
 */
static inline void Mtx_Identity(C3D_Mtx* out)
{
	Mtx_Diagonal(out, 1.0f, 1.0f, 1.0f, 1.0f);
}

/**
 *@brief Transposes the matrix. Row => Column, and vice versa.
 *@param[in,out] out     Output matrix.
 */
void Mtx_Transpose(C3D_Mtx* out);

/**
 * @brief Matrix addition
 * @param[out]   out    Output matrix.
 * @param[in]    lhs    Left matrix.
 * @param[in]    rhs    Right matrix.
 * @return lhs+rhs (sum)
 */
static inline void Mtx_Add(C3D_Mtx* out, const C3D_Mtx* lhs, const C3D_Mtx* rhs)
{
	for (int i = 0; i < 16; i++)
		out->m[i] = lhs->m[i] + rhs->m[i];
}

/**
 * @brief Matrix subtraction
 * @param[out]   out    Output matrix.
 * @param[in]    lhs    Left matrix.
 * @param[in]    rhs    Right matrix.
 * @return lhs-rhs (difference)
 */
static inline void Mtx_Subtract(C3D_Mtx* out, const C3D_Mtx* lhs, const C3D_Mtx* rhs)
{
	for (int i = 0; i < 16; i++)
		out->m[i] = lhs->m[i] - rhs->m[i];
}

/**
 * @brief Multiply two matrices
 * @param[out] out Output matrix
 * @param[in]  a   Multiplicand
 * @param[in]  b   Multiplier
 */
void Mtx_Multiply(C3D_Mtx* out, const C3D_Mtx* a, const C3D_Mtx* b);

/**
 * @brief Inverse a matrix
 * @param[in,out] out Matrix to inverse
 * @retval 0.0f Degenerate matrix (no inverse)
 * @return determinant
 */
float Mtx_Inverse(C3D_Mtx* out);

/**
 * @brief Multiply 3x3 matrix by a FVec3
 * @param[in] mtx Matrix
 * @param[in] v   Vector
 * @return mtx*v (product)
 */
C3D_FVec Mtx_MultiplyFVec3(const C3D_Mtx* mtx, C3D_FVec v);

/**
 * @brief Multiply 4x4 matrix by a FVec4
 * @param[in] mtx Matrix
 * @param[in] v   Vector
 * @return mtx*v (product)
 */
C3D_FVec Mtx_MultiplyFVec4(const C3D_Mtx* mtx, C3D_FVec v);

/**
 * @brief Multiply 4x3 matrix by a FVec3
 * @param[in] mtx Matrix
 * @param[in] v   Vector
 * @return mtx*v (product)
 */
static inline C3D_FVec Mtx_MultiplyFVecH(const C3D_Mtx* mtx, C3D_FVec v)
{
	v.w = 1.0f;

	return Mtx_MultiplyFVec4(mtx, v);
}

/**
 * @brief Get 4x4 matrix equivalent to Quaternion
 * @param[out] m Output matrix
 * @param[in]  q Input Quaternion
 */
void Mtx_FromQuat(C3D_Mtx* m, C3D_FQuat q);
/** @} */

/**
 * @name 3D Transformation Matrix Math
 * @note bRightSide is used to determine which side to perform the transformation.
 *       With an input matrix A and a transformation matrix B, bRightSide being
 *       true yields AB, while being false yield BA.
 * @{
 */

/**
 * @brief 3D translation
 * @param[in,out] mtx Matrix to translate
 * @param[in]     x            X component to translate
 * @param[in]     y            Y component to translate
 * @param[in]     z            Z component to translate
 * @param[in]     bRightSide   Whether to transform from the right side
 */
void Mtx_Translate(C3D_Mtx* mtx, float x, float y, float z, bool bRightSide);

/**
 * @brief 3D Scale
 * @param[in,out] mtx Matrix to scale
 * @param[in]     x   X component to scale
 * @param[in]     y   Y component to scale
 * @param[in]     z   Z component to scale
 */
void Mtx_Scale(C3D_Mtx* mtx, float x, float y, float z);

/**
 * @brief 3D Rotation
 * @param[in,out] mtx        Matrix to rotate
 * @param[in]     axis       Axis about which to rotate
 * @param[in]     angle      Radians to rotate
 * @param[in]     bRightSide Whether to transform from the right side
 */
void Mtx_Rotate(C3D_Mtx* mtx, C3D_FVec axis, float angle, bool bRightSide);

/**
 * @brief 3D Rotation about the X axis
 * @param[in,out] mtx        Matrix to rotate
 * @param[in]     angle      Radians to rotate
 * @param[in]     bRightSide Whether to transform from the right side
 */
void Mtx_RotateX(C3D_Mtx* mtx, float angle, bool bRightSide);

/**
 * @brief 3D Rotation about the Y axis
 * @param[in,out] mtx        Matrix to rotate
 * @param[in]     angle      Radians to rotate
 * @param[in]     bRightSide Whether to transform from the right side
 */
void Mtx_RotateY(C3D_Mtx* mtx, float angle, bool bRightSide);

/**
 * @brief 3D Rotation about the Z axis
 * @param[in,out] mtx        Matrix to rotate
 * @param[in]     angle      Radians to rotate
 * @param[in]     bRightSide Whether to transform from the right side
 */
void Mtx_RotateZ(C3D_Mtx* mtx, float angle, bool bRightSide);
/** @} */

/**
 * @name 3D Projection Matrix Math
 * @{
 */

/**
 * @brief Orthogonal projection
 * @param[out] mtx Output matrix
 * @param[in]  left         Left clip plane (X=left)
 * @param[in]  right        Right clip plane (X=right)
 * @param[in]  bottom       Bottom clip plane (Y=bottom)
 * @param[in]  top          Top clip plane (Y=top)
 * @param[in]  near         Near clip plane (Z=near)
 * @param[in]  far          Far clip plane (Z=far)
 * @param[in]  isLeftHanded Whether to build a LH projection
 * @sa Mtx_OrthoTilt
 */
void Mtx_Ortho(C3D_Mtx* mtx, float left, float right, float bottom, float top, float near, float far, bool isLeftHanded);

/**
 * @brief Perspective projection
 * @param[out] mtx          Output matrix
 * @param[in]  fovy         Vertical field of view in radians
 * @param[in]  aspect       Aspect ration of projection plane (width/height)
 * @param[in]  near         Near clip plane (Z=near)
 * @param[in]  far          Far clip plane (Z=far)
 * @param[in]  isLeftHanded Whether to build a LH projection
 * @sa Mtx_PerspTilt
 * @sa Mtx_PerspStereo
 * @sa Mtx_PerspStereoTilt
 */
void Mtx_Persp(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, bool isLeftHanded);

/**
 * @brief Stereo perspective projection
 * @note Typically you will use iod to mean the distance between the eyes. Plug
 *       in -iod for the left eye and iod for the right eye.
 * @note The focal length is defined by screen. If objects are further than this,
 *       they will appear to be inside the screen. If objects are closer than this,
 *       they will appear to pop out of the screen. Objects at this distance appear
 *       to be at the screen.
 * @param[out] mtx          Output matrix
 * @param[in]  fovy         Vertical field of view in radians
 * @param[in]  aspect       Aspect ration of projection plane (width/height)
 * @param[in]  near         Near clip plane (Z=near)
 * @param[in]  far          Far clip plane (Z=far)
 * @param[in]  iod          Interocular distance
 * @param[in]  screen       Focal length
 * @param[in]  isLeftHanded Whether to build a LH projection
 * @sa Mtx_Persp
 * @sa Mtx_PerspTilt
 * @sa Mtx_PerspStereoTilt
 */
void Mtx_PerspStereo(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, float iod, float screen, bool isLeftHanded);

/**
 * @brief Orthogonal projection, tilted to account for the 3DS screen rotation
 * @param[out] mtx          Output matrix
 * @param[in]  left         Left clip plane (X=left)
 * @param[in]  right        Right clip plane (X=right)
 * @param[in]  bottom       Bottom clip plane (Y=bottom)
 * @param[in]  top          Top clip plane (Y=top)
 * @param[in]  near         Near clip plane (Z=near)
 * @param[in]  far          Far clip plane (Z=far)
 * @param[in]  isLeftHanded Whether to build a LH projection
 * @sa Mtx_Ortho
 */
void Mtx_OrthoTilt(C3D_Mtx* mtx, float left, float right, float bottom, float top, float near, float far, bool isLeftHanded);

/**
 * @brief Perspective projection, tilted to account for the 3DS screen rotation
 * @param[out] mtx          Output matrix
 * @param[in]  fovy         Vertical field of view in radians
 * @param[in]  aspect       Aspect ration of projection plane (width/height)
 * @param[in]  near         Near clip plane (Z=near)
 * @param[in]  far          Far clip plane (Z=far)
 * @param[in]  isLeftHanded Whether to build a LH projection
 * @sa Mtx_Persp
 * @sa Mtx_PerspStereo
 * @sa Mtx_PerspStereoTilt
 */
void Mtx_PerspTilt(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, bool isLeftHanded);

/**
 * @brief Stereo perspective projection, tilted to account for the 3DS screen rotation
 * @note See the notes for @ref Mtx_PerspStereo
 * @param[out] mtx          Output matrix
 * @param[in]  fovy         Vertical field of view in radians
 * @param[in]  aspect       Aspect ration of projection plane (width/height)
 * @param[in]  near         Near clip plane (Z=near)
 * @param[in]  far          Far clip plane (Z=far)
 * @param[in]  iod          Interocular distance
 * @param[in]  screen       Focal length
 * @param[in]  isLeftHanded Whether to build a LH projection
 * @sa Mtx_Persp
 * @sa Mtx_PerspTilt
 * @sa Mtx_PerspStereo
 */
void Mtx_PerspStereoTilt(C3D_Mtx* mtx, float fovy, float aspect, float near, float far, float iod, float screen, bool isLeftHanded);

/**
 * @brief Look-At matrix, based on DirectX implementation
 * @note See https://msdn.microsoft.com/en-us/library/windows/desktop/bb205342
 * @param[out] out            Output matrix.
 * @param[in]  cameraPosition Position of the intended camera in 3D space.
 * @param[in]  cameraTarget   Position of the intended target the camera is supposed to face in 3D space.
 * @param[in]  cameraUpVector The vector that points straight up depending on the camera's "Up" direction.
 * @param[in]  isLeftHanded   Whether to build a LH projection
 */
void Mtx_LookAt(C3D_Mtx* out, C3D_FVec cameraPosition, C3D_FVec cameraTarget, C3D_FVec cameraUpVector, bool isLeftHanded);
/** @} */

/**
 * @name Quaternion Math
 * @{
 */

/**
 * @brief Create a new Quaternion
 * @param[in] i I-component
 * @param[in] j J-component
 * @param[in] k K-component
 * @param[in] r Real component
 * @return New Quaternion
 */
#define Quat_New(i,j,k,r) FVec4_New(i,j,k,r)

/**
 * @brief Negate a Quaternion
 * @note This is equivalent to `Quat_Scale(v, -1)`
 * @param[in] q Quaternion to negate
 * @return -q
 */
#define Quat_Negate(q) FVec4_Negate(q)

/**
 * @brief Add two Quaternions
 * @param[in] lhs Augend
 * @param[in] rhs Addend
 * @return lhs+rhs (sum)
 */
#define Quat_Add(lhs,rhs) FVec4_Add(lhs,rhs)

/**
 * @brief Subtract two Quaternions
 * @param[in] lhs Minuend
 * @param[in] rhs Subtrahend
 * @return lhs-rhs (difference)
 */
#define Quat_Subtract(lhs,rhs) FVec4_Subtract(lhs,rhs)

/**
 * @brief Scale a Quaternion
 * @param[in] q Quaternion to scale
 * @param[in] s Scale factor
 * @return q*s
 */
#define Quat_Scale(q,s) FVec4_Scale(q,s)

/**
 * @brief Normalize a Quaternion
 * @param[in] q Quaternion to normalize
 * @return q/‖q‖
 */
#define Quat_Normalize(q) FVec4_Normalize(q)

/**
 * @brief Dot product of two Quaternions
 * @param[in] lhs Left-side Quaternion
 * @param[in] rhs Right-side Quaternion
 * @return lhs∙rhs
 */
#define Quat_Dot(lhs,rhs) FVec4_Dot(lhs,rhs)

/**
 * @brief Multiply two Quaternions
 * @param[in] lhs Multiplicand
 * @param[in] rhs Multiplier
 * @return lhs*rhs
 */
C3D_FQuat Quat_Multiply(C3D_FQuat lhs, C3D_FQuat rhs);

/**
 * @brief Raise Quaternion to a power
 * @note If p is 0, this returns the identity Quaternion.
 *       If p is 1, this returns q.
 * @param[in] q Base Quaternion
 * @param[in] p Power
 * @return q<sup>p</sup>
 */
C3D_FQuat Quat_Pow(C3D_FQuat q, float p);

/**
 * @brief Cross product of Quaternion and FVec3
 * @param[in] q Base Quaternion
 * @param[in] v Vector to cross
 * @return q×v
 */
C3D_FVec Quat_CrossFVec3(C3D_FQuat q, C3D_FVec v);

/**
 * @brief 3D Rotation
 * @param[in] q          Quaternion to rotate
 * @param[in] axis       Axis about which to rotate
 * @param[in] r          Radians to rotate
 * @param[in] bRightSide Whether to transform from the right side
 * @return Rotated Quaternion
 */
C3D_FQuat Quat_Rotate(C3D_FQuat q, C3D_FVec axis, float r, bool bRightSide);

/**
 * @brief 3D Rotation about the X axis
 * @param[in] q          Quaternion to rotate
 * @param[in] r          Radians to rotate
 * @param[in] bRightSide Whether to transform from the right side
 * @return Rotated Quaternion
 */
C3D_FQuat Quat_RotateX(C3D_FQuat q, float r, bool bRightSide);

/**
 * @brief 3D Rotation about the Y axis
 * @param[in] q          Quaternion to rotate
 * @param[in] r          Radians to rotate
 * @param[in] bRightSide Whether to transform from the right side
 * @return Rotated Quaternion
 */
C3D_FQuat Quat_RotateY(C3D_FQuat q, float r, bool bRightSide);

/**
 * @brief 3D Rotation about the Z axis
 * @param[in] q          Quaternion to rotate
 * @param[in] r          Radians to rotate
 * @param[in] bRightSide Whether to transform from the right side
 * @return Rotated Quaternion
 */
C3D_FQuat Quat_RotateZ(C3D_FQuat q, float r, bool bRightSide);

/**
 * @brief Get Quaternion equivalent to 4x4 matrix
 * @note If the matrix is orthogonal or special orthogonal, where determinant(matrix) = +1.0f, then the matrix can be converted.
 * @param[in]   m Input  Matrix
 * @return      Generated Quaternion
 */
C3D_FQuat Quat_FromMtx(const C3D_Mtx* m);

/**
 * @brief Identity Quaternion
 * @return Identity Quaternion
 */
static inline C3D_FQuat Quat_Identity(void)
{
	// r=1, i=j=k=0
	return Quat_New(0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * @brief Quaternion conjugate
 * @param[in] q Quaternion of which to get conjugate
 * @return q*
 */
static inline C3D_FQuat Quat_Conjugate(C3D_FQuat q)
{
	// q* = q.r - q.i - q.j - q.k
	return Quat_New(-q.i, -q.j, -q.k, q.r);
}

/**
 * @brief Quaternion inverse
 * @note This is equivalent to `Quat_Pow(v, -1)`
 * @param[in] q Quaternion of which to get inverse
 * @return q<sup>-1</sup>
 */
static inline C3D_FQuat Quat_Inverse(C3D_FQuat q)
{
	// q^-1 = (q.r - q.i - q.j - q.k) / (q.r^2 + q.i^2 + q.j^2 + q.k^2)
	//      = q* / (q∙q)
	C3D_FQuat c = Quat_Conjugate(q);
	float     d = Quat_Dot(q, q);
	return Quat_New(c.i/d, c.j/d, c.k/d, c.r/d);
}

/**
 * @brief Cross product of FVec3 and Quaternion
 * @param[in] v Base FVec3
 * @param[in] q Quaternion to cross
 * @return v×q
 */
static inline C3D_FVec FVec3_CrossQuat(C3D_FVec v, C3D_FQuat q)
{
	// v×q = (q^-1)×v
	return Quat_CrossFVec3(Quat_Inverse(q), v);
}

/**
 * @brief Converting Pitch, Yaw, and Roll to Quaternion equivalent
 * @param[in] pitch      The pitch angle in radians.
 * @param[in] yaw        The yaw angle in radians.
 * @param[in] roll       The roll angle in radians.
 * @param[in] bRightSide Whether to transform from the right side
 * @return    C3D_FQuat  The Quaternion equivalent with the pitch, yaw, and roll (in that order) orientations applied.
 */
C3D_FQuat Quat_FromPitchYawRoll(float pitch, float yaw, float roll, bool bRightSide);

/**
 * @brief Quaternion Look-At
 * @param[in] source   C3D_FVec Starting position. Origin of rotation.
 * @param[in] target   C3D_FVec Target position to orient towards.
 * @param[in] forwardVector C3D_FVec The Up vector.
 * @param[in] upVector C3D_FVec The Up vector.
 * @return Quaternion rotation.
 */
C3D_FQuat Quat_LookAt(C3D_FVec source, C3D_FVec target, C3D_FVec forwardVector, C3D_FVec upVector);

/**
 * @brief Quaternion, created from a given axis and angle in radians.
 * @param[in] axis  C3D_FVec The axis to rotate around at.
 * @param[in] angle float The angle to rotate. Unit: Radians
 * @return Quaternion rotation based on the axis and angle. Axis doesn't have to be orthogonal.
 */
C3D_FQuat Quat_FromAxisAngle(C3D_FVec axis, float angle);
/** @} */
/** @} */
