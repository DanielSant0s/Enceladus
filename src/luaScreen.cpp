
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
	if (argc == 1)color = luaL_checkinteger(L, 1);
	else color = 0x000000FF;
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

static int lua_splash(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
	displaySplashScreen();
	return 0;
}

//Register our Screen Functions
static const luaL_Reg Screen_functions[] = {
  {"clear",            lua_clear},
  {"flip",              lua_flip},
  //{"getPixel",        lua_getP},
  {"waitVblankStart", lua_vblank},
  {"showSplash",	  lua_splash},
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
	int a = 255;
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
  {"getR",  lua_getR},
  {"getG",  lua_getG},
  {"getB",  lua_getB},
  {"getA",  lua_getA},
  {0, 0}
};

void luaScreen_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Screen_functions, 0);
	lua_setglobal(L, "Screen");
	lua_newtable(L);
	luaL_setfuncs(L, Color_functions, 0);
	lua_setglobal(L, "Color");
}