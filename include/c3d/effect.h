#pragma once
#include "types.h"

void C3D_DepthMap(float zScale, float zOffset);
void C3D_CullFace(GPU_CULLMODE mode);
void C3D_StencilTest(bool enable, GPU_TESTFUNC function, int ref, int inputMask, int writeMask);
void C3D_StencilOp(GPU_STENCILOP sfail, GPU_STENCILOP dfail, GPU_STENCILOP pass);
void C3D_BlendingColor(u32 color);
void C3D_DepthTest(bool enable, GPU_TESTFUNC function, GPU_WRITEMASK writemask);
void C3D_AlphaTest(bool enable, GPU_TESTFUNC function, int ref);
void C3D_AlphaBlend(GPU_BLENDEQUATION colorEq, GPU_BLENDEQUATION alphaEq, GPU_BLENDFACTOR srcClr, GPU_BLENDFACTOR dstClr, GPU_BLENDFACTOR srcAlpha, GPU_BLENDFACTOR dstAlpha);
void C3D_ColorLogicOp(GPU_LOGICOP op);
void C3D_FragOpMode(GPU_FRAGOPMODE mode);
