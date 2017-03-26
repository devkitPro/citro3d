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

// Flags for C3D_FrameBufClear
typedef enum
{
	C3D_CLEAR_COLOR = BIT(0),
	C3D_CLEAR_DEPTH = BIT(1),
	C3D_CLEAR_ALL   = C3D_CLEAR_COLOR | C3D_CLEAR_DEPTH,
} C3D_ClearBits;

u32 C3D_CalcColorBufSize(u32 width, u32 height, GPU_COLORBUF fmt);
u32 C3D_CalcDepthBufSize(u32 width, u32 height, GPU_DEPTHBUF fmt);

C3D_FrameBuf* C3D_GetFrameBuf(void);
void C3D_SetFrameBuf(C3D_FrameBuf* fb);
void C3D_FrameBufTex(C3D_FrameBuf* fb, C3D_Tex* tex, GPU_TEXFACE face, int level);
void C3D_FrameBufClear(C3D_FrameBuf* fb, C3D_ClearBits clearBits, u32 clearColor, u32 clearDepth);
void C3D_FrameBufTransfer(C3D_FrameBuf* fb, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags);

static inline void C3D_FrameBufAttrib(C3D_FrameBuf* fb, u16 width, u16 height, bool block32)
{
	fb->width   = width;
	fb->height  = height;
	fb->block32 = block32;
}

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
