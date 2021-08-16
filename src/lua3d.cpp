#include <stdlib.h>
#include <string.h>

#include "include/luaplayer.h"
#include "include/graphics.h"

static int lua_init3D(lua_State *L) {
	int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "wrong number of arguments.");
    init3D();
	return 0;
}

//Register our Graphics Functions
static const luaL_Reg Render_functions[] = {
  {"init",           lua_init3D},
  {0, 0}
};


void luaRender_init(lua_State *L) {
    lua_newtable(L);
	luaL_setfuncs(L, Render_functions, 0);
	lua_setglobal(L, "Render");
	
}
