/**
 * @file texenv.h
 * @brief Configure texture combiner stages (TexEnv)
 * @see https://www.khronos.org/opengl/wiki/Texture_Combiners
 * @see https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glTexEnv.xml
 */
#pragma once
#include "types.h"

/// TexEnv stage configuration
typedef struct
{
	u16 srcRgb, srcAlpha;
	union
	{
		u32 opAll;
		struct { u32 opRgb:12, opAlpha:12; };
	};
	u16 funcRgb, funcAlpha;
	u32 color;
	u16 scaleRgb, scaleAlpha;
} C3D_TexEnv;

/// TexEnv operation mode
typedef enum
{
	C3D_RGB = BIT(0),               ///< RGB mode
	C3D_Alpha = BIT(1),             ///< Alpha mode
	C3D_Both = C3D_RGB | C3D_Alpha, ///< Both
} C3D_TexEnvMode;

/**
 * @brief Gets the global TexEnv for a given stage.
 * @param[in] id TexEnv stage between 0-5 to return.
 * @return TexEnv of the given stage.
 */
C3D_TexEnv* C3D_GetTexEnv(int id);

/**
 * @brief Sets the global TexEnv for a given stage.
 * @param[in] id  TexEnv stage between 0-5 to set.
 * @param[in] env Pointer to user TexEnv.
 */
void C3D_SetTexEnv(int id, C3D_TexEnv* env);

/**
 * @brief Marks a TexEnv as needing to be updated.
 * @note One must use this if they are continuing to use a TexEnv pointer they got from \ref C3D_GetTexEnv() after performing an action that flushes state.
 * @param[in] env Pointer to a TexEnv struct.
 */
void C3D_DirtyTexEnv(C3D_TexEnv* env);

/**
 * @brief Configures the stages where the GPU_PREVIOUS_BUFFER source value should be updated with the output of that stage.
 * @param[in] mode TexEnv update modes (see \ref C3D_TexEnvMode)
 * @param[in] mask Bitmask containing which stages update GPU_PREVIOUS_BUFFER (bitmask can be created using \ref GPU_TEV_BUFFER_WRITE_CONFIG())
 */
void C3D_TexEnvBufUpdate(int mode, int mask);

/**
 * @brief Configure the initial value of GPU_PREVIOUS_BUFFER. This value will be kept until it is updated; see \ref C3D_TexEnvBufUpdate().
 * @param[in] color Color value.
 */
void C3D_TexEnvBufColor(u32 color);

/**
 * @brief Resets a TexEnv to its default values.
 * @param[out] env TexEnv to initialize.
 */
static inline void C3D_TexEnvInit(C3D_TexEnv* env)
{
	env->srcRgb     = GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0);
	env->srcAlpha   = env->srcRgb;
	env->opAll      = 0;
	env->funcRgb    = GPU_REPLACE;
	env->funcAlpha  = env->funcRgb;
	env->color      = 0xFFFFFFFF;
	env->scaleRgb   = GPU_TEVSCALE_1;
	env->scaleAlpha = GPU_TEVSCALE_1;
}

#ifdef __cplusplus
#define _C3D_DEFAULT(x) = x
#else
#define _C3D_DEFAULT(x)
#endif

/**
 * @brief Sets the input source of a TexEnv.
 * @param[out] env  Pointer to TexEnv struct.
 * @param[in]  mode TexEnv update modes (see \ref C3D_TexEnvMode)
 * @param[in]  s1   First source.
 * @param[in]  s2   Second source.
 * @param[in]  s3   Third source.
 */
static inline void C3D_TexEnvSrc(C3D_TexEnv* env, C3D_TexEnvMode mode,
	GPU_TEVSRC s1,
	GPU_TEVSRC s2 _C3D_DEFAULT(GPU_PRIMARY_COLOR),
	GPU_TEVSRC s3 _C3D_DEFAULT(GPU_PRIMARY_COLOR))
{
	int param = GPU_TEVSOURCES((int)s1, (int)s2, (int)s3);
	if ((int)mode & C3D_RGB)
		env->srcRgb = param;
	if ((int)mode & C3D_Alpha)
		env->srcAlpha = param;
}

/**
 * @brief Configures the operation to be applied to the input color of a TexEnv before the function is applied.
 * @param[out] env Pointer to TexEnv struct.
 * @param[in]  o1  Operation to perform on the first source.
 * @param[in]  o2  Operation to perform on the second source.
 * @param[in]  o3  Operation to perform on the third source.
 */
static inline void C3D_TexEnvOpRgb(C3D_TexEnv* env,
	GPU_TEVOP_RGB o1,
	GPU_TEVOP_RGB o2 _C3D_DEFAULT(GPU_TEVOP_RGB_SRC_COLOR),
	GPU_TEVOP_RGB o3 _C3D_DEFAULT(GPU_TEVOP_RGB_SRC_COLOR))
{
	env->opRgb = GPU_TEVOPERANDS((int)o1, (int)o2, (int)o3);
}

/**
 * @brief Configures the operation to be applied to the input alpha of a TexEnv before the function is applied.
 * @param[out] env Pointer to TexEnv struct.
 * @param[in]  o1  Operation to perform on the first source.
 * @param[in]  o2  Operation to perform on the second source.
 * @param[in]  o3  Operation to perform on the third source.
 */
static inline void C3D_TexEnvOpAlpha(C3D_TexEnv* env,
	GPU_TEVOP_A o1,
	GPU_TEVOP_A o2 _C3D_DEFAULT(GPU_TEVOP_A_SRC_ALPHA),
	GPU_TEVOP_A o3 _C3D_DEFAULT(GPU_TEVOP_A_SRC_ALPHA))
{
	env->opAlpha = GPU_TEVOPERANDS((int)o1, (int)o2, (int)o3);
}

/**
 * @brief Sets the combiner function to perform in this TexEnv.
 * @param[out] env   Pointer to TexEnv struct.
 * @param[in]  mode  TexEnv update modes (see \ref C3D_TexEnvMode)
 * @param[in]  param Function to use.
 */
static inline void C3D_TexEnvFunc(C3D_TexEnv* env, C3D_TexEnvMode mode, GPU_COMBINEFUNC param)
{
	if ((int)mode & C3D_RGB)
		env->funcRgb = param;
	if ((int)mode & C3D_Alpha)
		env->funcAlpha = param;
}

/**
 * @brief Sets the value of the GPU_CONSTANT source for a TexEnv stage.
 * @param[out] env   Pointer to TexEnv struct.
 * @param[in]  color RGBA color value to apply.
 */
static inline void C3D_TexEnvColor(C3D_TexEnv* env, u32 color)
{
	env->color = color;
}

/**
 * @brief Configures the scaling to be applied to the output of a TexEnv.
 * @param[out] env   Pointer to TexEnv struct.
 * @param[in]  mode  TexEnv update modes (see \ref C3D_TexEnvMode)
 * @param[in]  param Scale factor to apply.
 */
static inline void C3D_TexEnvScale(C3D_TexEnv* env, int mode, GPU_TEVSCALE param)
{
	if (mode & C3D_RGB)
		env->scaleRgb = param;
	if (mode & C3D_Alpha)
		env->scaleAlpha = param;
}

#undef _C3D_DEFAULT
