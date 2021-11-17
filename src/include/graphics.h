#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <kernel.h>

#include <gsKit.h>
#include <dmaKit.h>

#include <gsToolkit.h>
#include <gsInline.h>

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

extern void initGraphics();

extern void clearScreen(Color color);

extern void flipScreen();

extern void graphicWaitVblankStart();

extern void gsKit_clear_screens();

extern GSGLOBAL *getGSGLOBAL();

extern int GetInterlacedFrameMode();

extern int getFreeVRAM();

extern int FPSCounter(clock_t prevtime, clock_t curtime);

extern void setVideoMode(s16 mode, int width, int height, int psm, s16 interlace, s16 field);

extern GSTEXTURE* luaP_loadpng(const char *Path, bool delayed);

extern GSTEXTURE* luaP_loadbmp(const char *Path, bool delayed);

extern GSTEXTURE* luaP_loadjpeg(const char *Path, bool scale_down, bool delayed);

extern void drawImage(GSTEXTURE* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, Color color);

extern void drawImageRotate(GSTEXTURE* source, float x, float y, float width, float height, float startx, float starty, float endx, float endy, float angle, Color color);

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

extern void fntDrawQuad(rm_quad_t *q);

extern GSFONT* loadFont(const char* path);

extern void printFontText(GSFONT* font, char* text, float x, float y, float scale, Color color);

extern void unloadFont(GSFONT* font);

extern void loadFontM();

extern void printFontMText(char* text, float x, float y, float scale, Color color);

extern void unloadFontM();

extern void displaySplashScreen();

#endif
