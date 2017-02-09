#include "internal.h"

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

bool C3D_TexInitWithParams(C3D_Tex* tex, C3D_TexInitParams p)
{
	if (tex->data) return false;
	if ((p.width|p.height) & 7) return false;

	u32 size = fmtSize(p.format);
	if (!size) return false;
	size *= (u32)p.width * p.height / 8;

	u32 alloc_size = size;
	{
		int i;
		u32 level_size = size;
		for (i = 0; i < p.maxLevel; i ++)
		{
			level_size >>= 2;
			alloc_size += level_size;
		}
	}

	tex->data = p.onVram ? vramAlloc(alloc_size) : linearAlloc(alloc_size);
	if (!tex->data) return false;

	tex->width = p.width;
	tex->height = p.height;
	tex->param = GPU_TEXTURE_MAG_FILTER(GPU_NEAREST) | GPU_TEXTURE_MIN_FILTER(GPU_NEAREST) | GPU_TEXTURE_MODE(p.type);
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

void* C3D_TexGetLevel(C3D_Tex* tex, int level, u32* size)
{
	u8* data = (u8*)tex->data;
	u32 level_size = tex->size;
	int i;
	for (i = 0; i <= tex->maxLevel; i ++)
	{
		if (i == level)
		{
			if (size) *size = level_size;
			return data;
		}
		data += level_size;
		level_size >>= 2;
	}
	return NULL;
}

void C3D_TexUploadLevel(C3D_Tex* tex, const void* data, int level)
{
	if (!tex->data || addrIsVRAM(tex->data))
		return;

	u32 size = 0;
	void* out = C3D_TexGetLevel(tex, level, &size);
	if (out)
		memcpy(out, data, size);
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

void C3D_TexGenerateMipmap(C3D_Tex* tex)
{
	int fmt = tex->fmt;
	size_t block_size = (8*8*fmtSize(fmt))/8;

	/*
	const u32 transfer_flags =
		GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_XY) | BIT(5) |
		GX_TRANSFER_IN_FORMAT(tex->fmt) | GX_TRANSFER_OUT_FORMAT(tex->fmt);
	*/

	int i;
	u32 level_size = tex->size;
	void* src = tex->data;
	u32 src_width = tex->width;
	u32 src_height = tex->height;
	for (i = 0; i < tex->maxLevel; i ++)
	{
		void* dst = (u8*)src + level_size;
		u32 dst_width = src_width>>1;
		u32 dst_height = src_height>>1;

		/* Doesn't work due to size restriction bullshit
		C3D_SafeDisplayTransfer(
			(u32*)src, GX_BUFFER_DIM(src_width,src_height),
			(u32*)dst, GX_BUFFER_DIM(dst_width,dst_height),
			transfer_flags);
		gspWaitForPPF();
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

	if (unitId > 0 && ((tex->param>>28)&7) != GPU_TEX_2D)
		return;

	ctx->flags |= C3DiF_Tex(unitId);
	ctx->tex[unitId] = tex;
}

void C3D_TexFlush(C3D_Tex* tex)
{
	if (tex->data && !addrIsVRAM(tex->data))
		GSPGPU_FlushDataCache(tex->data, tex->size);
}

void C3D_TexDelete(C3D_Tex* tex)
{
	if (!tex->data) return;

	if (addrIsVRAM(tex->data))
		vramFree(tex->data);
	else
		linearFree(tex->data);

	tex->data = NULL;
}
