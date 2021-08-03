#include <stdlib.h>
#include <string.h>

#include "include/luaplayer.h"
#include "include/graphics.h"

static int lua_init3D(lua_State *L) {
	int argc = lua_gettop(L);
    init3D();
	return 0;
}

static int lua_rendermodel(lua_State *L) {
	int argc = lua_gettop(L);
    render();
	return 0;
}

//Register our Graphics Functions
static const luaL_Reg Render_functions[] = {
  {"init",           lua_init3D},
  {"renderModel",           lua_rendermodel},
  {0, 0}
};


void luaRender_init(lua_State *L) {
    lua_newtable(L);
	luaL_setfuncs(L, Render_functions, 0);
	lua_setglobal(L, "Render");
	
}
