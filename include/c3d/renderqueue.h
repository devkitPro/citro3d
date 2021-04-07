/**
 * @file renderqueue.h
 * @brief Set up rendertarget and render frame
 */
#pragma once
#include "framebuffer.h"

/// Render target structure
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

/// Flags for C3D_FrameBegin
enum
{
	C3D_FRAME_SYNCDRAW = BIT(0), ///< Perform \ref C3D_FrameSync() before checking the GPU status
	C3D_FRAME_NONBLOCK = BIT(1), ///< Return false instead of waiting if the GPU is busy
};

/**
 * @brief Specifies framerate cap.
 * Specifies target framerate for \ref C3D_FrameSync().
 * @param[in] fps Specifies the target framerate. Must be between 0.0 and 60.0 fps.
 * @return Previous framerate.
 */
float C3D_FrameRate(float fps);

/**
 * @brief Performs framerate limiting.
 * Waits for the required amount of vblanks specified by \ref C3D_FrameRate().
 * @note Used by \ref C3D_FrameBegin() when using \ref C3D_FRAME_SYNCDRAW.
 */
void C3D_FrameSync(void);

/**
 * @brief Returns total number of frames drawn.
 * @param[in] id Vblank frame counter id.
 * @return Total number of frames.
 */
u32 C3D_FrameCounter(int id);

/**
 * @brief Begins drawing frame.
 * @param[in] flags Specifies options for rendering; 0 or more flags may be provided.
 *       \ref C3D_FRAME_SYNCDRAW specifies that \ref C3D_FrameSync() should be performed before checking the GPU status,
 *       \ref C3D_FRAME_NONBLOCK specifies that the function should return false instead of waiting for GPU to be ready.
 * @return True if frame began successfully, otherwise false.
 */
bool C3D_FrameBegin(u8 flags);

/**
 * @brief Specifies render target to draw frame to.
 * @param[in] target Pointer to render target.
 * @return True if rendertarget was set successfully, otherwise false.
 */
bool C3D_FrameDrawOn(C3D_RenderTarget* target);

/**
 * @brief Splits and submits the GPU cmdlist in the middle of a renderqueue frame.
 * @param[in] flags Specifies 0 or more GX_CMDLIST flags.
 */
void C3D_FrameSplit(u8 flags);

/**
 * @brief Ends drawing frame.
 * @param[in] flags Specifies 0 or more GX_CMDLIST flags.
 */
void C3D_FrameEnd(u8 flags);

/**
 * @brief Executes callback upon \ref C3D_FrameEnd().
 * @param[in] hook  Function callback.
 * @param[in] param User data.
 */
void C3D_FrameEndHook(void (* hook)(void*), void* param);

/**
 * @brief Gets time spent by the GPU during last render.
 * @return Drawing time in milliseconds.
 */
float C3D_GetDrawingTime(void);

/**
 * @brief Gets time elapsed between last \ref C3D_FrameBegin() and \ref C3D_FrameEnd().
 * @return Time in milliseconds.
 */
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

/**
 * @brief Creates a new render target.
 * @note When creating a rendertarget intended for display, keep in mind the orientation of the screens.
 *  When you hold a 3DS normally, the screens are rotated 90 degrees counter-clockwise.
 * @param[in] width    Specifies width of the render target in pixels.
 * @param[in] height   Specifies height of the render target in pixels.
 * @param[in] colorFmt Specifies the color format of the render target.
 * @param[in] depthFmt Specifies the depth format of the render target using \ref GPU_DEPTHBUF. (-1 for no depth type)
 * @return Pointer to newly created render target.
 */
C3D_RenderTarget* C3D_RenderTargetCreate(int width, int height, GPU_COLORBUF colorFmt, C3D_DEPTHTYPE depthFmt);

/**
 * @brief Constructs render target for texture.
 * @param[in] tex      Pointer to \ref C3D_Tex.
 * @param[in] face     Specifies face of cubemap to be used. (GPU_TEXFACE_2D if not cubemap)
 * @param[in] level    Specifies mipmap level to use.
 * @param[in] depthFmt Specifies the depth format of the render target using \ref GPU_DEPTHBUF. (-1 for no depth type)
 * @return Pointer to newly created render target.
 */
C3D_RenderTarget* C3D_RenderTargetCreateFromTex(C3D_Tex* tex, GPU_TEXFACE face, int level, C3D_DEPTHTYPE depthFmt);

/**
 * @brief Deletes render target.
 * @param[in] target Pointer to render target.
 */
void C3D_RenderTargetDelete(C3D_RenderTarget* target);

/**
 * @brief Sets render target output to screen.
 * @param[in] target        Pointer to render target.
 * @param[in] screen        Screen to transfer the framebuffer to.
 * @param[in] side          Side of the screen to transfer the framebuffer to. (unused for the bottom screen)
 * @param[in] transferFlags Specifies GX_TRANSFER bitflags.
 */
void C3D_RenderTargetSetOutput(C3D_RenderTarget* target, gfxScreen_t screen, gfx3dSide_t side, u32 transferFlags);

/**
 * @brief Detaches render target from screen output.
 * @param[in] target Pointer to render target.
 */
static inline void C3D_RenderTargetDetachOutput(C3D_RenderTarget* target)
{
	C3D_RenderTargetSetOutput(NULL, target->screen, target->side, 0);
}

/**
 * @brief Clears framebuffer of target.
 * @param[in] target     Pointer to render target.
 * @param[in] clearBits  Specifies which buffers to clear. (see \ref C3D_ClearBits)
 * @param[in] clearColor 32 bit RGBA value to clear color buffer with.
 * @param[in] clearDepth Value to clear depth buffer with.
 */
static inline void C3D_RenderTargetClear(C3D_RenderTarget* target, C3D_ClearBits clearBits, u32 clearColor, u32 clearDepth)
{
	C3D_FrameBufClear(&target->frameBuf, clearBits, clearColor, clearDepth);
}

/**
 * @brief Synchronizes and initiates a display transfer.
 * Synchronizes and initiates a \ref GX_DisplayTransfer().
 * @param[in]  inadr  Address of the input.
 * @param[in]  indim  \ref GX_BUFFER_DIM() of the input.
 * @param[out] outadr Address of the output.
 * @param[in]  outdim \ref GX_BUFFER_DIM() of the output.
 * @param[in]  flags  Flags to transfer with.
 */
void C3D_SyncDisplayTransfer(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 flags);

/**
 * @brief Synchronizes and initiates a texture copy.
 * Synchronizes and initiates a \ref GX_TextureCopy().
 * @param[in]  inadr  Address of the input.
 * @param[in]  indim  \ref GX_BUFFER_DIM() of the input.
 * @param[out] outadr Address of the output.
 * @param[in]  outdim \ref GX_BUFFER_DIM() of the output.
 * @param[in]  size   Size of the data to transfer.
 * @param[in]  flags  Flags to transfer with.
 */
void C3D_SyncTextureCopy(u32* inadr, u32 indim, u32* outadr, u32 outdim, u32 size, u32 flags);

/**
 * @brief Synchronizes and fills the memory of two buffers with the given values.
 * Synchronizes and initiates a \ref GX_MemoryFill().
 * @param[in] buf0a    Start address of the first buffer.
 * @param[in] buf0v    Value to fill first buffer.
 * @param[in] buf0e    End address of the first buffer.
 * @param[in] control0 Value to fill the first buffer with.
 * @param[in] buf1a    Start address of the second buffer.
 * @param[in] buf1v    Value to fill second buffer.
 * @param[in] buf1e    End address of the second buffer.
 * @param[in] control1 Value to fill the second buffer with.
 */
void C3D_SyncMemoryFill(u32* buf0a, u32 buf0v, u32* buf0e, u16 control0, u32* buf1a, u32 buf1v, u32* buf1e, u16 control1);
