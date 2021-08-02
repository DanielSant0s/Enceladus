#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <kernel.h>

#include <gsKit.h>
#include <dmaKit.h>

#include <gsToolkit.h>
#include <gsInline.h>
#include <math3d.h>

#include <draw.h>
#include <draw3d.h>

/// GSKit CLUT base struct. This should've been in gsKit from the start :)
typedef struct
{
    u8 PSM;       ///< Pixel Storage Method (Color Format)
    u8 ClutPSM;   ///< CLUT Pixel Storage Method (Color Format)
    u32 *Clut;    ///< EE CLUT Memory Pointer
    u32 VramClut; ///< GS VRAM CLUT Memory Pointer
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
    GSTEXTURE *txt;
} rm_quad_t;



typedef u32 Color;
#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))

extern GSTEXTURE* luaP_loadpng(const char *Path);

extern GSTEXTURE* luaP_loadbmp(const char *Path);

extern GSTEXTURE* luaP_loadjpeg(const char *Path);

extern GSTEXTURE* luaP_loadrawimg(const char *Path);

extern void drawImage(GSTEXTURE* source, float dx, float dy, int alpha, int width, int height, int lx, int ly);

extern void drawPixel(float x, float y, Color color);

extern void drawLine(float x, float y, float x2, float y2, Color color);

extern void drawRect(float x, float y, int width, int height, Color color);

extern void drawCircle(float x, float y, float radius, u64 color, u8 filled);

extern void drawTriangle(float x, float y, float x2, float y2, float x3, float y3, Color color);

extern void drawTriangle_gouraud(float x, float y, float x2, float y2, float x3, float y3, Color color, Color color2, Color color3);

extern void drawQuad(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color);

extern void drawQuad_gouraud(float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Color color, Color color2, Color color3, Color color4);

extern void InvalidateTexture(GSTEXTURE *txt);

extern void UnloadTexture(GSTEXTURE *txt);

extern int GetInterlacedFrameMode();

extern void fntDrawQuad(rm_quad_t *q);

/*
 * Initialize all pixels of the screen with a color.
 *
 * @param color - new color for the pixels
 */
extern void clearScreen();


/**
 * Exchange display buffer and drawing buffer.
 */
extern void flipScreen();

/**
 * Initialize the graphics.
 */
extern void initGraphics();

/**
 * Disable graphics, used for debug text output.
 */
extern void disableGraphics();

extern void graphicWaitVblankStart();

extern void printFontMText(char* text, float x, float y, float scale, Color color);

extern void loadFontM();

extern void unloadFontM();

extern void init3D();

extern int render();

extern void displaySplashScreen();

void gsKit_clear_screens();

extern int getFreeVRAM();

extern int FPSCounter(clock_t prevtime, clock_t curtime);

#endif
