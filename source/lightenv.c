#include <string.h>
#include "context.h"

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

static void C3Di_LightEnvUpdate(C3D_LightEnv* env)
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
		env->flags &= ~C3DF_LightEnv_LCDirty;
		env->flags |= C3DF_LightEnv_Dirty;
	}

	if (env->flags & C3DF_Light_MatDirty)
	{
		C3Di_LightEnvMtlBlend(env);
		env->flags &= ~C3DF_Light_MatDirty;
		env->flags |= C3DF_LightEnv_Dirty;
	}

	if (env->flags & C3DF_LightEnv_Dirty)
	{
		GPUCMD_AddWrite(GPUREG_LIGHTING_AMBIENT, conf->ambient);
		GPUCMD_AddIncrementalWrites(GPUREG_LIGHTING_NUM_LIGHTS, (u32*)&conf->numLights, 3);
		GPUCMD_AddIncrementalWrites(GPUREG_LIGHTING_LUTINPUT_ABS, (u32*)&conf->lutInput, 3);
		GPUCMD_AddWrite(GPUREG_LIGHTING_LIGHT_PERMUTATION, conf->permutation);
		env->flags &= ~C3DF_LightEnv_Dirty;
	}

	if (env->flags & C3DF_LightEnv_LutDirtyAll)
	{
		for (i = 0; i < 5; i ++)
		{
			if (!(env->flags & C3DF_LightEnv_LutDirty(i))) continue;
			// TODO: Upload LUT
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
			// TODO: Upload LUT
			light->flags &= ~C3DF_Light_SPDirty;
		}

		if (light->flags & C3DF_Light_DADirty)
		{
			// TODO: Upload LUT
			light->flags &= ~C3DF_Light_DADirty;
		}
	}
}

static void C3Di_LightEnvDirty(C3D_LightEnv* env)
{
	env->flags |= C3DF_LightEnv_Dirty;
	int i;
	for (i = 0; i < 5; i ++)
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
	env->Update = C3Di_LightEnvUpdate;
	env->Dirty = C3Di_LightEnvDirty;

	env->flags = C3DF_LightEnv_Dirty;
	env->conf.config[0] = (4<<8) | BIT(27) | BIT(31);
	env->conf.config[1] = ~0;
	env->conf.lutInput.abs = 0xFF;
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

void C3D_LightEnvMaterial(C3D_LightEnv* env, C3D_Material* mtl)
{
	int i;
	memcpy(&env->material, mtl, sizeof(C3D_Material));
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
