#pragma once
#include "types.h"

typedef struct
{
	u32 offset;
	u32 flags[2];
} C3D_BufCfg;

typedef struct
{
	u32 base_paddr;
	int bufCount;
	C3D_BufCfg buffers[12];
} C3D_BufInfo;

void BufInfo_Init(C3D_BufInfo* info);
int  BufInfo_Add(C3D_BufInfo* info, const void* data, ptrdiff_t stride, int attribCount, u64 permutation);

C3D_BufInfo* C3D_GetBufInfo(void);
void C3D_SetBufInfo(C3D_BufInfo* info);

typedef struct
{
	void* data;
	size_t capacity;
	size_t size;
	int vertexCount;
} C3D_VBO;

bool C3D_VBOInit(C3D_VBO* vbo, size_t capacity);
bool C3D_VBOAddData(C3D_VBO* vbo, const void* data, size_t size, int vertexCount);
void C3D_VBOFlush(C3D_VBO* vbo);
void C3D_VBODelete(C3D_VBO* vbo);

typedef struct
{
	void* data;
	int capacity;
	int indexCount;
	int format;
} C3D_IBO;

enum
{
	C3D_UNSIGNED_BYTE = 0,
	C3D_UNSIGNED_SHORT = 1,
};

bool C3D_IBOInit(C3D_IBO* ibo, int capacity, int format);
bool C3D_IBOAddData(C3D_IBO* ibo, const void* data, int indexCount);
void C3D_IBOFlush(C3D_IBO* ibo);
void C3D_IBODelete(C3D_IBO* ibo);
