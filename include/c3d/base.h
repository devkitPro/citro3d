/**
 * @file base.h
 * @brief Base citro3d functions
 */
#pragma once
#include "buffers.h"
#include "maths.h"

/// Default command buffer size
#define C3D_DEFAULT_CMDBUF_SIZE 0x40000

/**
 * @brief Data type of indices for use with \ref C3D_DrawElements().
 */
enum
{
	C3D_UNSIGNED_BYTE = 0,  ///< Unsigned 8-bit integer
	C3D_UNSIGNED_SHORT = 1, ///< Unsigned 16-bit integer
};

/**
 * @brief Initializes citro3d.
 * @param[in] cmdBufSize Desired size of GPU command buffer. \ref C3D_DEFAULT_CMDBUF_SIZE should generally be used here.
 * @note If you have a particularly complex scene you might need to specify a larger \p cmdBufSize.
 *       Conversely, you may want to decrease it if you're particularly concerned about memory consumption.
 * @return true if library was initialized successfully, false if there was an error.
 */
bool C3D_Init(size_t cmdBufSize);

/**
 * @brief Deinitializes citro3d
 * @sa C3D_Init()
 */
void C3D_Fini(void);

/**
 * @brief Retrieves the current command buffer usage.
 * @return Fraction of command buffer used. (0.0f to 1.0f)
 */
float C3D_GetCmdBufUsage(void);

/**
 * @brief Binds a shader program to the current rendering state.
 * @param[in] program Specifies the pointer to a shader program object whose executables are to be used
 *       as part of the current rendering state.
 */
void C3D_BindProgram(shaderProgram_s* program);

/**
 * @brief Sets the viewport for the current framebuffer.
 * @note This function is called by \ref C3D_FrameDrawOn(). (using values specified by \ref C3D_RenderTargetCreate())
 * @note When using this with a rendertarget intended for display, keep in mind the orientation of the screens.
 * @param[in] x X offset from the origin of the viewport in pixels.
 * @param[in] y Y offset from the origin of the viewport in pixels.
 * @param[in] w Width of the viewport in pixels.
 * @param[in] h Height of the viewport in pixels.
 */
void C3D_SetViewport(u32 x, u32 y, u32 w, u32 h);

/**
 * @brief Defines the scissor box.
 * @note When using this with a rendertarget intended for display, keep in mind the orientation of the screens.
 * @param[in] mode   Specifies scissoring mode.
 * @param[in] left   Leftmost boundary in pixels.
 * @param[in] top    Topmost boundary in pixels.
 * @param[in] right  Rightmost boundary in pixels.
 * @param[in] bottom Bottommost boundary in pixels.
 */
void C3D_SetScissor(GPU_SCISSORMODE mode, u32 left, u32 top, u32 right, u32 bottom);

/**
 * @brief Renders primitives from current vertex array buffer.
 * @param[in] primitive Specifies what kind of primitives to render.
 * @param[in] first     Specifies the starting index in the current buffers.
 * @param[in] size      Specifies the number of indices to be rendered.
 */
void C3D_DrawArrays(GPU_Primitive_t primitive, int first, int size);

/**
 * @brief Renders primitives from current vertex array buffer in a manually specified order.
 * @param[in] primitive Specifies what kind of primitives to render.
 * @param[in] count     Specifies the number of indices to be rendered.
 * @param[in] type      Specifies the data type of the indices.
 *       May be \ref C3D_UNSIGNED_BYTE or \ref C3D_UNSIGNED_SHORT.
 * @param[in] indices   Specifies a pointer to where the indices are stored.
 */
void C3D_DrawElements(GPU_Primitive_t primitive, int count, int type, const void* indices);

/**
 * @name Immediate-mode vertex submission
 * @{
 */

/**
 * @brief Delimits the vertices of a primitive or a group of like primitives.
 * @param[in] primitive Specifies type of primitive or primitives that will be created
 *       using the vertices specified between \ref C3D_ImmDrawBegin() and \ref C3D_ImmDrawEnd().
 */
void C3D_ImmDrawBegin(GPU_Primitive_t primitive);

/**
 * @brief Specifies an immediate attribute.
 * @note Attributes must be specified in the same order they were specified using \ref AttrInfo_AddLoader().
 * @param[in] x Specifies the X value of the current attribute.
 * @param[in] y Specifies the Y value of the current attribute.
 * @param[in] z Specifies the Z value of the current attribute.
 * @param[in] w Specifies the W value of the current attribute.
 */
void C3D_ImmSendAttrib(float x, float y, float z, float w);

/**
 * @brief Delimits the vertices of a primitive or a group of like primitives.
 * @sa C3D_ImmDrawBegin()
 */
void C3D_ImmDrawEnd(void);

/**
 * @brief Specifies the end of the previous strip/fan and the beginning of a new one.
 * @sa C3D_ImmDrawBegin()
 */
static inline void C3D_ImmDrawRestartPrim(void)
{
	GPUCMD_AddWrite(GPUREG_RESTART_PRIMITIVE, 1);
}
/** @} */

/**
 * @name Fixed vertex attributes
 * @{
 */


/**
 * @brief Gets the pointer to the fixed attribute vector for the specified attribute index.
 * @param[in] id Attribute index.
 * @return Pointer to the fixed attribute vector for the current attribute.
 * @sa C3D_FixedAttribSet()
 */
C3D_FVec* C3D_FixedAttribGetWritePtr(int id);

/**
 * @brief Sets fixed attribute vector for the specified attribute index.
 * @note The attribute index should be the same as returned by \ref AttrInfo_AddFixed().
 * @param[in] id Attribute index.
 * @param[in] x  Specifies the X value of the attribute.
 * @param[in] y  Specifies the Y value of the attribute.
 * @param[in] z  Specifies the Z value of the attribute.
 * @param[in] w  Specifies the W value of the attribute.
 */
static inline void C3D_FixedAttribSet(int id, float x, float y, float z, float w)
{
	C3D_FVec* ptr = C3D_FixedAttribGetWritePtr(id);
	ptr->x = x;
	ptr->y = y;
	ptr->z = z;
	ptr->w = w;
}
/** @} */
