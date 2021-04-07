/**
 * @file framebuffer.h
 * @brief Process render target framebuffer
 */
#pragma once
#include "texture.h"

typedef struct
{
	void* colorBuf;
	void* depthBuf;
	u16 width;
	u16 height;
	GPU_COLORBUF colorFmt;
	GPU_DEPTHBUF depthFmt;
	bool block32;
	u8 colorMask : 4;
	u8 depthMask : 4;
} C3D_FrameBuf;

/// Flags for C3D_FrameBufClear
typedef enum
{
	C3D_CLEAR_COLOR = BIT(0),                            ///< Clear the color buffer.
	C3D_CLEAR_DEPTH = BIT(1),                            ///< Clear the the depth/stencil buffer.
	C3D_CLEAR_ALL   = C3D_CLEAR_COLOR | C3D_CLEAR_DEPTH, ///< Clear both buffers
} C3D_ClearBits;

/**
 * @brief Calculates the size of a color buffer.
 * @param[in] width  Width of the color buffer in pixels.
 * @param[in] height Height of the color buffer in pixels.
 * @param[in] fmt    Format of the color buffer.
 * @return Calculated color buffer size.
 */
u32 C3D_CalcColorBufSize(u32 width, u32 height, GPU_COLORBUF fmt);

/**
 * @brief Calculates the size of a depth buffer.
 * @param[in] width  Width of the depth buffer in pixels.
 * @param[in] height Height of the depth buffer in pixels.
 * @param[in] fmt    Format of the depth buffer.
 * @return Calculated depth buffer size.
 */
u32 C3D_CalcDepthBufSize(u32 width, u32 height, GPU_DEPTHBUF fmt);

/**
 * @brief Returns global citro3d framebuffer structure.
 * @return Pointer to \ref C3D_FrameBuf struct.
 */
C3D_FrameBuf* C3D_GetFrameBuf(void);

/**
 * @brief Sets global citro3d framebuffer structure.
 * @param[in] fb Pointer to \ref C3D_FrameBuf struct.
 */
void C3D_SetFrameBuf(C3D_FrameBuf* fb);

/**
 * @brief Binds a texture to a framebuffer. This texture will be used as the color buffer.
 * @param[out] fb   Pointer to \ref C3D_FrameBuf struct.
 * @param[in] tex   Pointer to \ref C3D_Tex struct.
 * @param[in] face  Specifies face of cubemap to be used (ignored if it is a 2D texture)
 * @param[in] level Specifies mipmap level. 0 is the original image, 1 is the first mipmap, and so on.
 * @remark This calls \ref C3D_FrameBufColor with the proper arguments for the buffer of the texture.
 */
void C3D_FrameBufTex(C3D_FrameBuf* fb, C3D_Tex* tex, GPU_TEXFACE face, int level);

/**
 * @brief Sets the clear bits and color for a framebuffer.
 * @param[in] fb Pointer to a \ref C3D_FrameBuf struct.
 * @param[in] clearBits  Specifies which buffers to clear. (see \ref C3D_ClearBits)
 * @param[in] clearColor 32 bit RGBA value to clear the color buffer with.
 * @param[in] clearDepth Value to clear the depth buffer with.
 */
void C3D_FrameBufClear(C3D_FrameBuf* fb, C3D_ClearBits clearBits, u32 clearColor, u32 clearDepth);

/**
 * @brief Transfers a framebuffer to the LCD display.
 * @param[in] fb            Pointer to a \ref C3D_FrameBuf struct.
 * @param[in] screen        Screen to transfer the framebuffer to.
 * @param[in] side          Side of the screen to transfer the framebuffer to (unused for the bottom screen)
 * @param[in] transferFlags Specifies GX_TRANSFER bitflags.
 */
void C3D_FrameBufTransfer(C3D_FrameBuf* fb, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags);

/**
 * @brief Sets framebuffer attributes.
 * @param[in] fb      Pointer to a \ref C3D_FrameBuf struct.
 * @param[in] width   Width of framebuffer in pixels.
 * @param[in] height  Height of framebuffer in pixels.
 * @param[in] block32 Specifies if using 32x32 tile format.
 */
static inline void C3D_FrameBufAttrib(C3D_FrameBuf* fb, u16 width, u16 height, bool block32)
{
	fb->width   = width;
	fb->height  = height;
	fb->block32 = block32;
}

/**
 * @brief Assigns a color buffer to a framebuffer.
 * @param[in] fb  Pointer to a \ref C3D_FrameBuf struct.
 * @param[in] buf Pointer to the buffer to use.
 * @param[in] fmt Format of the color buffer.
 */
static inline void C3D_FrameBufColor(C3D_FrameBuf* fb, void* buf, GPU_COLORBUF fmt)
{
	if (buf)
	{
		fb->colorBuf  = buf;
		fb->colorFmt  = fmt;
		fb->colorMask = 0xF;
	} else
	{
		fb->colorBuf  = NULL;
		fb->colorFmt  = GPU_RB_RGBA8;
		fb->colorMask = 0;
	}
}

/**
 * @brief Assigns a depth buffer to a framebuffer.
 * @param[in] fb  Pointer to a \ref C3D_FrameBuf struct.
 * @param[in] buf Pointer to the buffer to use.
 * @param[in] fmt Format of the depth buffer.
 * @note Depending on the format chosen, this may be used as a stencil buffer as well.
 */
static inline void C3D_FrameBufDepth(C3D_FrameBuf* fb, void* buf, GPU_DEPTHBUF fmt)
{
	if (buf)
	{
		fb->depthBuf  = buf;
		fb->depthFmt  = fmt;
		fb->depthMask = fmt == GPU_RB_DEPTH24_STENCIL8 ? 0x3 : 0x2;
	} else
	{
		fb->depthBuf  = NULL;
		fb->depthFmt  = GPU_RB_DEPTH24;
		fb->depthMask = 0;
	}
}
