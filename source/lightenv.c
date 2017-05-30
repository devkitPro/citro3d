#include "internal.h"

static void C3Di_LightEnvMtlBlend(C3D_LightEnv* env)
{
	int i;
	C3D_Material* mtl = &env->material;
	u32 color = 0;
	for (i = 0; i < 3; i ++)
	{
		int v = 255*(mtl->emission[i] + mtl->ambient[i]*env->ambient[i]);
		if (v < 0) v = 0;
		else if (v > 255) v = 255;
		color |= v << (i*10);
	}
	env->conf.ambient = color;
}

static void C3Di_LightLutUpload(u32 config, C3D_LightLut* lut)
{
	int i;
	GPUCMD_AddWrite(GPUREG_LIGHTING_LUT_INDEX, config);
	for (i = 0; i < 256; i += 8)
		GPUCMD_AddWrites(GPUREG_LIGHTING_LUT_DATA0, &lut->data[i], 8);
}

static void C3Di_LightEnvSelectLayer(C3D_LightEnv* env)
{
	static const u8 layer_enabled[] =
	{
		BIT(GPU_LUT_D0) | BIT(GPU_LUT_RR) | BIT(GPU_LUT_SP) | BIT(GPU_LUT_DA),
		BIT(GPU_LUT_FR) | BIT(GPU_LUT_RR) | BIT(GPU_LUT_SP) | BIT(GPU_LUT_DA),
		BIT(GPU_LUT_D0) | BIT(GPU_LUT_D1) | BIT(GPU_LUT_RR) | BIT(GPU_LUT_DA),
		BIT(GPU_LUT_D0) | BIT(GPU_LUT_D1) | BIT(GPU_LUT_FR) | BIT(GPU_LUT_DA),
		0xFF &~ BIT(GPU_LUT_FR),
		0xFF &~ BIT(GPU_LUT_D1),
		0xFF &~ (BIT(GPU_LUT_RB) | BIT(GPU_LUT_RG)),
	};

	u32 reg = ~env->conf.config[1];
	if (reg & (0xFF<< 8)) reg |= GPU_LC1_LUTBIT(GPU_LUT_SP);
	if (reg & (0xFF<<24)) reg |= GPU_LC1_LUTBIT(GPU_LUT_DA);
	reg = (reg >> 16) & 0xFF;

	int i = 7;
	if (!(env->flags & C3DF_LightEnv_IsCP_Any))
		for (i = 0; i < 7; i ++)
			if ((layer_enabled[i] & reg) == reg) // Check if the layer supports all LUTs we need
				break;
	env->conf.config[0] = (env->conf.config[0] &~ (0xF<<4)) | (GPU_LIGHT_ENV_LAYER_CONFIG(i)<<4);
}

void C3Di_LightEnvUpdate(C3D_LightEnv* env)
{
	int i;
	C3D_LightEnvConf* conf = &env->conf;

	if (env->flags & C3DF_LightEnv_LCDirty)
	{
		conf->numLights = 0;
		conf->permutation = 0;
		for (i = 0; i < 8; i ++)
		{
			C3D_Light* light = env->lights[i];
			if (!light) continue;
			if (!(light->flags & C3DF_Light_Enabled)) continue;
			conf->permutation |= GPU_LIGHTPERM(conf->numLights++, i);
		}
		if (conf->numLights > 0) conf->numLights --;
		env->flags &= ~C3DF_LightEnv_LCDirty;
		env->flags |= C3DF_LightEnv_Dirty;
	}

	if (env->flags & C3DF_LightEnv_MtlDirty)
	{
		C3Di_LightEnvMtlBlend(env);
		env->flags &= ~C3DF_LightEnv_MtlDirty;
		env->flags |= C3DF_LightEnv_Dirty;
	}

	if (env->flags & C3DF_LightEnv_Dirty)
	{
		C3Di_LightEnvSelectLayer(env);
		GPUCMD_AddWrite(GPUREG_LIGHTING_AMBIENT, conf->ambient);
		GPUCMD_AddIncrementalWrites(GPUREG_LIGHTING_NUM_LIGHTS, (u32*)&conf->numLights, 3);
		GPUCMD_AddIncrementalWrites(GPUREG_LIGHTING_LUTINPUT_ABS, (u32*)&conf->lutInput, 3);
		GPUCMD_AddWrite(GPUREG_LIGHTING_LIGHT_PERMUTATION, conf->permutation);
		env->flags &= ~C3DF_LightEnv_Dirty;
	}

	if (env->flags & C3DF_LightEnv_LutDirtyAll)
	{
		for (i = 0; i < 6; i ++)
		{
			static const u8 lutIds[] = { 0, 1, 3, 4, 5, 6 };
			if (!(env->flags & C3DF_LightEnv_LutDirty(i))) continue;
			C3Di_LightLutUpload(GPU_LIGHTLUTIDX(GPU_LUTSELECT_COMMON, (u32)lutIds[i], 0), env->luts[i]);
		}

		env->flags &= ~C3DF_LightEnv_LutDirtyAll;
	}

	for (i = 0; i < 8; i ++)
	{
		C3D_Light* light = env->lights[i];
		if (!light) continue;

		if (light->flags & C3DF_Light_MatDirty)
		{
			C3Di_LightMtlBlend(light);
			light->flags &= ~C3DF_Light_MatDirty;
			light->flags |= C3DF_Light_Dirty;
		}

		if (light->flags & C3DF_Light_Dirty)
		{
			GPUCMD_AddIncrementalWrites(GPUREG_LIGHT0_SPECULAR0 + i*0x10, (u32*)&light->conf, 12);
			light->flags &= ~C3DF_Light_Dirty;
		}

		if (light->flags & C3DF_Light_SPDirty)
		{
			C3Di_LightLutUpload(GPU_LIGHTLUTIDX(GPU_LUTSELECT_SP, i, 0), light->lut_SP);
			light->flags &= ~C3DF_Light_SPDirty;
		}

		if (light->flags & C3DF_Light_DADirty)
		{
			C3Di_LightLutUpload(GPU_LIGHTLUTIDX(GPU_LUTSELECT_DA, i, 0), light->lut_DA);
			light->flags &= ~C3DF_Light_DADirty;
		}
	}
}

void C3Di_LightEnvDirty(C3D_LightEnv* env)
{
	env->flags |= C3DF_LightEnv_Dirty;
	int i;
	for (i = 0; i < 6; i ++)
		if (env->luts[i])
			env->flags |= C3DF_LightEnv_LutDirty(i);
	for (i = 0; i < 8; i ++)
	{
		C3D_Light* light = env->lights[i];
		if (!light) continue;

		light->flags |= C3DF_Light_Dirty;
		if (light->lut_SP)
			light->flags |= C3DF_Light_SPDirty;
		if (light->lut_DA)
			light->flags |= C3DF_Light_DADirty;
	}
}

void C3D_LightEnvInit(C3D_LightEnv* env)
{
	memset(env, 0, sizeof(*env));
	env->flags = C3DF_LightEnv_Dirty;
	env->ambient[0] = env->ambient[1] = env->ambient[2] = 1.0f;
	env->conf.config[0] = (4<<8) | BIT(27) | BIT(31);
	env->conf.config[1] = ~0;
	env->conf.lutInput.select = GPU_LIGHTLUTINPUT(GPU_LUT_SP, GPU_LUTINPUT_SP);
	env->conf.lutInput.abs = 0x2222222;
}

void C3D_LightEnvBind(C3D_LightEnv* env)
{
	C3D_Context* ctx = C3Di_GetContext();

	if (!(ctx->flags & C3DiF_Active))
		return;

	if (ctx->lightEnv == env)
		return;

	ctx->flags |= C3DiF_LightEnv;
	ctx->lightEnv = env;
}

void C3D_LightEnvMaterial(C3D_LightEnv* env, const C3D_Material* mtl)
{
	int i;
	memcpy(&env->material, mtl, sizeof(*mtl));
	env->flags |= C3DF_LightEnv_MtlDirty;
	for (i = 0; i < 8; i ++)
	{
		C3D_Light* light = env->lights[i];
		if (light) light->flags |= C3DF_Light_MatDirty;
	}
}

void C3D_LightEnvAmbient(C3D_LightEnv* env, float r, float g, float b)
{
	env->ambient[0] = b;
	env->ambient[1] = g;
	env->ambient[2] = r;
	env->flags |= C3DF_LightEnv_MtlDirty;
}

void C3D_LightEnvLut(C3D_LightEnv* env, GPU_LIGHTLUTID lutId, GPU_LIGHTLUTINPUT input, bool negative, C3D_LightLut* lut)
{
	static const s8 ids[] = { 0, 1, -1, 2, 3, 4, 5, -1 };
	int id = ids[lutId];
	if (id >= 0)
	{
		env->luts[id] = lut;
		if (lut)
		{
			env->conf.config[1] &= ~GPU_LC1_LUTBIT(lutId);
			env->flags |= C3DF_LightEnv_LutDirty(id);
		} else
		{
			env->conf.config[1] |= GPU_LC1_LUTBIT(lutId);
			env->luts[id] = NULL;
		}
	}

	env->conf.lutInput.select &= ~GPU_LIGHTLUTINPUT(lutId, 0xF);
	env->conf.lutInput.select |=  GPU_LIGHTLUTINPUT(lutId, input);

	u32 absbit = 1 << (lutId*4 + 1);
	env->conf.lutInput.abs &= ~absbit;
	if (negative)
		env->conf.lutInput.abs |= absbit;

	env->flags |= C3DF_LightEnv_Dirty;
	if (input == GPU_LUTINPUT_CP)
		env->flags |= C3DF_LightEnv_IsCP(lutId);
	else
		env->flags &= ~C3DF_LightEnv_IsCP(lutId);
}

void C3D_LightEnvFresnel(C3D_LightEnv* env, GPU_FRESNELSEL selector)
{
	env->conf.config[0] &= ~(3<<2);
	env->conf.config[0] |= (selector&3)<<2;
	env->flags |= C3DF_LightEnv_Dirty;
}

void C3D_LightEnvBumpMode(C3D_LightEnv* env, GPU_BUMPMODE mode)
{
	env->conf.config[0] &= ~(3<<28);
	env->conf.config[0] |= (mode&3)<<28;
	env->flags |= C3DF_LightEnv_Dirty;
}

void C3D_LightEnvBumpSel(C3D_LightEnv* env, int texUnit)
{
	env->conf.config[0] &= ~(3<<22);
	env->conf.config[0] |= (texUnit&3)<<22;
	env->flags |= C3DF_LightEnv_Dirty;
}

void C3D_LightEnvShadowMode(C3D_LightEnv* env, u32 mode)
{
	mode &= 0xF<<16;
	if (mode & (GPU_SHADOW_PRIMARY | GPU_SHADOW_SECONDARY | GPU_SHADOW_ALPHA))
		mode |= BIT(0);
	env->conf.config[0] &= ~((0xF<<16) | BIT(0));
	env->conf.config[0] |= mode;
	env->flags |= C3DF_LightEnv_Dirty;
}

void C3D_LightEnvShadowSel(C3D_LightEnv* env, int texUnit)
{
	env->conf.config[0] &= ~(3<<24);
	env->conf.config[0] |= (texUnit&3)<<24;
	env->flags |= C3DF_LightEnv_Dirty;
}

void C3D_LightEnvClampHighlights(C3D_LightEnv* env, bool clamp)
{
	if (clamp)
		env->conf.config[0] |= BIT(27);
	else
		env->conf.config[0] &= ~BIT(27);
	env->flags |= C3DF_LightEnv_Dirty;
}
