#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "include/luaplayer.h"
#include "include/graphics.h"
#include "include/fntsys.h"

static int lua_fontload(lua_State *L){
	if (lua_gettop(L) != 1) return luaL_error(L, "wrong number of arguments"); 
	const char* path = luaL_checkstring(L, 1);
	GSFONT* font = loadFont(path);
	if (font == NULL) return luaL_error(L, "Error loading font (invalid magic).");
	lua_pushinteger(L, (uint32_t)(font));
	return 1;
}

static int lua_print(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 5 && argc != 6) return luaL_error(L, "wrong number of arguments");
	GSFONT* font = (GSFONT*)luaL_checkinteger(L, 1);
    float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
    float scale =  luaL_checknumber(L, 4);
    char* text = (char*)(luaL_checkstring(L, 5));
	Color color = 0x80808080;
	if (argc == 6) color = luaL_checkinteger(L, 6);
	printFontText(font, text, x, y, scale, color);
	return 0;
}

static int lua_fontunload(lua_State *L){
	int argc = lua_gettop(L); 
	if (argc != 1) return luaL_error(L, "wrong number of arguments"); 
	GSFONT* font = (GSFONT*)luaL_checkinteger(L, 1);
	unloadFont(font);
	return 0;
}

static int lua_ftinit(lua_State *L){
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments"); 
	fntInit();
	return 0;
}

static int lua_ftload(lua_State *L){
	if (lua_gettop(L) != 1) return luaL_error(L, "wrong number of arguments"); 
	const char* fontpath = luaL_checkstring(L, 1);
	int fntHandle = fntLoadFile(fontpath);
    lua_pushinteger(L, fntHandle);
	return 1;
}

static int lua_ftSetPixelSize(lua_State *L) {
	if (lua_gettop(L) != 3) return luaL_error(L, "wrong number of arguments"); 
	int fontid = luaL_checkinteger(L, 1);
	int width = luaL_checkinteger(L, 2); 
	int height = luaL_checkinteger(L, 3); 
	fntSetPixelSize(fontid, width, height);
	return 0;
}


static int lua_ftSetCharSize(lua_State *L) {
	if (lua_gettop(L) != 3) return luaL_error(L, "wrong number of arguments"); 
	int fontid = luaL_checkinteger(L, 1);
	int width = luaL_checkinteger(L, 2); 
	int height = luaL_checkinteger(L, 3); 
	fntSetCharSize(fontid, width, height);
	return 0;
}

static int lua_ftprint(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 7 && argc != 8) return luaL_error(L, "wrong number of arguments");
	int fontid = luaL_checkinteger(L, 1);
    int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int alignment = luaL_checkinteger(L, 4);
	int width = luaL_checkinteger(L, 5); 
	int height = luaL_checkinteger(L, 6); 
    const char* text = luaL_checkstring(L, 7);
	Color color = 0x80808080;
	if (argc == 8) color = luaL_checkinteger(L, 8);
	fntRenderString(fontid, x, y, alignment, width, height, text, color);
	return 0;
}

static int lua_ftunload(lua_State *L){
	int argc = lua_gettop(L); 
	if (argc != 1) return luaL_error(L, "wrong number of arguments"); 
	int fontid = luaL_checkinteger(L, 1);
	fntRelease(fontid);
	return 0;
}


static int lua_ftend(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
	fntEnd();
	return 0;
}

static int lua_fmload(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
	loadFontM();
	return 0;
}

static int lua_fmprint(lua_State *L) {
	int argc = lua_gettop(L);
	//if (argc != 3) return luaL_error(L, "wrong number of arguments");
    float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
    float scale =  luaL_checknumber(L, 3);
    char* text = (char*)(luaL_checkstring(L, 4));
	Color color = 0x80808080;
	if (argc == 5) color =  luaL_checkinteger(L, 5);
	printFontMText(text, x, y, scale, color);
	return 0;
}

static int lua_fmunload(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
	unloadFontM();
	return 0;
}


static const luaL_Reg Font_functions[] = {
	//FreeType functions
	{"ftInit",            		  lua_ftinit},
	{"ftLoad",            		  lua_ftload},
	{"ftSetPixelSize",    lua_ftSetPixelSize},
	{"ftSetCharSize", 	   lua_ftSetCharSize},
	{"ftPrint",         		 lua_ftprint},
	{"ftUnload",           		lua_ftunload},
	{"ftEnd",           	       lua_ftend},
	//gsFont functions
  	{"load",	                lua_fontload},
  	{"print",                      lua_print},
    {"unload",                lua_fontunload},
	//gsFontM functions
  	{"fmLoad",            		  lua_fmload}, 
	{"fmPrint",           		 lua_fmprint}, 
	{"fmUnload",         	    lua_fmunload}, 
  {0, 0}
};

static int lua_loadimg(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
    lua_gc(L, LUA_GCCOLLECT, 0);
	const char* text = luaL_checkstring(L, 1);
	int file = open(text, O_RDONLY, 0777);
	uint16_t magic;
	read(file, &magic, 2);
	close(file);
	GSTEXTURE* image = NULL;
	if (magic == 0x4D42) image = luaP_loadbmp(text);
	else if (magic == 0xD8FF) image = luaP_loadjpeg(text, false);
	else if (magic == 0x5089) image = luaP_loadpng(text);
	else return luaL_error(L, "Error loading image (invalid magic).");
	lua_pushinteger(L, (uint32_t)(image));
	return 1;
}

static int lua_drawimg(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 3 && argc != 4) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	Color color = 0x80808080;
	if (argc == 4) color = (Color)luaL_checknumber(L, 4);
	float width = source->Width;
	float height = source->Height;
	float startx = 0.0f;
	float starty = 0.0f;
	float endx = source->Width;
	float endy = source->Height;

	drawImage(source, x, y, width, height, startx, starty, endx, endy, color);

	return 0;
}

static int lua_drawimg_rotate(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 4 && argc != 5) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float radius = luaL_checknumber(L, 4);
	Color color = 0x80808080;
	if (argc == 5) color = (Color)luaL_checknumber(L, 5);
	float width = source->Width;
	float height = source->Height;
	float startx = 0.0f;
	float starty = 0.0f;
	float endx = source->Width;
	float endy = source->Height;

	drawImageRotate(source, x, y, width, height, startx, starty, endx, endy, radius, color);

	return 0;
}

static int lua_drawimg_scale(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 5 && argc != 6) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float width = luaL_checknumber(L, 4);
	float height = luaL_checknumber(L, 5);
	Color color = 0x80808080;
	if (argc == 6) color = (Color)luaL_checknumber(L, 6);
	float startx = 0.0f;
	float starty = 0.0f;
	float endx = source->Width;
	float endy = source->Height;

	drawImage(source, x, y, width, height, startx, starty, endx, endy, color);

	return 0;
}


static int lua_drawimg_part(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 7 && argc != 8) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float startx = (float)luaL_checknumber(L, 4);
	float starty = (float)luaL_checknumber(L, 5);
	float endx = (float)luaL_checknumber(L, 6);
	float endy = (float)luaL_checknumber(L, 7);
	Color color = 0x80808080;
	if (argc == 8) color = (Color)luaL_checknumber(L, 8);
	float width = source->Width;
	float height = source->Height;
	
	drawImage(source, x, y, width, height, startx, starty, endx, endy, color);

	return 0;
}

static int lua_drawimg_full(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 10 && argc != 11) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float startx = (float)luaL_checknumber(L, 4);
	float starty = (float)luaL_checknumber(L, 5);
	float endx = (float)luaL_checknumber(L, 6);
	float endy = (float)luaL_checknumber(L, 7);
	float width = (float)luaL_checknumber(L, 8);
	float height = (float)luaL_checknumber(L, 9);
	float angle = (float)luaL_checknumber(L, 10);
	Color color = 0x80808080;
	if (argc == 11) color = (Color)luaL_checknumber(L, 11);

	drawImageRotate(source, x, y, width, height, startx, starty, endx, endy, angle, color);

	return 0;
}

static int lua_width(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	lua_pushinteger(L, (uint32_t)(source->Width));
	return 1;
}


static int lua_height(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	lua_pushinteger(L, (uint32_t)(source->Height));
	return 1;
}

static int lua_filters(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 2) return luaL_error(L, "wrong number of arguments");
    GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	source->Filter = luaL_checknumber(L, 2);
	return 0;
}


static int lua_rect(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 5) return luaL_error(L, "wrong number of arguments");
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);
    Color color =  luaL_checkinteger(L, 5);

	drawRect(x, y, width, height, color);

	return 0;
}


static int lua_line(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 5) return luaL_error(L, "wrong number of arguments");
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);
    Color color = luaL_checkinteger(L, 5);

	drawLine(x, y, x2, y2, color);

	return 0;
}

static int lua_pixel(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 3) return luaL_error(L, "wrong number of arguments");
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
    Color color =  luaL_checkinteger(L, 3);

	drawPixel(x, y, color);
	return 0;
}

static int lua_triangle(lua_State *L) {
	int argc = lua_gettop(L);

	if (argc != 7 && argc != 9) return luaL_error(L, "wrong number of arguments");

	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);
	float x3 = luaL_checknumber(L, 5);
    float y3 = luaL_checknumber(L, 6);
    Color color =  luaL_checkinteger(L, 7);

	if (argc == 7) drawTriangle(x, y, x2, y2, x3, y3, color);

	if (argc == 9) {
		Color color2 =  luaL_checkinteger(L, 8);
		Color color3 =  luaL_checkinteger(L, 9);
		drawTriangle_gouraud(x, y, x2, y2, x3, y3, color, color2, color3);
	}
	return 0;
}

static int lua_quad(lua_State *L) {
	int argc = lua_gettop(L);

	if (argc != 9 && argc != 12) return luaL_error(L, "wrong number of arguments");

	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);
	float x3 = luaL_checknumber(L, 5);
    float y3 = luaL_checknumber(L, 6);
	float x4 = luaL_checknumber(L, 7);
    float y4 = luaL_checknumber(L, 8);
    Color color =  luaL_checkinteger(L, 9);

	if (argc == 9) drawQuad(x, y, x2, y2, x3, y3, x4, y4, color);

	if (argc == 12) {
		Color color2 = luaL_checkinteger(L, 10);
		Color color3 = luaL_checkinteger(L, 11);
		Color color4 = luaL_checkinteger(L, 12);
		drawQuad_gouraud(x, y, x2, y2, x3, y3, x4, y4, color, color2, color3, color4);
	}
	return 0;
}

static int lua_circle(lua_State *L) {
	int argc = lua_gettop(L);

	if (argc != 4 && argc != 5) return luaL_error(L, "wrong number of arguments");

	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
    float radius = luaL_checknumber(L, 3);
    Color color = luaL_checkinteger(L, 4);

	bool filling = (argc == 5);
    int filled = filling? luaL_checknumber(L, 5) : 1;

	drawCircle(x, y, radius, color, filled);

	return 0;
}


static int lua_free(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
#endif
	GSTEXTURE* source = (GSTEXTURE*)(luaL_checkinteger(L, 1));
	UnloadTexture(source);
	return 0;
}

//Register our Graphics Functions
static const luaL_Reg Graphics_functions[] = {
  	{"drawPixel",           		   lua_pixel},
  //{"getPixel",            		  lua_gpixel},
  	{"drawRect",           				lua_rect},
  	{"drawLine",            			lua_line},
  	{"drawCircle",         			  lua_circle},
	{"drawTriangle",        		lua_triangle},
	{"drawQuad",        				lua_quad},
    {"loadImage",           		 lua_loadimg},
  //{"loadAnimatedImage",   	   lua_loadanimg},
  //{"getImageFramesNum",   	lua_getnumframes},
  //{"setImageFrame",       		lua_setframe},
    {"drawImage",           		 lua_drawimg},
  	{"drawRotateImage",       lua_drawimg_rotate},
  	{"drawScaleImage",     	   lua_drawimg_scale},
  	{"drawPartialImage",    	lua_drawimg_part},
  	{"drawImageExtended",   	lua_drawimg_full},
  //{"createImage",         	 lua_createimage},
  	{"setImageFilters",     		 lua_filters},
  	{"getImageWidth",       		   lua_width},
  	{"getImageHeight",      		  lua_height},
    {"freeImage",           			lua_free},
  {0, 0}
};


void luaGraphics_init(lua_State *L) {

    lua_newtable(L);
	luaL_setfuncs(L, Graphics_functions, 0);
	lua_setglobal(L, "Graphics");

	lua_newtable(L);
	luaL_setfuncs(L, Font_functions, 0);
	lua_setglobal(L, "Font");

	lua_pushinteger(L, GS_FILTER_NEAREST);
	lua_setglobal (L, "NEAREST");

	lua_pushinteger(L, GS_FILTER_LINEAR);
	lua_setglobal (L, "LINEAR");
}
