#include <string.h>
#include "context.h"

void LightLut_FromArray(C3D_LightLut* lut, float* data)
{
	int i;
	for (i = 0; i < 256; i ++)
	{
		float in = data[i], diff = data[i+256];

		u32 val = 0;
		if (in > 0.0)
		{
			in *= 0x1000;
			val = (in < 0x1000) ? (u32)in : 0xFFF;
		}

		u32 val2 = 0;
		if (diff != 0.0)
		{
			if (diff < 0)
			{
				diff = -diff;
				val2 = 0x800;
			}
			diff *= 0x800;
			val2 |= (diff < 0x800) ? (u32)diff : 0x7FF;
		}

		lut->data[i] = val | (val2 << 12);
	}
}

void LightLut_FromFunc(C3D_LightLut* lut, C3D_LightLutFunc func, float param, bool negative)
{
	int i;
	float data[512];
	memset(data, 0, sizeof(data));
	int start = negative ? (-127) : 0;
	for (i = start; i < 128; i ++)
	{
		data[i & 0xFF] = func((float)i / 127.0f, param);
		if (i != start) data[(i & 0xFF) + 255] = data[i & 0xFF] - data[(i-1) & 0xFF];
	}
	LightLut_FromArray(lut, data);
}

void LightLutDA_Create(C3D_LightLutDA* lut, C3D_LightLutFuncDA func, float from, float to, float arg0, float arg1)
{
	int i;
	float data[512];
	data[511] = 0;

	lut->scale = 1.0f / (to-from);
	lut->bias = -from*lut->scale;

	for (i = 0; i < 256; i ++)
	{
		float dist = ((float)i/255 - lut->bias) / lut->scale;
		data[i] = func(dist, arg0, arg1);
		if (i > 0)
			data[i+255] = data[i]-data[i-1];
	}

	LightLut_FromArray(&lut->lut, data);
}
