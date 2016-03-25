#pragma once
#include "renderbuffer.h"

typedef struct C3D_RenderTarget_tag C3D_RenderTarget;

struct C3D_RenderTarget_tag
{
	C3D_RenderTarget *next, *prev, *link, *frame[2];
	C3D_RenderBuf renderBuf;
	u32 transferFlags;

	u8 clearBits;
	bool drawOk, transferOk;

	bool linked;
	gfxScreen_t screen;
	gfx3dSide_t side;
};

// Flags for C3D_FrameBegin
enum
{
	C3D_FRAME_SYNCDRAW = BIT(0), // Do not render the frame until the previous has finished rendering
	C3D_FRAME_NONBLOCK = BIT(1), // Return false instead of waiting for the GPU to finish rendering
};

bool C3D_FrameBegin(u8 flags);
bool C3D_FrameDrawOn(C3D_RenderTarget* target);
void C3D_FrameEnd(u8 flags);

// Flags for C3D_RenderTargetSetClear (only C3D_CLEAR_ALL implemented atm)
enum
{
	C3D_CLEAR_COLOR = BIT(0),
	C3D_CLEAR_DEPTH = BIT(1),
	C3D_CLEAR_ALL   = C3D_CLEAR_COLOR | C3D_CLEAR_DEPTH,
};

C3D_RenderTarget* C3D_RenderTargetCreate(int width, int height, int colorFmt, int depthFmt);
void C3D_RenderTargetDelete(C3D_RenderTarget* target);
void C3D_RenderTargetSetClear(C3D_RenderTarget* target, u32 clearBits, u32 clearColor, u32 clearDepth);
void C3D_RenderTargetSetOutput(C3D_RenderTarget* target, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags);

void C3D_SafeDisplayTransfer(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 flags);
void C3D_SafeTextureCopy(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 size, u32 flags);
void C3D_SafeMemoryFill(u32* buf0a, u32 buf0v, u32* buf0e, u16 control0, u32* buf1a, u32 buf1v, u32* buf1e, u16 control1);
