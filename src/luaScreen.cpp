
#include "include/luaplayer.h"
#include "include/graphics.h"

static int lua_flip(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments.");
	flipScreen();
	return 0;
}

static int lua_clear(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 0 && argc != 1) return luaL_error(L, "wrong number of arguments.");
	Color color; 
	if (argc == 1) color = luaL_checkinteger(L, 1);
	else color = GS_SETREG_RGBAQ(0x00, 0x00, 0x00, 0x80, 0x00);
	clearScreen(color);
	return 0;
}

/*
static int lua_getP(lua_State *L) {
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if (argc != 2) return luaL_error(L, "wrong number of arguments");
	#endif
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	uint32_t* buffer = (uint32_t*)vita2d_get_current_fb();
	lua_pushinteger(L,buffer[x + y * 1024]);
	return 1;
}
*/

static int lua_vblank(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
	graphicWaitVblankStart();
	return 0;
}


static int lua_getFreeVRAM(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "no arguments expected.");
	
	int result = getFreeVRAM();
	lua_pushinteger(L, (uint32_t)(result));
	return 1;
}


static int lua_getFPS(lua_State *L)
{
	if (lua_gettop(L) != 1) return luaL_error(L, "no arguments expected.");
	int result = FPSCounter(luaL_checkinteger(L, 1));
	lua_pushinteger(L, (uint32_t)(result));
	return 1;
}

static int lua_setvmode(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 6 && argc != 8) return luaL_error(L, "wrong number of arguments");
	s16 mode = (s16)luaL_checkinteger(L, 1);
	int width = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	int psm = luaL_checkinteger(L, 4);
	s16 interlace = (s16)luaL_checkinteger(L, 5);
	s16 field = (s16)luaL_checkinteger(L, 6);
	bool zbuffering = false;
	int psmz = GS_PSMZ_16S;
	if(argc == 8){
		zbuffering = lua_toboolean(L, 7);
		psmz = luaL_checkinteger(L, 8);

	}
	setVideoMode(mode, width, height, psm, interlace, field, zbuffering, psmz);
	return 0;
}

static int lua_getvmode(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments");

	GSGLOBAL *gsGlobal = getGSGLOBAL();

	lua_newtable(L);
    lua_pushstring(L, "mode");
    lua_pushinteger(L, gsGlobal->Mode);
    lua_settable(L, -3);

    lua_pushstring(L, "interlace");
    lua_pushinteger(L, gsGlobal->Interlace);
    lua_settable(L, -3);

    lua_pushstring(L, "field");
    lua_pushinteger(L, gsGlobal->Field);
    lua_settable(L, -3);

    lua_pushstring(L, "dithering");
    lua_pushinteger(L, gsGlobal->Dithering);
    lua_settable(L, -3);

	lua_pushstring(L, "doubleBuffering");
    lua_pushinteger(L, gsGlobal->DoubleBuffering);
    lua_settable(L, -3);

	lua_pushstring(L, "zBuffering");
    lua_pushinteger(L, gsGlobal->ZBuffering);
    lua_settable(L, -3);

	lua_pushstring(L, "width");
    lua_pushinteger(L, gsGlobal->Width);
    lua_settable(L, -3);

	lua_pushstring(L, "height");
    lua_pushinteger(L, gsGlobal->Height);
    lua_settable(L, -3);

	lua_pushstring(L, "aspect");
    lua_pushinteger(L, gsGlobal->Aspect);
    lua_settable(L, -3);
	
	lua_pushstring(L, "colorMode");
    lua_pushinteger(L, gsGlobal->PSM);
    lua_settable(L, -3);

	lua_pushstring(L, "zColorMode");
    lua_pushinteger(L, gsGlobal->PSMZ);
    lua_settable(L, -3);

	return 1;
}

//Register our Screen Functions
static const luaL_Reg Screen_functions[] = {
	{"clear",              lua_clear},
	{"flip",              	lua_flip},
	{"getFreeVRAM",  lua_getFreeVRAM},
	{"getFPS",            lua_getFPS},
  //{"getPixel",        	lua_getP},
	{"waitVblankStart",	  lua_vblank},
	{"getMode",			lua_getvmode},
	{"setMode",			lua_setvmode},
	{0, 0}
};

static int lua_color(lua_State *L) {
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if ((argc != 3) && (argc != 4)) return luaL_error(L, "wrong number of arguments");
	#endif
	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);
	int a = 0x80;
	if (argc==4) a = luaL_checkinteger(L, 4);
	int color = r | (g << 8) | (b << 16) | (a << 24);
	lua_pushinteger(L,color);
	return 1;
}

static int lua_getR(lua_State *L) {
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	int color = luaL_checkinteger(L, 1);
	int colour = color & 0xFF;
	lua_pushinteger(L,colour);
	return 1;
}

static int lua_getG(lua_State *L) {
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	int color = luaL_checkinteger(L, 1);
	int colour = (color >> 8) & 0xFF;
	lua_pushinteger(L,colour);
	return 1;
}

static int lua_getB(lua_State *L) {
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	int color = luaL_checkinteger(L, 1);
	int colour = (color >> 16) & 0xFF;
	lua_pushinteger(L,colour);
	return 1;
}

static int lua_getA(lua_State *L) {
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	int color = luaL_checkinteger(L, 1);
	int colour = (color >> 24) & 0xFF;
	lua_pushinteger(L,colour);
	return 1;
}

//Register our Color Functions
static const luaL_Reg Color_functions[] = {
  {"new",   lua_color},
  {"getR",   lua_getR},
  {"getG",   lua_getG},
  {"getB",   lua_getB},
  {"getA",   lua_getA},
  {0, 0}
};

void luaScreen_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Screen_functions, 0);
	lua_setglobal(L, "Screen");
	lua_newtable(L);
	luaL_setfuncs(L, Color_functions, 0);
	lua_setglobal(L, "Color");

	lua_pushinteger(L, GS_MODE_NTSC);
	lua_setglobal (L, "NTSC");

	lua_pushinteger(L, GS_MODE_DTV_480P);
	lua_setglobal (L, "_480p");

	lua_pushinteger(L, GS_MODE_PAL);
	lua_setglobal (L, "PAL");

	lua_pushinteger(L, GS_MODE_DTV_576P);
	lua_setglobal (L, "_576p");

	lua_pushinteger(L, GS_MODE_DTV_720P);
	lua_setglobal (L, "_720p");

	lua_pushinteger(L, GS_MODE_DTV_1080I);
	lua_setglobal (L, "_1080i");

	lua_pushinteger(L, GS_INTERLACED);
	lua_setglobal (L, "INTERLACED");

	lua_pushinteger(L, GS_NONINTERLACED);
	lua_setglobal (L, "NONINTERLACED");

	lua_pushinteger(L, GS_FIELD);
	lua_setglobal (L, "FIELD");

	lua_pushinteger(L, GS_FRAME);
	lua_setglobal (L, "FRAME");

	lua_pushinteger(L, GS_PSM_CT32);
	lua_setglobal (L, "CT32");

	lua_pushinteger(L, GS_PSM_CT24);
	lua_setglobal (L, "CT24");

	lua_pushinteger(L, GS_PSM_CT16);
	lua_setglobal (L, "CT16");

	lua_pushinteger(L, GS_PSM_CT16S);
	lua_setglobal (L, "CT16S");

	lua_pushinteger(L, GS_PSMZ_32);
	lua_setglobal (L, "Z32");

	lua_pushinteger(L, GS_PSMZ_24);
	lua_setglobal (L, "Z24");

	lua_pushinteger(L, GS_PSMZ_16);
	lua_setglobal (L, "Z16");

	lua_pushinteger(L, GS_PSMZ_16S);
	lua_setglobal (L, "Z16S");

}
