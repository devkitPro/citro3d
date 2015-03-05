#include <c3d/uniforms.h>
//#include <stdio.h>

C3D_FVec C3D_FVUnif[2][C3D_FVUNIF_COUNT];
C3D_IVec C3D_IVUnif[2][C3D_IVUNIF_COUNT];
u16      C3D_BoolUnifs[2];

bool C3D_FVUnifDirty[2][C3D_FVUNIF_COUNT];
bool C3D_IVUnifDirty[2][C3D_IVUNIF_COUNT];
bool C3D_BoolUnifsDirty[2];

void C3D_UpdateUniforms(GPU_SHADER_TYPE type)
{
	int offset = type == GPU_GEOMETRY_SHADER ? (GPUREG_GSH_BOOLUNIFORM-GPUREG_VSH_BOOLUNIFORM) : 0;

	// Update FVec uniforms
	int i = 0;
	while (i < C3D_FVUNIF_COUNT)
	{
		if (!C3D_FVUnifDirty[type][i])
		{
			i ++;
			continue;
		}

		// Find the number of consecutive dirty uniforms
		int j;
		for (j = i; j < C3D_FVUNIF_COUNT && C3D_FVUnifDirty[type][j]; j ++);

		/*
		consoleClear();
		printf("FVEC Uniform %02X--%02X\n", i, j-1);
		int pp;
		for (pp = i; pp < j; pp ++)
			printf("%02X: (%f, %f, %f, %f)\n", pp, C3D_FVUnif[pp].x, C3D_FVUnif[pp].y, C3D_FVUnif[pp].z, C3D_FVUnif[pp].w);
		*/

		// Upload the uniforms
		GPU_SetFloatUniform(type, i, (u32*)&C3D_FVUnif[i], j-i);

		// Clear the dirty flag
		int k;
		for (k = i; k < j; k ++)
			C3D_FVUnifDirty[type][k] = false;

		// Advance
		i += j;
	}

	// Update IVec uniforms
	for (i = 0; i < C3D_IVUNIF_COUNT; i ++)
	{
		if (!C3D_IVUnifDirty[type][i]) continue;

		GPUCMD_AddWrite(GPUREG_VSH_INTUNIFORM_I0+offset+i, C3D_IVUnif[type][i]);
		C3D_IVUnifDirty[type][i] = false;
	}

	// Update bool uniforms
	if (C3D_BoolUnifsDirty[type])
	{
		GPUCMD_AddWrite(GPUREG_VSH_BOOLUNIFORM+offset, 0x7FFF0000 | (u32)C3D_BoolUnifs[type]);
		C3D_BoolUnifsDirty[type] = false;
	}
}
