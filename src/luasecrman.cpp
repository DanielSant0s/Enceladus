#include <stdlib.h>
#include "include/luaplayer.h"
#include "include/secrman_rpc.h"


static int lua_initsecrman(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
#endif
	InitSECRMAN();
	return 0;
}

static int lua_deinitsecrman(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
#endif
	DeinitSECRMAN();
	return 0;
}

static int lua_secrdownloadfile(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 4) return luaL_error(L, "wrong number of arguments");
#endif
    int port = luaL_checkinteger(L, 1);
    int slot = luaL_checkinteger(L, 2);
    int buffer = luaL_checkinteger(L, 3);
    int size = luaL_checkinteger(L, 4);

	int result = (int)SecrDownloadFile(port, slot, (void*)buffer, size);
    lua_pushinteger(L, (uint32_t)result);
	return 1;
}

static const luaL_Reg Secrman_functions[] = {
  	{"init",           		   lua_initsecrman},
    {"deinit",               lua_deinitsecrman},
    {"downloadFile",      lua_secrdownloadfile},
  {0, 0}
};


void luaSecrMan_init(lua_State *L) {

    lua_newtable(L);
	luaL_setfuncs(L, Secrman_functions, 0);
	lua_setglobal(L, "Secrman");

}