#include "internal.h"

void C3Di_LightMtlBlend(C3D_Light* light)
{
	int i;
	C3D_Material* mtl = &light->parent->material;
	C3D_LightMatConf* conf = &light->conf.material;
	memset(conf, 0, sizeof(*conf));

	for (i = 0; i < 3; i ++)
	{
		conf->specular0 |= ((u32)(255*(mtl->specular0[i]*light->specular0[i]))) << (i*10);
		conf->specular1 |= ((u32)(255*(mtl->specular1[i]*light->specular1[i]))) << (i*10);
		conf->diffuse   |= ((u32)(255*(mtl->diffuse[i]  *light->diffuse[i])))   << (i*10);
		conf->ambient   |= ((u32)(255*(mtl->ambient[i]  *light->ambient[i])))   << (i*10);
	}
}

int C3D_LightInit(C3D_Light* light, C3D_LightEnv* env)
{
	int i;
	memset(light, 0, sizeof(*light));

	for (i = 0; i < 8; i ++)
		if (!env->lights[i])
			break;
	if (i == 8) return -1;

	env->lights[i] = light;
	light->flags = C3DF_Light_Enabled | C3DF_Light_Dirty | C3DF_Light_MatDirty;
	light->id = i;
	light->parent = env;
	light->diffuse[0]   = light->diffuse[1]   = light->diffuse[2]   = 1.0f;
	light->specular0[0] = light->specular0[1] = light->specular0[2] = 1.0f;
	light->specular1[0] = light->specular1[1] = light->specular1[2] = 1.0f;

	env->flags |= C3DF_LightEnv_LCDirty;
	return i;
}

void C3D_LightEnable(C3D_Light* light, bool enable)
{
	if ((light->flags & C3DF_Light_Enabled) == (enable?C3DF_Light_Enabled:0))
		return;

	if (enable)
		light->flags |= C3DF_Light_Enabled;
	else
		light->flags &= ~C3DF_Light_Enabled;

	light->parent->flags |= C3DF_LightEnv_LCDirty;
}

void C3D_LightTwoSideDiffuse(C3D_Light* light, bool enable)
{
	if (enable)
		light->conf.config |= BIT(1);
	else
		light->conf.config &= ~BIT(1);
	light->flags |= C3DF_Light_Dirty;
}

void C3D_LightGeoFactor(C3D_Light* light, int id, bool enable)
{
	id = 2 + (id&1);
	if (enable)
		light->conf.config |= BIT(id);
	else
		light->conf.config &= ~BIT(id);
	light->flags |= C3DF_Light_Dirty;
}

void C3D_LightAmbient(C3D_Light* light, float r, float g, float b)
{
	light->ambient[0] = b;
	light->ambient[1] = g;
	light->ambient[2] = r;
	light->flags |= C3DF_Light_MatDirty;
}

void C3D_LightDiffuse(C3D_Light* light, float r, float g, float b)
{
	light->diffuse[0] = b;
	light->diffuse[1] = g;
	light->diffuse[2] = r;
	light->flags |= C3DF_Light_MatDirty;
}

void C3D_LightSpecular0(C3D_Light* light, float r, float g, float b)
{
	light->specular0[0] = b;
	light->specular0[1] = g;
	light->specular0[2] = r;
	light->flags |= C3DF_Light_MatDirty;
}

void C3D_LightSpecular1(C3D_Light* light, float r, float g, float b)
{
	light->specular1[0] = b;
	light->specular1[1] = g;
	light->specular1[2] = r;
	light->flags |= C3DF_Light_MatDirty;
}

void C3D_LightPosition(C3D_Light* light, C3D_FVec* pos)
{
	// Enable/disable positional light depending on W coordinate
	light->conf.config &= ~BIT(0);
	light->conf.config |= (pos->w == 0.0f);
	light->conf.position[0] = f32tof16(pos->x);
	light->conf.position[1] = f32tof16(pos->y);
	light->conf.position[2] = f32tof16(pos->z);
	light->flags |= C3DF_Light_Dirty;
}

static void C3Di_EnableCommon(C3D_Light* light, bool enable, u32 bit)
{
	C3D_LightEnv* env = light->parent;
	u32* var = &env->conf.config[1];

	if (enable == !(*var & bit))
		return;

	if (!enable)
		*var |= bit;
	else
		*var &= ~bit;

	env->flags |= C3DF_LightEnv_Dirty;
}

void C3D_LightShadowEnable(C3D_Light* light, bool enable)
{
	C3Di_EnableCommon(light, enable, GPU_LC1_SHADOWBIT(light->id));
}

void C3D_LightSpotEnable(C3D_Light* light, bool enable)
{
	C3Di_EnableCommon(light, enable, GPU_LC1_SPOTBIT(light->id));
}

static inline u16 floattofix2_11(float x)
{
	return (u16)((s32)(x * (1U<<11)) & 0x1FFF);
}

void C3D_LightSpotDir(C3D_Light* light, float x, float y, float z)
{
	C3Di_EnableCommon(light, true, GPU_LC1_SPOTBIT(light->id));
	C3D_FVec vec = FVec3_New(-x, -y, -z);
	vec = FVec3_Normalize(vec);
	light->conf.spotDir[0] = floattofix2_11(vec.x);
	light->conf.spotDir[1] = floattofix2_11(vec.y);
	light->conf.spotDir[2] = floattofix2_11(vec.z);
	light->flags |= C3DF_Light_Dirty;
}

void C3D_LightSpotLut(C3D_Light* light, C3D_LightLut* lut)
{
	bool hasLut = lut != NULL;
	C3Di_EnableCommon(light, hasLut, GPU_LC1_SPOTBIT(light->id));
	light->lut_SP = lut;
	if (hasLut)
		light->flags |= C3DF_Light_SPDirty;
}

void C3D_LightDistAttnEnable(C3D_Light* light, bool enable)
{
	C3Di_EnableCommon(light, enable, GPU_LC1_ATTNBIT(light->id));
}

void C3D_LightDistAttn(C3D_Light* light, C3D_LightLutDA* lut)
{
	bool hasLut = lut != NULL;
	C3Di_EnableCommon(light, hasLut, GPU_LC1_ATTNBIT(light->id));
	if (!hasLut) return;

	light->conf.distAttnBias  = f32tof20(lut->bias);
	light->conf.distAttnScale = f32tof20(lut->scale);
	light->lut_DA = &lut->lut;
	light->flags |= C3DF_Light_Dirty | C3DF_Light_DADirty;
}
