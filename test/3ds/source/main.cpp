#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <3ds.h>
#include <citro3d.h>

#include "vshader_shbin.h"

#define CLEAR_COLOR 0x777777FF

#define DISPLAY_TRANSFER_FLAGS \
  (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
   GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
   GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace
{

template<class T>
inline T clamp(T val, T min, T max)
{
  return std::max(min, std::min(max, val));
}

typedef struct
{
  float position[3];
  float texcoord[2];
  float normal[3];
} attribute_t;

const attribute_t attribute_list[] =
{
  { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f } },
  { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f,  1.0f } },
  { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f,  1.0f } },
  { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f,  1.0f } },
  { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f } },
  { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f } },

  { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
  { { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
  { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
  { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
  { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
  { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },

  { {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, {  1.0f, 0.0f, 0.0f } },
  { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, {  1.0f, 0.0f, 0.0f } },
  { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, {  1.0f, 0.0f, 0.0f } },
  { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, {  1.0f, 0.0f, 0.0f } },
  { {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, {  1.0f, 0.0f, 0.0f } },
  { {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, {  1.0f, 0.0f, 0.0f } },

  { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
  { { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
  { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
  { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
  { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
  { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },

  { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f,  1.0f, 0.0f } },
  { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0f,  1.0f, 0.0f } },
  { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f,  1.0f, 0.0f } },
  { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f,  1.0f, 0.0f } },
  { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f,  1.0f, 0.0f } },
  { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f,  1.0f, 0.0f } },

  { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
  { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
  { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
  { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
  { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
  { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
};

#define attribute_list_count (sizeof(attribute_list)/sizeof(attribute_list[0]))

int uLoc_projection, uLoc_modelView, uLoc_texView;
int uLoc_lightVec, uLoc_lightHalfVec, uLoc_lightClr, uLoc_material;
C3D_Mtx material =
{
  {
    { { 0.0f, 0.2f, 0.2f, 0.2f } }, // Ambient
    { { 0.0f, 0.4f, 0.4f, 0.4f } }, // Diffuse
    { { 0.0f, 0.8f, 0.8f, 0.8f } }, // Specular
    { { 1.0f, 0.0f, 0.0f, 0.0f } }, // Emission
  }
};

struct
{
  C3D_Tex      tex;
  const char   *path;
  size_t       width, height;
  GPU_TEXCOLOR format;
} texture[] =
{
  { {}, "romfs:/logo.bin",  64, 64, GPU_RGBA8, },
};

#define num_textures (sizeof(texture)/sizeof(texture[0]))
  
void *vbo_data;

void sceneInit(shaderProgram_s *program)
{
  uLoc_projection   = shaderInstanceGetUniformLocation(program->vertexShader, "projection");
  uLoc_modelView    = shaderInstanceGetUniformLocation(program->vertexShader, "modelView");
  uLoc_texView      = shaderInstanceGetUniformLocation(program->vertexShader, "texView");
  uLoc_lightVec     = shaderInstanceGetUniformLocation(program->vertexShader, "lightVec");
  uLoc_lightHalfVec = shaderInstanceGetUniformLocation(program->vertexShader, "lightHalfVec");
  uLoc_lightClr     = shaderInstanceGetUniformLocation(program->vertexShader, "lightClr");
  uLoc_material     = shaderInstanceGetUniformLocation(program->vertexShader, "material");

  // Configure attributes for use with the vertex shader
  C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
  AttrInfo_Init(attrInfo);
  AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
  AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
  AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal

  // Create the VBO (vertex buffer object)
  vbo_data = linearAlloc(sizeof(attribute_list));
  std::memcpy(vbo_data, attribute_list, sizeof(attribute_list));

  // Configure buffers
  C3D_BufInfo* bufInfo = C3D_GetBufInfo();
  BufInfo_Init(bufInfo);
  BufInfo_Add(bufInfo, vbo_data, sizeof(attribute_t), 3, 0x210);
  
  // Load the texture and bind it to the first texture unit
  for(size_t i = 0; i < num_textures; ++i)
  {
    struct stat st;
    int         fd = ::open(texture[i].path, O_RDONLY);
    ::fstat(fd, &st);

    size_t size = st.st_size;

    void *buffer = std::malloc(size);
    void *p      = buffer;
  
    while(size > 0)
    {
      ssize_t rc = ::read(fd, p, size);
      if(rc <= 0 || static_cast<size_t>(rc) > size)
        break;

      size -= rc;
      p    =  reinterpret_cast<char*>(p) + rc;
    }

    ::close(fd);

    C3D_TexInit(&texture[i].tex, texture[i].width, texture[i].height, texture[i].format);
    C3D_TexUpload(&texture[i].tex, buffer);
    C3D_TexSetFilter(&texture[i].tex, GPU_LINEAR, GPU_NEAREST);

    ::free(buffer);
  }

  C3D_TexBind(0, &texture[0].tex);

  // Configure the first fragment shading substage to blend the texture color with
  // the vertex color (calculated by the vertex shader using a lighting algorithm)
  // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
  C3D_TexEnv* env = C3D_GetTexEnv(0);
  C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
  C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
  C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
}

void sceneExit()
{
  for(size_t i = 0; i < num_textures; ++i)
    C3D_TexDelete(&texture[i].tex);

  linearFree(vbo_data);
}

void persp_tilt_test()
{
  C3D_RenderTarget *top;
  C3D_Mtx          projection;
  C3D_Mtx          modelView;
  C3D_Mtx          texView;
  float            x = 0.0f, y = 0.0f, z = -2.0f,
                   old_x = x, old_y = y, old_z = z;
  float            angle = 0.0f;

  top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(top, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

  Mtx_PerspTilt(&projection, 60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f, false);

  Mtx_Identity(&modelView);
  Mtx_Translate(&modelView, x, y, z, true);

  Mtx_Identity(&texView);

  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);
  C3D_FVUnifMtx2x4(GPU_VERTEX_SHADER, uLoc_texView,    &texView);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,   &material);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);

  C3D_TexBind(0, &texture[0].tex);

  std::printf("\x1b[2J");
  std::printf("(LEFT/RIGHT) x %.1f\n", x);
  std::printf("(UP/DOWN)    y %.1f\n", y);
  std::printf("(L/R)        z %.1f\n", z);

  while(aptMainLoop())
  {
    gspWaitForVBlank();

    hidScanInput();
    u32 down = hidKeysDown();
    u32 held = hidKeysHeld();
    if(down & (KEY_START|KEY_SELECT))
      break;

    old_x = x;
    old_y = y;
    old_z = z;

    if((down | held) & KEY_LEFT)
      x = clamp(x - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_RIGHT)
      x = clamp(x + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_UP)
      y = clamp(y + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_DOWN)
      y = clamp(y - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_L)
      z = clamp(z + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_R)
      z = clamp(z - 0.1f, -10.0f, 10.0f);

    if((x != old_x) || (y != old_y) || (z != old_z))
    {
      std::printf("\x1b[0;0H");
      std::printf("(LEFT/RIGHT) x %.1f\n", x);
      std::printf("(UP/DOWN)    y %.1f\n", y);
      std::printf("(L/R)        z %.1f\n", z);
    }

    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, x, y, z, true);
    Mtx_RotateY(&modelView, angle*M_TAU/360.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    angle += 1.0f;
    if(angle >= 360.0f)
      angle = 0.0f;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(top);
    C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);
    C3D_FrameEnd(0);
  }

  C3D_RenderTargetDelete(top);
}

void ortho_tilt_test()
{
  C3D_RenderTarget *top;
  C3D_Mtx          projection;
  C3D_Mtx          modelView;
  C3D_Mtx          texView;
  float            x = 0.0f, y = 0.0f, z = 0.0f,
                   old_x = x, old_y = y, old_z = z;
  float            angle = 0.0f;

  top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(top, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

  Mtx_OrthoTilt(&projection, 0.0f, 400.0f, 0.0f, 240.0f, 100.0f, -100.0f, false);

  Mtx_Identity(&texView);

  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);
  C3D_FVUnifMtx2x4(GPU_VERTEX_SHADER, uLoc_texView,    &texView);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,   &material);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);

  C3D_TexBind(0, &texture[0].tex);

  std::printf("\x1b[2J");
  std::printf("(LEFT/RIGHT) x %.1f\n", x);
  std::printf("(UP/DOWN)    y %.1f\n", y);
  std::printf("(L/R)        z %.1f\n", z);

  while(aptMainLoop())
  {
    gspWaitForVBlank();

    hidScanInput();
    u32 down = hidKeysDown();
    u32 held = hidKeysHeld();
    if(down & (KEY_START|KEY_SELECT))
      break;

    old_x = x;
    old_y = y;
    old_z = z;

    if((down | held) & KEY_LEFT)
      x = clamp(x - 1.0f, 0.0f, 400.0f);
    if((down | held) & KEY_RIGHT)
      x = clamp(x + 1.0f, 0.0f, 400.0f);
    if((down | held) & KEY_UP)
      y = clamp(y + 1.0f, 0.0f, 240.0f);
    if((down | held) & KEY_DOWN)
      y = clamp(y - 1.0f, 0.0f, 240.0f);
    if((down | held) & KEY_L)
      z = clamp(z + 1.0f, -100.0f, 100.0f);
    if((down | held) & KEY_R)
      z = clamp(z - 1.0f, -100.0f, 100.0f);

    if((x != old_x) || (y != old_y) || (z != old_z))
    {
      std::printf("\x1b[0;0H");
      std::printf("(LEFT/RIGHT) x %.1f\n", x);
      std::printf("(UP/DOWN)    y %.1f\n", y);
      std::printf("(L/R)        z %.1f\n", z);
    }

    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, x, y, z, true);
    Mtx_Scale(&modelView, 64.0f, 64.0f, 64.0f);
    Mtx_RotateY(&modelView, angle*M_TAU/360.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    angle += 1.0f;
    if(angle >= 360.0f)
      angle = 0.0f;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(top);
    C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);
    C3D_FrameEnd(0);
  }

  C3D_RenderTargetDelete(top);
}

void stereo_tilt_test()
{
  C3D_RenderTarget *topLeft, *topRight;
  C3D_Mtx          projLeft, projRight;
  C3D_Mtx          modelView;
  C3D_Mtx          texView;
  float            x = 0.0f, y = 0.0f, z = -2.0f,
                   old_x = x, old_y = y, old_z = z;
  float            iod = osGet3DSliderState(), old_iod = iod;
  float            focLen = 2.0f, old_focLen = focLen;
  float            angle = 0.0f;

  gfxSet3D(true);

  topLeft  = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  topRight = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(topLeft,  C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetClear(topRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetOutput(topLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
  C3D_RenderTargetSetOutput(topRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);

  Mtx_PerspStereoTilt(&projLeft,  60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f, -iod, focLen, false);
  Mtx_PerspStereoTilt(&projRight, 60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f,  iod, focLen, false);

  Mtx_Identity(&texView);

  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);
  C3D_FVUnifMtx2x4(GPU_VERTEX_SHADER, uLoc_texView,    &texView);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,   &material);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);

  C3D_TexBind(0, &texture[0].tex);

  std::printf("\x1b[2J");
  std::printf("(LEFT/RIGHT) x      %.1f\n", x);
  std::printf("(UP/DOWN)    y      %.1f\n", y);
  std::printf("(L/R)        z      %.1f\n", z);
  std::printf("(Y/A)        focLen %.1f\n", focLen);
  std::printf("(3D Slider)  iod    %.1f\n", iod);

  while(aptMainLoop())
  {
    gspWaitForVBlank();

    hidScanInput();
    u32 down = hidKeysDown();
    u32 held = hidKeysHeld();
    if(down & (KEY_START|KEY_SELECT))
      break;

    old_x      = x;
    old_y      = y;
    old_z      = z;
    old_focLen = focLen;
    old_iod    = iod;

    if((down | held) & KEY_LEFT)
      x = clamp(x - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_RIGHT)
      x = clamp(x + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_UP)
      y = clamp(y + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_DOWN)
      y = clamp(y - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_L)
      z = clamp(z + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_R)
      z = clamp(z - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_Y)
      focLen = clamp(focLen - 0.1f, 0.1f, 10.0f);
    if((down | held) & KEY_A)
      focLen = clamp(focLen + 0.1f, 0.1f, 10.0f);

    iod = osGet3DSliderState();

    if((x != old_x) || (y != old_y) || (z != old_z)
    || (focLen != old_focLen) || (iod != old_iod))
    {
      std::printf("\x1b[0;0H");
      std::printf("(LEFT/RIGHT) x      %.1f\n", x);
      std::printf("(UP/DOWN)    y      %.1f\n", y);
      std::printf("(L/R)        z      %.1f\n", z);
      std::printf("(Y/A)        focLen %.1f\n", focLen);
      std::printf("(3D Slider)  iod    %.1f\n", iod);

      Mtx_PerspStereoTilt(&projLeft,  60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f, -iod, focLen, false);
      Mtx_PerspStereoTilt(&projRight, 60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f,  iod, focLen, false);
    }

    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, x, y, z, true);
    Mtx_RotateY(&modelView, angle*M_TAU/360.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    angle += 1.0f;
    if(angle >= 360.0f)
      angle = 0.0f;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C3D_FrameDrawOn(topLeft);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projLeft);
    C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);

    if(iod > 0.0f)
    {
      C3D_FrameDrawOn(topRight);
      C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projRight);
      C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);
    }

    C3D_FrameEnd(0);
  }

  C3D_RenderTargetDelete(topLeft);
  C3D_RenderTargetDelete(topRight);

  gfxSet3D(false);
}

void persp_test()
{
  C3D_RenderTarget *top, *tex;
  C3D_Mtx          projTop, projTex;
  C3D_Mtx          modelView;
  C3D_Mtx          texView;
  float            x = 0.0f, y = 0.0f, z = -2.0f,
                   old_x = x, old_y = y, old_z = z;
  float            angle = 0.0f;

  top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(top, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

  tex = C3D_RenderTargetCreate(512, 256, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(tex, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_TexSetFilter(&tex->renderBuf.colorBuf, GPU_LINEAR, GPU_NEAREST);

  Mtx_Persp(&projTex, 60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f, false);

  Mtx_Identity(&modelView);
  Mtx_Translate(&modelView, x, y, z, true);

  Mtx_OrthoTilt(&projTop, -0.5f, 0.5f, -0.5f, 0.5f, 100.0f, -100.0f, false);
  Mtx_Identity(&texView);

  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);
  C3D_FVUnifMtx2x4(GPU_VERTEX_SHADER, uLoc_texView,    &texView);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,   &material);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);

  std::printf("\x1b[2J");
  std::printf("(LEFT/RIGHT) x %.1f\n", x);
  std::printf("(UP/DOWN)    y %.1f\n", y);
  std::printf("(L/R)        z %.1f\n", z);

  while(aptMainLoop())
  {
    gspWaitForVBlank();

    hidScanInput();
    u32 down = hidKeysDown();
    u32 held = hidKeysHeld();
    if(down & (KEY_START|KEY_SELECT))
      break;

    old_x      = x;
    old_y      = y;
    old_z      = z;

    if((down | held) & KEY_LEFT)
      x = clamp(x - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_RIGHT)
      x = clamp(x + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_UP)
      y = clamp(y + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_DOWN)
      y = clamp(y - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_L)
      z = clamp(z + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_R)
      z = clamp(z - 0.1f, -10.0f, 10.0f);

    if((x != old_x) || (y != old_y) || (z != old_z))
    {
      std::printf("\x1b[0;0H");
      std::printf("(LEFT/RIGHT) x %.1f\n", x);
      std::printf("(UP/DOWN)    y %.1f\n", y);
      std::printf("(L/R)        z %.1f\n", z);

      Mtx_Persp(&projTex,  60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f, false);
    }

    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, x, y, z, true);
    Mtx_RotateY(&modelView, angle*M_TAU/360.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    angle += 1.0f;
    if(angle >= 360.0f)
      angle = 0.0f;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C3D_TexBind(0, &texture[0].tex);

    C3D_FrameDrawOn(tex);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projTex);
    C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projTop);
    Mtx_Identity(&modelView);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    C3D_FrameDrawOn(top);
    C3D_TexBind(0, &tex->renderBuf.colorBuf);
    C3D_DrawArrays(GPU_TRIANGLES, 0, 6);
    C3D_FrameEnd(0);
  }

  C3D_RenderTargetDelete(top);
  C3D_RenderTargetDelete(tex);
}

void stereo_test()
{
  C3D_RenderTarget     *topLeft, *topRight, *texLeft, *texRight;
  C3D_Mtx              projLeft, projRight, proj;
  C3D_Mtx              modelView;
  C3D_Mtx              texView;
  float                x = 0.0f, y = 0.0f, z = -2.0f,
                       old_x = x, old_y = y, old_z = z;
  float                iod = osGet3DSliderState(), old_iod = iod;
  float                focLen = 2.0f, old_focLen = focLen;
  float                angle = 0.0f;

  gfxSet3D(true);

  topLeft  = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  topRight = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(topLeft,  C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetClear(topRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetOutput(topLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
  C3D_RenderTargetSetOutput(topRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);

  texLeft  = C3D_RenderTargetCreate(512, 256, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  texRight = C3D_RenderTargetCreate(512, 256, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(texLeft,  C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetClear(texRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_TexSetFilter(&texLeft->renderBuf.colorBuf, GPU_LINEAR, GPU_NEAREST);
  C3D_TexSetFilter(&texRight->renderBuf.colorBuf, GPU_LINEAR, GPU_NEAREST);

  Mtx_PerspStereo(&projLeft,  60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f, -iod, focLen, false);
  Mtx_PerspStereo(&projRight, 60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f,  iod, focLen, false);

  Mtx_Identity(&modelView);
  Mtx_Translate(&modelView, x, y, z, true);

  Mtx_OrthoTilt(&proj, -0.5f, 0.5f, -0.5f, 0.5f, 100.0f, -100.0f, false);
  Mtx_Identity(&texView);

  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);
  C3D_FVUnifMtx2x4(GPU_VERTEX_SHADER, uLoc_texView,    &texView);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,   &material);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);

  std::printf("\x1b[2J");
  std::printf("(LEFT/RIGHT) x      %.1f\n", x);
  std::printf("(UP/DOWN)    y      %.1f\n", y);
  std::printf("(L/R)        z      %.1f\n", z);
  std::printf("(Y/A)        focLen %.1f\n", focLen);
  std::printf("(3D Slider)  iod    %.1f\n", iod);

  while(aptMainLoop())
  {
    gspWaitForVBlank();

    hidScanInput();
    u32 down = hidKeysDown();
    u32 held = hidKeysHeld();
    if(down & (KEY_START|KEY_SELECT))
      break;

    old_x      = x;
    old_y      = y;
    old_z      = z;
    old_focLen = focLen;
    old_iod    = iod;

    if((down | held) & KEY_LEFT)
      x = clamp(x - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_RIGHT)
      x = clamp(x + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_UP)
      y = clamp(y + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_DOWN)
      y = clamp(y - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_L)
      z = clamp(z + 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_R)
      z = clamp(z - 0.1f, -10.0f, 10.0f);
    if((down | held) & KEY_Y)
      focLen = clamp(focLen - 0.1f, 0.1f, 10.0f);
    if((down | held) & KEY_A)
      focLen = clamp(focLen + 0.1f, 0.1f, 10.0f);

    iod = osGet3DSliderState();

    if((x != old_x) || (y != old_y) || (z != old_z)
    || (focLen != old_focLen) || (iod != old_iod))
    {
      std::printf("\x1b[0;0H");
      std::printf("(LEFT/RIGHT) x      %.1f\n", x);
      std::printf("(UP/DOWN)    y      %.1f\n", y);
      std::printf("(L/R)        z      %.1f\n", z);
      std::printf("(Y/A)        focLen %.1f\n", focLen);
      std::printf("(3D Slider)  iod    %.1f\n", iod);

      Mtx_PerspStereo(&projLeft,  60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f, -iod, focLen, false);
      Mtx_PerspStereo(&projRight, 60.0f*M_TAU/360.0f, 400.0f/240.0f, 1.0f, 10.0f,  iod, focLen, false);
    }

    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, x, y, z, true);
    Mtx_RotateY(&modelView, angle*M_TAU/360.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    angle += 1.0f;
    if(angle >= 360.0f)
      angle = 0.0f;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C3D_TexBind(0, &texture[0].tex);

    C3D_FrameDrawOn(texLeft);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projLeft);
    C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);

    if(iod > 0.0f)
    {
      C3D_FrameDrawOn(texRight);
      C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projRight);
      C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);
    }

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &proj);
    Mtx_Identity(&modelView);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    C3D_FrameDrawOn(topLeft);
    C3D_TexBind(0, &texLeft->renderBuf.colorBuf);
    C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);
    if(iod > 0.0f)
    {
      C3D_FrameDrawOn(topRight);
      C3D_TexBind(0, &texRight->renderBuf.colorBuf);
      C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);
    }
    C3D_FrameEnd(0);
  }

  C3D_RenderTargetDelete(topLeft);
  C3D_RenderTargetDelete(topRight);
  C3D_RenderTargetDelete(texLeft);
  C3D_RenderTargetDelete(texRight);

  gfxSet3D(false);
}

void ortho_test()
{
  C3D_RenderTarget *top, *tex;
  C3D_Mtx          projTop, projTex;
  C3D_Mtx          modelView;
  C3D_Mtx          texView;
  float            x = 0.0f, y = 0.0f, z = -2.0f,
                   old_x = x, old_y = y, old_z = z;
  float            angle = 0.0f;

  top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(top, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

  tex = C3D_RenderTargetCreate(512, 256, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetClear(tex, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
  C3D_TexSetFilter(&tex->renderBuf.colorBuf, GPU_LINEAR, GPU_NEAREST);

  Mtx_Ortho(&projTex, 0.0f, 400.0f, 0.0f, 240.0f, 100.0f, -100.0f, false);

  Mtx_Identity(&modelView);
  Mtx_Translate(&modelView, x, y, z, true);

  Mtx_OrthoTilt(&projTop, -0.5f, 0.5f, -0.5f, 0.5f, 100.0f, -100.0f, false);
  Mtx_Identity(&texView);

  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,  &modelView);
  C3D_FVUnifMtx2x4(GPU_VERTEX_SHADER, uLoc_texView,    &texView);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,   &material);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
  C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);

  std::printf("\x1b[2J");
  std::printf("(LEFT/RIGHT) x %.1f\n", x);
  std::printf("(UP/DOWN)    y %.1f\n", y);
  std::printf("(L/R)        z %.1f\n", z);

  while(aptMainLoop())
  {
    gspWaitForVBlank();

    hidScanInput();
    u32 down = hidKeysDown();
    u32 held = hidKeysHeld();
    if(down & (KEY_START|KEY_SELECT))
      break;

    old_x      = x;
    old_y      = y;
    old_z      = z;

    if((down | held) & KEY_LEFT)
      x = clamp(x - 1.0f, 0.0f, 400.0f);
    if((down | held) & KEY_RIGHT)
      x = clamp(x + 1.0f, 0.0f, 400.0f);
    if((down | held) & KEY_UP)
      y = clamp(y + 1.0f, 0.0f, 240.0f);
    if((down | held) & KEY_DOWN)
      y = clamp(y - 1.0f, 0.0f, 240.0f);
    if((down | held) & KEY_L)
      z = clamp(z + 1.0f, -100.0f, 100.0f);
    if((down | held) & KEY_R)
      z = clamp(z - 1.0f, -100.0f, 100.0f);

    if((x != old_x) || (y != old_y) || (z != old_z))
    {
      std::printf("\x1b[0;0H");
      std::printf("(LEFT/RIGHT) x %.1f\n", x);
      std::printf("(UP/DOWN)    y %.1f\n", y);
      std::printf("(L/R)        z %.1f\n", z);
    }

    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, x, y, z, true);
    Mtx_Scale(&modelView, 64.0f, 64.0f, 64.0f);
    Mtx_RotateY(&modelView, angle*M_TAU/360.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    angle += 1.0f;
    if(angle >= 360.0f)
      angle = 0.0f;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C3D_TexBind(0, &texture[0].tex);

    C3D_FrameDrawOn(tex);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projTex);
    C3D_DrawArrays(GPU_TRIANGLES, 0, attribute_list_count);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projTop);
    Mtx_Identity(&modelView);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

    C3D_FrameDrawOn(top);
    C3D_TexBind(0, &tex->renderBuf.colorBuf);
    C3D_DrawArrays(GPU_TRIANGLES, 0, 6);
    C3D_FrameEnd(0);
  }

  C3D_RenderTargetDelete(top);
  C3D_RenderTargetDelete(tex);
}

typedef struct
{
  const char *name;
  void (*test)();
} test_t;

test_t tests[] =
{
  { "Mtx_PerspTilt",       persp_tilt_test,  },
  { "Mtx_OrthoTilt",       ortho_tilt_test,  },
  { "Mtx_PerspStereoTilt", stereo_tilt_test, },
  { "Mtx_Persp",           persp_test,       },
  { "Mtx_PerspStereo",     stereo_test,      },
  { "Mtx_Ortho",           ortho_test,       },
};

const size_t num_tests = sizeof(tests)/sizeof(tests[0]);

void print_choices(size_t choice)
{
  std::printf("\x1b[2J");
  for(size_t i = 0; i < num_tests; ++i)
    std::printf("\x1b[%zu;0H%c%s", i, i == choice ? '*' : ' ', tests[i].name);
}

}

int main(int argc, char *argv[])
{
  size_t          choice = 0;
  shaderProgram_s program;
  DVLB_s          *vsh_dvlb;

  romfsInit();
  gfxInitDefault();
  gfxSet3D(false);
  consoleInit(GFX_BOTTOM, nullptr);
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

  shaderProgramInit(&program);
  vsh_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
  shaderProgramSetVsh(&program, &vsh_dvlb->DVLE[0]);
  C3D_BindProgram(&program);

  sceneInit(&program);

  print_choices(choice);
  while(aptMainLoop())
  { 
    gfxFlushBuffers();
    gspWaitForVBlank();
    gfxSwapBuffers();

    hidScanInput();
    u32 down = hidKeysDown();

    if(down & KEY_UP)
    {
      choice = (choice + num_tests - 1) % num_tests;
      print_choices(choice);
    }
    else if(down & KEY_DOWN)
    {
      choice = (choice + 1) % num_tests;
      print_choices(choice);
    }
    else if(down & KEY_A)
    {
      tests[choice].test();
      print_choices(choice);
    }
    else if(down & KEY_B)
      break;
  }

  sceneExit();

  shaderProgramFree(&program);
  DVLB_Free(vsh_dvlb);
  C3D_Fini();
  gfxExit();
  romfsExit();

  return 0;
}
