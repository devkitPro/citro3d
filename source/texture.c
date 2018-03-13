#include "internal.h"
#include <c3d/renderqueue.h>

// Return bits per pixel
static inline size_t fmtSize(GPU_TEXCOLOR fmt)
{
	switch (fmt)
	{
		case GPU_RGBA8:
			return 32;
		case GPU_RGB8:
			return 24;
		case GPU_RGBA5551:
		case GPU_RGB565:
		case GPU_RGBA4:
		case GPU_LA8:
		case GPU_HILO8:
			return 16;
		case GPU_L8:
		case GPU_A8:
		case GPU_LA4:
		case GPU_ETC1A4:
			return 8;
		case GPU_L4:
		case GPU_A4:
		case GPU_ETC1:
			return 4;
		default:
			return 0;
	}
}

static inline bool addrIsVRAM(const void* addr)
{
	u32 vaddr = (u32)addr;
	return vaddr >= 0x1F000000 && vaddr < 0x1F600000;
}

static inline bool checkTexSize(u32 size)
{
	if (size < 8 || size > 1024)
		return false;
	if (size & (size-1))
		return false;
	return true;
}

static inline void allocFree(void* addr)
{
	if (addrIsVRAM(addr))
		vramFree(addr);
	else
		linearFree(addr);
}

static void C3Di_TexCubeDelete(C3D_TexCube* cube)
{
	int i;
	for (i = 0; i < 6; i ++)
	{
		if (cube->data[i])
		{
			allocFree(cube->data[i]);
			cube->data[i] = NULL;
		}
	}
}

bool C3D_TexInitWithParams(C3D_Tex* tex, C3D_TexCube* cube, C3D_TexInitParams p)
{
	if (!checkTexSize(p.width) || !checkTexSize(p.height)) return false;

	bool isCube = typeIsCube(p.type);
	if (isCube && !cube) return false;

	u32 size = fmtSize(p.format);
	if (!size) return false;
	size *= (u32)p.width * p.height / 8;
	u32 total_size = C3D_TexCalcTotalSize(size, p.maxLevel);

	if (!isCube)
	{
		tex->data = p.onVram ? vramAlloc(total_size) : linearAlloc(total_size);
		if (!tex->data) return false;
	} else
	{
		memset(cube, 0, sizeof(*cube));
		int i;
		for (i = 0; i < 6; i ++)
		{
			cube->data[i] = p.onVram ? vramAlloc(total_size) : linearAlloc(total_size);
			if (!cube->data[i] ||
				(i>0 && (((u32)cube->data[0] ^ (u32)cube->data[i])>>(3+22)))) // Check upper 6bits match with first face
			{
				C3Di_TexCubeDelete(cube);
				return false;
			}
		}
		tex->cube = cube;
	}

	tex->width = p.width;
	tex->height = p.height;
	tex->param = GPU_TEXTURE_MODE(p.type);
	if (p.format == GPU_ETC1)
		tex->param |= GPU_TEXTURE_ETC1_PARAM;
	if (p.type == GPU_TEX_SHADOW_2D || p.type == GPU_TEX_SHADOW_CUBE)
		tex->param |= GPU_TEXTURE_SHADOW_PARAM;
	tex->fmt = p.format;
	tex->size = size;
	tex->border = 0;
	tex->lodBias = 0;
	tex->maxLevel = p.maxLevel;
	tex->minLevel = 0;
	return true;
}

void C3D_TexLoadImage(C3D_Tex* tex, const void* data, GPU_TEXFACE face, int level)
{
	u32 size = 0;
	void* out = C3D_TexGetImagePtr(tex,
		C3Di_TexIs2D(tex) ? tex->data : tex->cube->data[face],
		level, &size);

	if (!addrIsVRAM(out))
		memcpy(out, data, size);
	else
		C3D_SyncTextureCopy((u32*)data, 0, (u32*)out, 0, size, 8);
}

static void C3Di_DownscaleRGBA8(u32* dst, const u32* src[4])
{
	u32 i, j;
	for (i = 0; i < 64; i ++)
	{
		const u32* a = src[i>>4] + (i<<2 & 0x3F);
		u32 dest = 0;
		for (j = 0; j < 32; j += 8)
		{
			u32 val = (((a[0]>>j)&0xFF)+((a[1]>>j)&0xFF)+((a[2]>>j)&0xFF)+((a[3]>>j)&0xFF))>>2;
			dest |= val<<j;
		}
		*dst++ = dest;
	}
}

static void C3Di_DownscaleRGB8(u8* dst, const u8* src[4])
{
	u32 i, j;
	for (i = 0; i < 64; i ++)
	{
		const u8* a = src[i>>4] + 3*(i<<2 & 0x3F);
		for (j = 0; j < 3; j ++)
		{
			*dst++ = ((u32)a[0] + a[3] + a[6] + a[9])>>2;
			a++;
		}
	}
}

void C3D_TexGenerateMipmap(C3D_Tex* tex, GPU_TEXFACE face)
{
	int fmt = tex->fmt;
	size_t block_size = (8*8*fmtSize(fmt))/8;

	/*
	const u32 transfer_flags =
		GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_XY) | BIT(5) |
		GX_TRANSFER_IN_FORMAT(tex->fmt) | GX_TRANSFER_OUT_FORMAT(tex->fmt);
	*/

	void* src = C3Di_TexIs2D(tex) ? tex->data : tex->cube->data[face];
	if (addrIsVRAM(src))
		return; // CPU can't write to VRAM

	int i;
	u32 level_size = tex->size;
	u32 src_width = tex->width;
	u32 src_height = tex->height;
	for (i = 0; i < tex->maxLevel; i ++)
	{
		void* dst = (u8*)src + level_size;
		u32 dst_width = src_width>>1;
		u32 dst_height = src_height>>1;

		/* Doesn't work due to size restriction bullshit
		C3D_SyncDisplayTransfer(
			(u32*)src, GX_BUFFER_DIM(src_width,src_height),
			(u32*)dst, GX_BUFFER_DIM(dst_width,dst_height),
			transfer_flags);
		*/

		u32 i,j;
		u32 src_stride = src_width/8;
		u32 dst_stride = dst_width/8;
		for (j = 0; j < (dst_height/8); j ++)
		{
			for (i = 0; i < dst_stride; i ++)
			{
				void* dst_block = (u8*)dst + block_size*(i + j*dst_stride);
				const void* src_blocks[4] =
				{
					(u8*)src + block_size*(2*i+0 + (2*j+0)*src_stride),
					(u8*)src + block_size*(2*i+1 + (2*j+0)*src_stride),
					(u8*)src + block_size*(2*i+0 + (2*j+1)*src_stride),
					(u8*)src + block_size*(2*i+1 + (2*j+1)*src_stride),
				};
				switch (fmt)
				{
					case GPU_RGBA8:
						C3Di_DownscaleRGBA8(dst_block, (const u32**)src_blocks);
						break;
					case GPU_RGB8:
						C3Di_DownscaleRGB8(dst_block, (const u8**)src_blocks);
					default:
						break;
				}
			}
		}

		level_size >>= 2;
		src = dst;
		src_width = dst_width;
		src_height = dst_height;
	}
}

void C3D_TexBind(int unitId, C3D_Tex* tex)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	if (unitId > 0 && C3D_TexGetType(tex) != GPU_TEX_2D)
		return;

	ctx->flags |= C3DiF_Tex(unitId);
	ctx->tex[unitId] = tex;
}

void C3D_TexFlush(C3D_Tex* tex)
{
	if (!addrIsVRAM(tex->data))
		GSPGPU_FlushDataCache(tex->data, C3D_TexCalcTotalSize(tex->size, tex->maxLevel));
}

void C3D_TexDelete(C3D_Tex* tex)
{
	if (C3Di_TexIs2D(tex))
		allocFree(tex->data);
	else
		C3Di_TexCubeDelete(tex->cube);
}

void C3D_TexShadowParams(bool perspective, float bias)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	u32 iBias = (u32)(fabs(bias) * BIT(24));
	if (iBias >= BIT(24))
		iBias = BIT(24)-1;

	ctx->texShadow = (iBias &~ 1) | (perspective ? 0 : 1);
	ctx->flags |= C3DiF_TexStatus;
}

void C3Di_SetTex(int unit, C3D_Tex* tex)
{
	u32 reg[10];
	u32 regcount = 5;
	reg[0] = tex->border;
	reg[1] = tex->dim;
	reg[2] = tex->param;
	reg[3] = tex->lodParam;
	if (C3Di_TexIs2D(tex))
		reg[4] = osConvertVirtToPhys(tex->data) >> 3;
	else
	{
		int i;
		C3D_TexCube* cube = tex->cube;
		regcount = 10;
		reg[4] = osConvertVirtToPhys(cube->data[0]) >> 3;
		for (i = 1; i < 6; i ++)
			reg[4+i] = (osConvertVirtToPhys(cube->data[i]) >> 3) & 0x3FFFFF;
	}

	switch (unit)
	{
		case 0:
			GPUCMD_AddIncrementalWrites(GPUREG_TEXUNIT0_BORDER_COLOR, reg, regcount);
			GPUCMD_AddWrite(GPUREG_TEXUNIT0_TYPE, tex->fmt);
			break;
		case 1:
			GPUCMD_AddIncrementalWrites(GPUREG_TEXUNIT1_BORDER_COLOR, reg, 5);
			GPUCMD_AddWrite(GPUREG_TEXUNIT1_TYPE, tex->fmt);
			break;
		case 2:
			GPUCMD_AddIncrementalWrites(GPUREG_TEXUNIT2_BORDER_COLOR, reg, 5);
			GPUCMD_AddWrite(GPUREG_TEXUNIT2_TYPE, tex->fmt);
			break;
	}
}
