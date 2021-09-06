#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <draw3d.h>

#include "include/luaplayer.h"
#include "include/graphics.h"

static int lua_init3D(lua_State *L) {
	int argc = lua_gettop(L);
  	if (argc != 0) return luaL_error(L, "wrong number of arguments.");
  	init3D();
	return 0;
}


static int lua_camposition(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 3) return luaL_error(L, "wrong number of arguments");
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);
	
	setCameraPosition(x, y, z);

	return 0;
}


static int lua_camrotation(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 3) return luaL_error(L, "wrong number of arguments");
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);
	
	setCameraRotation(x, y, z);

	return 0;
}

static int lua_lightnumber(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");

	int lightcount = luaL_checkinteger(L, 1);

	setLightQuantity(lightcount);

	return 0;
}

static int lua_createlight(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 8) return luaL_error(L, "wrong number of arguments");
	int id = luaL_checkinteger(L, 1);
	float dir_x = luaL_checknumber(L, 2);
	float dir_y = luaL_checknumber(L, 3);
	float dir_z = luaL_checknumber(L, 4);
	float r = luaL_checknumber(L, 5);
	float g = luaL_checknumber(L, 6);
	float b = luaL_checknumber(L, 7);
	int type = luaL_checknumber(L, 8);
	
	
	createLight(id, dir_x, dir_y, dir_z, type, r, g, b);

	return 0;
}

static int lua_loadobj(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1 && argc != 2) return luaL_error(L, "wrong number of arguments");

	const char *model_file = luaL_checkstring(L, 1); //Model filename

	const char *texture_file = NULL;
	if (argc == 2) texture_file = luaL_checkstring(L, 2); //Texture filename

	ps2ObjMesh* m = loadOBJ(model_file, texture_file);

	// Push model object into Lua stack
	lua_pushinteger(L, (uint32_t)m);
	return 1;
}

static int lua_drawobj(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 7) return luaL_error(L, "wrong number of arguments");
	ps2ObjMesh* m = (ps2ObjMesh*)luaL_checkinteger(L, 1);
	float pos_x = luaL_checknumber(L, 2);
	float pos_y = luaL_checknumber(L, 3);
	float pos_z = luaL_checknumber(L, 4);
	float rot_x = luaL_checknumber(L, 5);
	float rot_y = luaL_checknumber(L, 6);
	float rot_z = luaL_checknumber(L, 7);
	
	drawOBJ(m, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z);

	return 0;
}

//Register our Graphics Functions
static const luaL_Reg Render_functions[] = {
  {"init",           		 lua_init3D},
  {"load",     				lua_loadobj},
  {"draw",  				lua_drawobj},
  {0, 0}
};

static const luaL_Reg Lights_functions[] = {
  {"set",  		lua_createlight},
  {"create", 	lua_lightnumber},
  {0, 0}
};

static const luaL_Reg Camera_functions[] = {
  {"position", 	lua_camposition},
  {"rotation", 	lua_camrotation},
  {0, 0}
};

void luaRender_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Render_functions, 0);
	lua_setglobal(L, "Render");

	lua_newtable(L);
	luaL_setfuncs(L, Lights_functions, 0);
	lua_setglobal(L, "Lights");

	lua_newtable(L);
	luaL_setfuncs(L, Camera_functions, 0);
	lua_setglobal(L, "Camera");

	lua_pushinteger(L, LIGHT_AMBIENT);
	lua_setglobal (L, "AMBIENT");

	lua_pushinteger(L, LIGHT_DIRECTIONAL);
	lua_setglobal (L, "DIRECTIONAL");
	
}
