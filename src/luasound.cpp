#include <audsrv.h>
#include <string.h>
#include <kernel.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/luaplayer.h"

static const luaL_Reg Sound_functions[] = {

  {0, 0}
};

void luaSound_init(lua_State *L) {


    lua_newtable(L);
	luaL_setfuncs(L, Sound_functions, 0);
	lua_setglobal(L, "Sound");
}