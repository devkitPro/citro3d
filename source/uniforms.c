#include <c3d/uniforms.h>
//#include <stdio.h>

C3D_FVec C3D_FVUnif[C3D_FVUNIF_COUNT];
C3D_IVec C3D_IVUnif[C3D_IVUNIF_COUNT];
u16      C3D_BoolUnifs;

bool C3D_FVUnifDirty[C3D_FVUNIF_COUNT];
bool C3D_IVUnifDirty[C3D_IVUNIF_COUNT];
bool C3D_BoolUnifsDirty;

void C3D_UpdateUniforms(void)
{
	// Update FVec uniforms
	int i = 0;
	while (i < C3D_FVUNIF_COUNT)
	{
		if (!C3D_FVUnifDirty[i])
		{
			i ++;
			continue;
		}

		// Find the number of consecutive dirty uniforms
		int j;
		for (j = i; j < C3D_FVUNIF_COUNT && C3D_FVUnifDirty[j]; j ++);

		/*
		consoleClear();
		printf("FVEC Uniform %02X--%02X\n", i, j-1);
		int pp;
		for (pp = i; pp < j; pp ++)
			printf("%02X: (%f, %f, %f, %f)\n", pp, C3D_FVUnif[pp].x, C3D_FVUnif[pp].y, C3D_FVUnif[pp].z, C3D_FVUnif[pp].w);
		*/

		// Upload the uniforms
		GPU_SetFloatUniform(GPU_VERTEX_SHADER, i, (u32*)&C3D_FVUnif[i], j-i);

		// Clear the dirty flag
		int k;
		for (k = i; k < j; k ++)
			C3D_FVUnifDirty[k] = false;

		// Advance
		i += j;
	}

	// Update IVec uniforms
	for (i = 0; i < C3D_IVUNIF_COUNT; i ++)
	{
		if (!C3D_IVUnifDirty[i]) continue;

		GPUCMD_AddSingleParam(0x000F02B1+i, C3D_IVUnif[i]);
		C3D_IVUnifDirty[i] = false;
	}

	// Update bool uniforms
	if (C3D_BoolUnifsDirty)
	{
		GPUCMD_AddSingleParam(0x000F02B0, 0x7FFF0000 | (u32)C3D_BoolUnifs);
		C3D_BoolUnifsDirty = false;
	}
}
