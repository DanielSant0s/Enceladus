#ifndef __LUAPLAYER_H
#define __LUAPLAYER_H

#include <debug.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <cstdint>

extern char boot_path[255];

#ifdef DEBUG
#define dbgprintf(args...) scr_printf(args)
#else
#define dbgprintf(args...) ;
#endif

int getBootDevice(void);

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(val, min, max) ((val)>(max)?(max):((val)<(min)?(min):(val)))

#define ASYNC_TASKS_MAX 1

extern size_t GetFreeSize(void);

extern const char * runScript(const char* script, bool isStringBuffer);
extern void luaC_collectgarbage (lua_State *L);

//extern void luaSound_init(lua_State *L);
extern void luaControls_init(lua_State *L);
extern void luaGraphics_init(lua_State *L);
extern void luaScreen_init(lua_State *L);
extern void luaTimer_init(lua_State *L);
extern void luaSystem_init(lua_State *L);
extern void luaSound_init(lua_State *L);
extern void luaRender_init(lua_State *L);
extern void stackDump (lua_State *L);

#endif

