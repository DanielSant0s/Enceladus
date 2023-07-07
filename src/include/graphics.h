#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <kernel.h>
#include <math3d.h>


/* ps2sdk */
#include "graph.h"

/* GL */
#include "GL/ps2gl.h"

/* ps2stuff */
#include "ps2s/core.h"
#include "ps2s/displayenv.h"
#include "ps2s/drawenv.h"
#include "ps2s/gs.h"
#include "ps2s/timer.h"

/* ps2gl */
#include "ps2gl/debug.h"
#include "ps2gl/displaycontext.h"
#include "ps2gl/drawcontext.h"
#include "ps2gl/glcontext.h"

struct gl_texture_t
{
  u32 width;
  u32 height;

  u32 format;
  s32 internalFormat;
  u32 id;

  u32 mag_filter;
  u32 min_filter;

  u8 clut_size;
  u32* clut;
  u8 *texels;
};

/// GSKit CLUT base struct. This should've been in gsKit from the start :)
typedef struct
{
    u8 internalFormat;       ///< Pixel Storage Method (Color Format)
    u8 clut_size;   ///< CLUT Pixel Storage Method (Color Format)
    u32 *clut;    ///< EE CLUT Memory Pointer
} GSCLUT;

typedef struct
{
    float x, y;
    float u, v;
} rm_tx_coord_t;

typedef struct
{
    rm_tx_coord_t ul;
    rm_tx_coord_t br;
    u64 color;
    gl_texture_t* txt;
} rm_quad_t;

struct materialRangeInfo {
    uint32_t start;
    uint32_t end;
    uint32_t materialIndex;
};

struct material {
    float Ka[4];  /* Ambient */
    float Kd[4];  /* Diffuse */
    float Ks[4];  /* Specular */
    float Ke[4];  /* Emission */
    float Kt[4];  /* Transmittance */
    float Ns;     /* Shininess */
    float Ni;     /* Index of refraction */
    float Tf[4];  /* Transmission filter */
    float d;      /* Disolve (alpha) */
    int   illum;  /* Illumination model */

    gl_texture_t* texture;
};

struct model {
	uint32_t indexCount;
    float* positions;
	float* texcoords;
	float* normals;
    float* colours;
    float bounding_box[24];

    gl_texture_t* texture;

    material* materials;
	int materialCount;

    materialRangeInfo* ranges;
    int rangeCount;
};



typedef u32 Color;
#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))

extern void initGraphics();

extern void clearScreen(Color color);

extern void flipScreen();

extern void graphicWaitVblankStart();

extern void setVSync(bool vsync_flag);

extern void gsKit_clear_screens();

extern void *getvoid();

extern int GetInterlacedFrameMode();

extern int getFreeVRAM();

extern float FPSCounter(int interval);

extern void setVideoMode(s16 mode, int width, int height, int psm, s16 interlace, s16 field, bool zbuffering, int psmz);

extern gl_texture_t* load_image(const char* path, bool delayed);

extern void drawImage(gl_texture_t* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color);
extern void drawImageRotate(gl_texture_t* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, float angle, Color color);

extern void drawPixel(float x, float y, Color color);
extern void drawLine(float x, float y, float x2, float y2, Color color);
extern void drawRect(float x, float y, int width, int height, Color color);
extern void drawCircle(float x, float y, float radius, u64 color, u8 filled);
extern void drawTriangle(float x, float y, float x2, float y2, float x3, float y3, Color color);
extern void drawTriangle_gouraud(float x, float y, float x2, float y2, float x3, float y3, Color color, Color color2, Color color3);
extern void drawQuad(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color);
extern void drawQuad_gouraud(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color, Color color2, Color color3, Color color4);

extern void InvalidateTexture(gl_texture_t *txt);

extern void UnloadTexture(gl_texture_t *txt);

extern void fntDrawQuad(rm_quad_t *q);

extern void* loadFont(const char* path);

extern void printFontText(void* font, const char* text, float x, float y, float scale, Color color);

extern void unloadFont(void* font);

extern void loadFontM();

extern void printFontMText(const char* text, float x, float y, float scale, Color color);

extern void unloadFontM();

extern void init3D();

extern void setCameraPosition(float x, float y, float z);

extern void setCameraRotation(float x, float y, float z);

extern int createLight();

extern void setLightAttribute(int lightid, float a, float b, float c, float d, int attr);

extern model* loadOBJ(const char* path, gl_texture_t* text);

extern void drawOBJ(model* m, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z);

extern void draw_bbox(model* m, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, Color color);

void viewport_3d(float fov, float aspect, float nearClip, float farClip);

void viewport_2d(float sx, float ex, float sy, float ey);

#endif
