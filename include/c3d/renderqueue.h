#pragma once
#include "framebuffer.h"

// Flags for C3D_RenderTargetSetClear
typedef enum
{
	C3D_CLEAR_COLOR = BIT(0),
	C3D_CLEAR_DEPTH = BIT(1),
	C3D_CLEAR_ALL   = C3D_CLEAR_COLOR | C3D_CLEAR_DEPTH,
} C3D_ClearBits;

typedef struct C3D_RenderTarget_tag C3D_RenderTarget;

struct C3D_RenderTarget_tag
{
	C3D_RenderTarget *next, *prev, *link, *frame[2];
	C3D_FrameBuf frameBuf;

	u32 transferFlags;
	u32 clearColor, clearDepth;
	C3D_ClearBits clearBits;
	bool ownsColor, ownsDepth;

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

float C3D_FrameRate(float fps);
bool C3D_FrameBegin(u8 flags);
bool C3D_FrameDrawOn(C3D_RenderTarget* target);
void C3D_FrameEnd(u8 flags);

float C3D_GetDrawingTime(void);
float C3D_GetProcessingTime(void);

#if defined(__GNUC__) && !defined(__cplusplus)
typedef union __attribute__((__transparent_union__))
{
	int __i;
	GPU_DEPTHBUF __e;
} C3D_DEPTHTYPE;
#define C3D_DEPTHTYPE_OK(_x)  ((_x).__i >= 0)
#define C3D_DEPTHTYPE_VAL(_x) ((_x).__e)
#else
typedef int C3D_DEPTHTYPE;
#define C3D_DEPTHTYPE_OK(_x)  ((_x) >= 0)
#define C3D_DEPTHTYPE_VAL(_x) ((GPU_DEPTHBUF)(_x))
#endif

C3D_RenderTarget* C3D_RenderTargetCreate(int width, int height, GPU_COLORBUF colorFmt, C3D_DEPTHTYPE depthFmt);
C3D_RenderTarget* C3D_RenderTargetCreateFromTex(C3D_Tex* tex, GPU_TEXFACE face, int level, C3D_DEPTHTYPE depthFmt);
void C3D_RenderTargetDelete(C3D_RenderTarget* target);
void C3D_RenderTargetSetClear(C3D_RenderTarget* target, C3D_ClearBits clearBits, u32 clearColor, u32 clearDepth);
void C3D_RenderTargetSetOutput(C3D_RenderTarget* target, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags);

void C3D_SafeDisplayTransfer(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 flags);
void C3D_SafeTextureCopy(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 size, u32 flags);
void C3D_SafeMemoryFill(u32* buf0a, u32 buf0v, u32* buf0e, u16 control0, u32* buf1a, u32 buf1v, u32* buf1e, u16 control1);
