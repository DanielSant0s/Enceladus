#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <debug.h>

#include "include/luaplayer.h"

#ifndef NO_PANIC_FUNCTION
int test_error(lua_State * L);
#endif
static lua_State *L;

const char * runScript(const char* script, bool isStringBuffer )
{	
    printf("Creating luaVM... \n");

  	L = luaL_newstate();
#ifndef NO_PANIC_FUNCTION
    lua_atpanic(L, test_error);
#endif
	  // Init Standard libraries
	  luaL_openlibs(L);

    printf("Loading libs... ");

	  // init graphics
    luaGraphics_init(L);
    luaControls_init(L);
	luaScreen_init(L);
    luaTimer_init(L);
    luaSystem_init(L);
    luaSound_init(L);
    luaRender_init(L);
#ifdef PS2KBD
	luaKeyboardInit(L);
#endif

    printf("done !\n");
     
	if(!isStringBuffer){
        printf("Loading script : `%s'\n", script);
	}

	int s = 0;
	const char * errMsg =(const char*)malloc(sizeof(char)*512);

	if(!isStringBuffer) s = luaL_loadfile(L, script);
	else {
    s = luaL_loadbuffer(L, script, strlen(script), NULL);
  }

		
	if (s == 0) s = lua_pcall(L, 0, LUA_MULTRET, 0);

	if (s) {
		sprintf((char*)errMsg, "%s\n", lua_tostring(L, -1));
    printf("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1); // remove error message
	}
	lua_close(L);
	
	return errMsg;
}
#define TPRINTF(arg, x...) \
    printf(arg, ##x); \
    scr_printf(arg, ##x)

#ifndef NO_PANIC_FUNCTION
int test_error(lua_State * L) {
    scr_clear();
    init_scr();
    scr_clear();
    scr_clear();
    scr_setCursor(0);
    int n = lua_gettop(L);
    int i;
    scr_printf("\t");
    TPRINTF("LUA ERROR.\n");

    if (n == 0) {
        scr_printf("\t");
        TPRINTF("Stack is empty!!!!\n");
    }

    for (i = 1; i <= n; i++) {
        scr_printf("\t");
        TPRINTF("%i: ", i);
        switch(lua_type(L, i)) {
        case LUA_TNONE:
            TPRINTF("Invalid");
            break;
        case LUA_TNIL:
            TPRINTF("(Nil)");
            break;
        case LUA_TNUMBER:
            TPRINTF("(Number) %f", lua_tonumber(L, i));
            break;
        case LUA_TBOOLEAN:
            TPRINTF("(Bool)   %s", (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TSTRING:
            TPRINTF("%s", lua_tostring(L, i));
            break;
        case LUA_TTABLE:
            TPRINTF("(Table)");
            break;
        case LUA_TFUNCTION:
            TPRINTF("(Function)");
            break;
        default:
            TPRINTF("Unknown");
        }
    TPRINTF("\n");
    }
    SleepThread(); //else, crash still ocurs
    return 0;
}
#endif
