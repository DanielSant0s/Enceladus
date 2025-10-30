#include <stdlib.h>
#include <time.h>
#include "../include/luaplayer.h"

// if the timer is running:
// measuredTime is the value of the last call to getCurrentMilliseconds
// offset is the value of startTime
//
// if the timer is stopped:
// measuredTime is 0
// offset is the value time() returns on stopped timers

struct Timer{
	uint32_t magic;
	bool isPlaying;
	clock_t tick;
};

static int lua_newT(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
	Timer* new_timer = (Timer*)malloc(sizeof(Timer));
	new_timer->tick = clock();
	new_timer->magic = 0x4C544D52;
	new_timer->isPlaying = true;
	lua_pushinteger(L,(uint32_t)new_timer);
	return 1;
}

static int lua_time(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	Timer* src = (Timer*)luaL_checkinteger(L,1);
	#ifndef SKIP_ERROR_HANDLING
	if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying) {
		lua_pushinteger(L, (clock() - src->tick));
	} else {
		lua_pushinteger(L, src->tick);
	}
	return 1;
}

static int lua_pause(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
	if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying){
		src->isPlaying = false;
		src->tick = (clock()-src->tick);
	}
	return 0;
}

static int lua_resume(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
	if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (!src->isPlaying){
		src->isPlaying = true;
		src->tick = (clock()-src->tick);
	}
	return 0;
}

static int lua_reset(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
	if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying) src->tick = clock();
	else src->tick = 0;
	return 0;
}

static int lua_set(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 2) return luaL_error(L, "wrong number of arguments");
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	uint32_t val = (uint32_t)luaL_checkinteger(L, 2);
	#ifndef SKIP_ERROR_HANDLING
	if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying) src->tick = clock() + val;
	else src->tick = val;
	return 0;
}

static int lua_wisPlaying(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
	if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	lua_pushboolean(L, src->isPlaying);
	return 1;
}

static int lua_destroy(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	Timer* timer = (Timer*)luaL_checkinteger(L,1);
	#ifndef SKIP_ERROR_HANDLING
	if (timer->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	free(timer);
	return 0;
}

//Register our Timer Functions
static const luaL_Reg Timer_functions[] = {
  {"new",        		  lua_newT},
  {"getTime",    		  lua_time},
  {"setTime",    		   lua_set},
  {"destroy",    	   lua_destroy},
  {"pause",      		 lua_pause},
  {"resume",     		lua_resume},
  {"reset",      		 lua_reset},
  {"isPlaying",  	lua_wisPlaying},
  {0, 0}
};

void luaTimer_init(lua_State *L){
	lua_newtable(L);
	luaL_setfuncs(L, Timer_functions, 0);
	lua_setglobal(L, "Timer");
}
