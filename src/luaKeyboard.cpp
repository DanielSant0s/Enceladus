#include <libkbd.h>
#include <ps2kbd.h>
#include <loadfile.h>

#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>

#include "include/luaplayer.h"

/* ///Reference of the structure for a valid keyboard mapfile
typedef struct keyboard_map
{
  PS2KbdKeyMap KeyMap;
  u8 SpecialMap[PS2KBD_KEYMAP_SIZE];
  u8 CtrlMap[PS2KBD_KEYMAP_SIZE];
  u8 AltMap[PS2KBD_KEYMAP_SIZE];
} keyboard_map_t; */

extern unsigned char ps2kbd_irx;
extern unsigned int size_ps2kbd_irx;

static int kbd_init(lua_State *L) {
    int ID, R, T;
	ID = SifExecModuleBuffer(&ps2kbd_irx, size_ps2kbd_irx, 0, NULL, &R);
    printf(" [PS2KBD]: ID=%d, RET=%d\n", ID, R);
    T = PS2KbdInit();
    printf("Keyboard EE Init (%d)\n", T);
	lua_pushboolean(L, ((ID > -1 && R != 1) && (!T)));
    return 1;
}

static int kbd_getchar(lua_State *L) {
    char key = 0;
	PS2KbdRead(&key);
	lua_pushinteger(L, key);
    return 1;
}
static int kbd_repeatRate(lua_State *L) {
    if (lua_gettop(L) != 1) return luaL_error(L, "%s wrong number of arguments", __FUNCTION__);
    uint32_t msec = luaL_checkinteger(L, 1);
    int R = PS2KbdSetRepeatRate(msec);
    lua_pushinteger(L, R);
    return 1;
}
static int kbd_setblockingmode(lua_State *L) {
    if (lua_gettop(L) != 1) return luaL_error(L, "%s wrong number of arguments", __FUNCTION__);
    uint32_t mode = luaL_checkinteger(L, 1);
    int R = PS2KbdSetBlockingMode(mode);
    lua_pushinteger(L, R);
    return 1;
}
static int kbd_close(lua_State *L) {
    lua_pushinteger(L, PS2KbdClose());
    return 1;
}
static int kbd_keymap_set(lua_State *L) {
    if (lua_gettop(L) != 3) return luaL_error(L, "%s wrong number of arguments", __FUNCTION__);
    size_t MAPSIZE;
    const char* MAP;
    MAP = luaL_checklstring(L, 1, &MAPSIZE);

    if (MAPSIZE == 0x600 && MAP != NULL) {
		PS2KbdSetKeymap((PS2KbdKeyMap*)MAP);
		PS2KbdSetCtrlmap((u8*)(MAP + 0x300));
		PS2KbdSetAltmap((u8*)(MAP + 0x400));
		PS2KbdSetSpecialmap((u8*)(MAP + 0x500));
	    lua_pushboolean(L, true);
    } else {
	    lua_pushboolean(L, false); 
        printf("%s: ERROR: provided map file is not 0x600 bytes sized\n", __FUNCTION__);
    }
    return 1;
}
static int kbd_keymap_reset(lua_State *L) {
    lua_pushinteger(L, PS2KbdResetKeymap());
    return 1;
}

static const luaL_Reg Keyboard_functions[] = {
  {"Init",        		           kbd_init},
  {"GetKey",    	            kbd_getchar},
  {"SetRepeatRate",          kbd_repeatRate},
  {"SetBlockingMode",   kbd_setblockingmode},
  {"Close",      		          kbd_close},
  {"LoadKeyMap",     	     kbd_keymap_set},
  {"ResetDefaultKeymap",   kbd_keymap_reset},
  {0, 0}
};

void luaKeyboardInit(lua_State *L){
	lua_newtable(L);
	luaL_setfuncs(L, Keyboard_functions, 0);
	lua_setglobal(L, "Keyboard");
}