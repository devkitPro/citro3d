/**
 * @file effect.h
 * @brief Configure GPU state
 */
#pragma once
#include "types.h"

/**
 * @brief Specify mapping of depth values from normalized device coordinates to window coordinates.
 * @param[in] bIsZBuffer Enables or disables depth range. The initial value is true.
 * @param[in] zScale     Specifies mapping of depth values from normalized device coordinates to window coordinates (nearVal - farVal). The initial value is -1.0f.
 * @param[in] zOffset    Sets the scale and units used to calculate depth values (nearVal + polygonOffset). The initial value is 0.0f.
 */
void C3D_DepthMap(bool bIsZBuffer, float zScale, float zOffset);

/**
 * @brief Specify whether front-facing or back-facing facets can be culled.
 * @param[in] mode Specifies whether front-facing, back-facing, or no facets are candidates for culling. The inital value is \ref GPU_CULL_BACK_CCW.
 */
void C3D_CullFace(GPU_CULLMODE mode);

/**
 * @brief Set front and back function and reference value for stencil testing
 * @param[in] enable    Enables or disables stencil test. The initial value is false.
 * @param[in] function  Specifies the test function. The initial value is \ref GPU_ALWAYS.
 * @param[in] ref       Specifies the reference value for the stencil test. ref is clamped to the range 02^n - 1 , where n is the number of bitplanes in the stencil buffer. The initial value is 0.
 * @param[in] inputMask Specifies a mask that is ANDed with both the reference value and the stored stencil value when the test is done. The initial value is all 1's.
 * @param[in] writeMask Specifies a bit mask to enable and disable writing of individual bits in the stencil planes. Initially, the mask is all 0's.
 */
void C3D_StencilTest(bool enable, GPU_TESTFUNC function, int ref, int inputMask, int writeMask);

/**
 * @brief Set front and back stencil test actions
 * @param[in] sfail Specifies the action to take when the stencil test fails. The initial value is \ref GPU_STENCIL_KEEP.
 * @param[in] dfail Specifies the stencil action when the stencil test passes, but the depth test fails. The initial value is \ref GPU_STENCIL_KEEP.
 * @param[in] pass  Specifies the stencil action when both the stencil test and the depth test pass, or when the stencil test passes and either there is no depth buffer or depth testing is not enabled. The initial value is \ref GPU_STENCIL_KEEP.
 */
void C3D_StencilOp(GPU_STENCILOP sfail, GPU_STENCILOP dfail, GPU_STENCILOP pass);

/**
 * @brief Set the blend color
 * @param[in] color Specifies the RGBA blend color. The initial value is 0.
 */
void C3D_BlendingColor(u32 color);

void C3D_EarlyDepthTest(bool enable, GPU_EARLYDEPTHFUNC function, u32 ref);

/**
 * Configure depth testing options.
 * @note Setting the enable parameter to false will not also disable depth writes. It will instead behave as if the depth function were set to \ref GPU_ALWAYS. To completely disable depth-related operations, the enable parameter must be false, and the writemask should be \ref GPU_WRITE_COLOR.
 * @param[in] enable    If true, do depth comparisons on the outgoing fragments and write to the depth buffer. The initial value is true.
 * @param[in] function  Specifies the depth comparison function. The initial value is \ref GPU_GREATER.
 * @param[in] writemask Configures buffer writemasks for the depth test stage. The initial value is \ref GPU_WRITE_ALL.
 */
void C3D_DepthTest(bool enable, GPU_TESTFUNC function, GPU_WRITEMASK writemask);

/**
 * @brief Configure Alpha testing
 * @param[in] enable   Enables or disables alpha test. The initial value is false.
 * @param[in] function Specifies the alpha comparison function. The initial value is \ref GPU_ALWAYS.
 * @param[in] ref      Specifies the reference value that incoming alpha values are compared to from 0 to 0xFF. The intial value is 0.
 */
void C3D_AlphaTest(bool enable, GPU_TESTFUNC function, int ref);

/**
 * @brief Configure blend functions
 * @param[in] colorEq  Specifies how source and destination colors are combined. The initial value is \ref GPU_BLEND_ADD.
 * @param[in] alphaEq  Specifies how source and destination alphas are combined. The initial value is \ref GPU_BLEND_ADD.
 * @param[in] srcClr   Specifies how the red, green, and blue source blending factors are computed. The initial value is \ref GPU_SRC_ALPHA.
 * @param[in] dstClr   Specifies how the red, green, and blue destination blending factors are computed. The initial value is \ref GPU_ONE_MINUS_SRC_ALPHA.
 * @param[in] srcAlpha Specifies how the alpha source blending factors are computed. The initial value is \ref GPU_SRC_ALPHA.
 * @param[in] dstAlpha Specifies how the alpha destination blending factors are computed. The initial value is \ref GPU_ONE_MINUS_SRC_ALPHA.
 */
void C3D_AlphaBlend(GPU_BLENDEQUATION colorEq, GPU_BLENDEQUATION alphaEq, GPU_BLENDFACTOR srcClr, GPU_BLENDFACTOR dstClr, GPU_BLENDFACTOR srcAlpha, GPU_BLENDFACTOR dstAlpha);

void C3D_ColorLogicOp(GPU_LOGICOP op);

void C3D_FragOpMode(GPU_FRAGOPMODE mode);

void C3D_FragOpShadow(float scale, float bias);
