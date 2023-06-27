/*
  Copyright 2010, Volca
  Licenced under Academic Free License version 3.0
  Review OpenUsbLd README & LICENSE files for further details.
*/

#include <stdio.h>
#include <malloc.h>
#include <cstring>
#include "include/atlas.h"
#include "include/graphics.h"

static inline struct atlas_allocation_t *allocNew(int x, int y, size_t width, size_t height)
{
    struct atlas_allocation_t *al = (struct atlas_allocation_t *)malloc(sizeof(struct atlas_allocation_t));

    al->x = x;
    al->y = y;
    al->w = width;
    al->h = height;

    al->leaf1 = NULL;
    al->leaf2 = NULL;

    return al;
}

static inline void allocFree(struct atlas_allocation_t *alloc)
{
    if (!alloc)
        return;

    // NOTE: If used on a tree component it
    // would have to be ensured pointers to freed
    // allocation are fixed

    allocFree(alloc->leaf1); // safe
    allocFree(alloc->leaf2);

    free(alloc);
}

#define ALLOC_FITS(alloc, width, height) \
    ((alloc->w >= width) && (alloc->h >= height))

#define ALLOC_ISFREE(alloc) \
    ((!alloc->leaf1) && (!alloc->leaf2))


static inline struct atlas_allocation_t *allocPlace(struct atlas_allocation_t *alloc, size_t width, size_t height)
{
    // do we fit?
    if (!ALLOC_FITS(alloc, width, height))
        return NULL;

    if (ALLOC_ISFREE(alloc)) {
        // extra space
        size_t dx = alloc->w - width;
        size_t dy = alloc->h - height;

        // make the wider piece also longer - less wasted space
        if (dx < dy) {
            alloc->leaf1 = allocNew(alloc->x + width, alloc->y, dx, height);
            alloc->leaf2 = allocNew(alloc->x, alloc->y + height, alloc->w, dy);
        } else {
            alloc->leaf1 = allocNew(alloc->x, alloc->y + height, width, dy);
            alloc->leaf2 = allocNew(alloc->x + width, alloc->y, dx, alloc->h);
        }

        return alloc;
    } else {
        // already occupied. Try children
        struct atlas_allocation_t *p = allocPlace(alloc->leaf1, width, height);
        if (p)
            return p;

        p = allocPlace(alloc->leaf2, width, height);
        if (p)
            return p;
    }

    return NULL;
}

atlas_t *atlasNew(size_t width, size_t height, u8 psm)
{
    atlas_t *atlas = (atlas_t *)malloc(sizeof(atlas_t));

    atlas->allocation = allocNew(0, 0, width, height);

    atlas->surface.width = width;
    atlas->surface.height = height;

    size_t txtsize = width* height;
    atlas->surface.internalFormat = psm;
    atlas->surface.texels = (u8 *)memalign(128, txtsize);

    // defaults to no clut
    atlas->surface.clut_size = 0;
    atlas->surface.clut = NULL;

    // zero out the atlas surface
    memset(atlas->surface.texels, 0x0, txtsize);

    return atlas;
}

void atlasFree(atlas_t *atlas)
{
    if (!atlas)
        return;

    allocFree(atlas->allocation);
    atlas->allocation = NULL;

    //UnloadTexture(&atlas->surface);
    //free(atlas->surface.Mem);
    //atlas->surface.Mem = NULL;

    free(atlas);
}

// copies the data into atlas
static void atlasCopyData(atlas_t *atlas, struct atlas_allocation_t *al, size_t width, size_t height, const void *surface)
{
    int y;
    size_t ps = atlas->surface.internalFormat;

    if (!ps)
        return;

    const char *src = (const char *)surface;
    char *data = (char *)atlas->surface.texels;

    // advance the pointer to the atlas position start (first pixel)
    data += ps * (al->y * atlas->allocation->w + al->x);

    size_t rowsize = width * ps;

    for (y = 0; y < height; ++y) {
        memcpy(data, src, rowsize);
        data += ps * atlas->allocation->w;
        src += ps * width;
    }
}

struct atlas_allocation_t *atlasPlace(atlas_t *atlas, size_t width, size_t height, const void *surface)
{
    if (!surface)
        return NULL;

    struct atlas_allocation_t *al = allocPlace(atlas->allocation, width + 1, height + 1);

    if (!al)
        return NULL;

    atlasCopyData(atlas, al, width, height, surface);

        // Gera uma nova textura OpenGL
    glGenTextures(1, &atlas->surface.id);
    
    // Vincula a textura
    glBindTexture(GL_TEXTURE_2D, atlas->surface.id);
    
    // Define os par�metros de filtragem da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Define os par�metros de repeti��o da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Carrega os dados da imagem na textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX, atlas->surface.width, atlas->surface.height, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, atlas->surface.texels);


    //InvalidateTexture(&atlas->surface);

    return al;
}
