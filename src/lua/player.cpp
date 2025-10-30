#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include "../include/luaplayer.h"

static lua_State *L;

const char * runScript(const char* script, bool isStringBuffer )
{
    printf("Creating luaVM... \n");

  	L = luaL_newstate();

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

	int s = 0, l = 0;
	char * errMsg =(char*)malloc(sizeof(char)*512);

	if(!isStringBuffer) s = luaL_loadfile(L, script);
	else {
    s = luaL_loadbuffer(L, script, strlen(script), NULL);
  }


	if (s == 0) s = lua_pcall(L, 0, LUA_MULTRET, 0);

	if (s) {
		l = sprintf(&errMsg[l], "\t%s\n", lua_tostring(L, -1));
    	printf("%s\n", lua_tostring(L, -1));
#ifndef LUAERROR_DONT_PRINT_STACK
    	int n = lua_gettop(L);
    	int i;

    	if (n == 0) {
			l += sprintf(&errMsg[l], "Stack is empty.\n");
			return 0;
    	}

    	for (i = 1; i <= n; i++) {
			l += sprintf(&errMsg[l], "\tLUA Stack:\n");
			l += sprintf(&errMsg[l], "\t[%i]: ", i);
    		switch(lua_type(L, i)) {
    		    case LUA_TNONE:
			    	l += sprintf(&errMsg[l], "Invalid");
    		        break;
    		    case LUA_TNIL:
    		        l += sprintf(&errMsg[l], "(Nil)");
    		        break;
    		    case LUA_TNUMBER:
    		        l += sprintf(&errMsg[l], "(Number) %f", lua_tonumber(L, i));
    		        break;
    		    case LUA_TBOOLEAN:
    		        l += sprintf(&errMsg[l], "(Bool)   %s", (lua_toboolean(L, i) ? "true" : "false"));
    		        break;
    		    case LUA_TSTRING:
    		        l += sprintf(&errMsg[l], "(String) %s", lua_tostring(L, i));
    		        break;
    		    case LUA_TTABLE:
    		        l += sprintf(&errMsg[l], "(Table)");
    		        break;
    		    case LUA_TFUNCTION:
    		        l += sprintf(&errMsg[l], "(Function)");
    		        break;
    		    default:
    		        l += sprintf(&errMsg[l], "Unknown");
			}

			l += sprintf(&errMsg[l], "\n");
    	}
#endif

		lua_pop(L, 1); // remove error message
	}
	lua_close(L);

	return (const char*)errMsg;
}
