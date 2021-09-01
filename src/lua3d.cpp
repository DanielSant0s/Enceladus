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


static int lua_loadobj(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");

	const char *model_file = luaL_checkstring(L, 1); //Model filename

	ps2ObjMesh* m = loadOBJ(model_file);

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
  {"init",           lua_init3D},
  {"loadOBJ",       lua_loadobj},
  {"drawOBJ",       lua_drawobj},
  {0, 0}
};


void luaRender_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Render_functions, 0);
	lua_setglobal(L, "Render");
	
}
