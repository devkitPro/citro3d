#include "internal.h"

void LightLut_FromArray(C3D_LightLut* lut, float* data)
{
	int i;
	for (i = 0; i < 256; i ++)
	{
		float in = data[i], diff = data[i+256];

		u32 val = 0;
		if (in > 0.0f)
		{
			in *= 0x1000;
			val = (in < 0x1000) ? (u32)in : 0xFFF;
		}

		u32 val2 = 0;
		if (diff != 0.0f)
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
	int min = negative ? (-128) : 0;
	int max = negative ?   128  : 256;
	for (i = min; i <= max; i ++)
	{
		float x   = (float)i/max;
		float val = func(x, param);
		int   idx = negative ? (i & 0xFF) : i;
		if (i < max)
			data[idx] = val;
		if (i > min)
			data[idx+255] = val-data[idx-1];
	}
	LightLut_FromArray(lut, data);
}

void LightLutDA_Create(C3D_LightLutDA* lut, C3D_LightLutFuncDA func, float from, float to, float arg0, float arg1)
{
	int i;
	float data[512];

	float range = to-from;
	lut->scale = 1.0f / range;
	lut->bias = -from*lut->scale;

	for (i = 0; i <= 256; i ++)
	{
		float x   = from + range*i/256.0f;
		float val = func(x, arg0, arg1);
		if (i < 256)
			data[i] = val;
		if (i > 0)
			data[i+255] = val-data[i-1];
	}

	LightLut_FromArray(&lut->lut, data);
}
