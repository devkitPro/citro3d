#pragma once
#include "framebuffer.h"

typedef struct C3D_RenderTarget_tag C3D_RenderTarget;

struct C3D_RenderTarget_tag
{
	C3D_RenderTarget *next, *prev;
	C3D_FrameBuf frameBuf;

	bool used;
	bool ownsColor, ownsDepth;

	bool linked;
	gfxScreen_t screen;
	gfx3dSide_t side;
	u32 transferFlags;
};

// Flags for C3D_FrameBegin
enum
{
	C3D_FRAME_SYNCDRAW = BIT(0), // Perform C3D_FrameSync before checking the GPU status
	C3D_FRAME_NONBLOCK = BIT(1), // Return false instead of waiting if the GPU is busy
};

float C3D_FrameRate(float fps);
void C3D_FrameSync(void);
u32 C3D_FrameCounter(int id);

bool C3D_FrameBegin(u8 flags);
bool C3D_FrameDrawOn(C3D_RenderTarget* target);
void C3D_FrameSplit(u8 flags);
void C3D_FrameEnd(u8 flags);

void C3D_FrameEndHook(void (* hook)(void*), void* param);

float C3D_GetDrawingTime(void);
float C3D_GetProcessingTime(void);

#if defined(__GNUC__) && !defined(__cplusplus)
typedef union __attribute__((__transparent_union__))
{
	int __i;
	GPU_DEPTHBUF __e;
} C3D_DEPTHTYPE;
#else
union C3D_DEPTHTYPE
{
private:
	int __i;
	GPU_DEPTHBUF __e;
public:
	C3D_DEPTHTYPE(GPU_DEPTHBUF e) : __e(e) {}
	C3D_DEPTHTYPE(int i) : __i(-1) { (void)i; }
};
#endif

#define C3D_DEPTHTYPE_OK(_x)  ((_x).__i >= 0)
#define C3D_DEPTHTYPE_VAL(_x) ((_x).__e)

C3D_RenderTarget* C3D_RenderTargetCreate(int width, int height, GPU_COLORBUF colorFmt, C3D_DEPTHTYPE depthFmt);
C3D_RenderTarget* C3D_RenderTargetCreateFromTex(C3D_Tex* tex, GPU_TEXFACE face, int level, C3D_DEPTHTYPE depthFmt);
void C3D_RenderTargetDelete(C3D_RenderTarget* target);
void C3D_RenderTargetSetOutput(C3D_RenderTarget* target, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags);

static inline void C3D_RenderTargetClear(C3D_RenderTarget* target, C3D_ClearBits clearBits, u32 clearColor, u32 clearDepth)
{
	C3D_FrameBufClear(&target->frameBuf, clearBits, clearColor, clearDepth);
}

void C3D_SyncDisplayTransfer(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 flags);
void C3D_SyncTextureCopy(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 size, u32 flags);
void C3D_SyncMemoryFill(u32* buf0a, u32 buf0v, u32* buf0e, u16 control0, u32* buf1a, u32 buf1v, u32* buf1e, u16 control1);
