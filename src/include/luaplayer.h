#ifndef __LUAPLAYER_H
#define __LUAPLAYER_H

#include "libpad.h"

extern "C" {
#include <debug.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern char boot_path[255];

#ifdef DEBUG
#define dbgprintf(args...) scr_printf(args)
#else
#define dbgprintf(args...) ;
#endif

int getBootDevice(void);

extern void luaC_collectgarbage (lua_State *L);
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(val, min, max) ((val)>(max)?(max):((val)<(min)?(min):(val)))

#define ASYNC_TASKS_MAX 1

extern int waitPadReady(int port, int slot);

extern int isButtonPressed(u32 button);

extern int initializePad(int port, int slot);

extern void pad_init();

extern padButtonStatus readPad(int port, int slot);

extern size_t GetFreeSize(void);

extern const char * runScript(const char* script, bool isStringBuffer);
extern void luaC_collectgarbage (lua_State *L);

//extern void luaSound_init(lua_State *L);
extern void luaControls_init(lua_State *L);
extern void luaGraphics_init(lua_State *L);
extern void luaScreen_init(lua_State *L);
extern void luaTimer_init(lua_State *L);
extern void luaSystem_init(lua_State *L);
extern void luaRender_init(lua_State *L);
extern void stackDump (lua_State *L);


#endif

