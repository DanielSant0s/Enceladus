#include "../include/luaplayer.h"
#include "../include/pad.h"
#include <stdint.h>
extern "C"{
#include <libds34bt.h>
#include <libds34usb.h>
}

static int lua_gettype(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0 && argc != 1) return luaL_error(L, "wrong number of arguments");
	int port = 0;
	if (argc == 1){
		port = luaL_checkinteger(L, 1);
		if (port > 1) return luaL_error(L, "wrong port number.");
	}
	int mode = padInfoMode(port, 0, PAD_MODETABLE, 0);
	lua_pushinteger(L, mode);
	return 1;
}

static int lua_getpad(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 0 && argc != 1) return luaL_error(L, "wrong number of arguments");
	int port = 0;
	if (argc == 1){
		port = luaL_checkinteger(L, 1);
		if (port > 1) return luaL_error(L, "wrong port number.");
	}

	padButtonStatus buttons;
	u32 paddata = 0;
	int ret;

	int state = padGetState(port, 0);

	if ((state == PAD_STATE_STABLE) || (state == PAD_STATE_FINDCTP1)) {
        // pad is connected. Read pad button information.
        ret = padRead(port, 0, &buttons); // port, slot, buttons
        if (ret != 0) {
            paddata = 0xffff ^ buttons.btns;
        }
    }

	if (ds34bt_get_status(port) & DS34BT_STATE_RUNNING) {
        ret = ds34bt_get_data(port, (u8 *)&buttons.btns);
        if (ret != 0) {
            paddata |= 0xffff ^ buttons.btns;
        }
    }

	if (ds34usb_get_status(port) & DS34USB_STATE_RUNNING) {
        ret = ds34usb_get_data(port, (u8 *)&buttons.btns);
        if (ret != 0) {
            paddata |= 0xffff ^ buttons.btns;
        }
    }

	lua_pushinteger(L, paddata);
	return 1;
}

static int lua_getleft(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 0 && argc != 1) return luaL_error(L, "wrong number of arguments.");
	int port = 0;
	if (argc == 1){
		port = luaL_checkinteger(L, 1);
		if (port > 1) return luaL_error(L, "wrong port number.");
	}

	padButtonStatus buttons;

	int state = padGetState(port, 0);

	if ((state == PAD_STATE_STABLE) || (state == PAD_STATE_FINDCTP1)) padRead(port, 0, &buttons);
	if (ds34bt_get_status(port) & DS34BT_STATE_RUNNING) ds34bt_get_data(port, (u8 *)&buttons.btns);
	if (ds34usb_get_status(port) & DS34USB_STATE_RUNNING) ds34usb_get_data(port, (u8 *)&buttons.btns);

	lua_pushinteger(L, buttons.ljoy_h-127);
	lua_pushinteger(L, buttons.ljoy_v-127);
	return 2;
	}

static int lua_getright(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 0 && argc != 1) return luaL_error(L, "wrong number of arguments.");
	int port = 0;
	if (argc == 1){
		port = luaL_checkinteger(L, 1);
		if (port > 1) return luaL_error(L, "wrong port number.");
	}

	padButtonStatus buttons;

	int state = padGetState(port, 0);

	if ((state == PAD_STATE_STABLE) || (state == PAD_STATE_FINDCTP1)) padRead(port, 0, &buttons);
	if (ds34bt_get_status(port) & DS34BT_STATE_RUNNING) ds34bt_get_data(port, (u8 *)&buttons.btns);
	if (ds34usb_get_status(port) & DS34USB_STATE_RUNNING) ds34usb_get_data(port, (u8 *)&buttons.btns);

	lua_pushinteger(L, buttons.rjoy_h-127);
	lua_pushinteger(L, buttons.rjoy_v-127);
	return 2;
	}

static int lua_getpressure(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1 && argc != 2) return luaL_error(L, "wrong number of arguments.");
	int port = 0;
	int button;
	if (argc == 2) {
		port = luaL_checkinteger(L, 1);
		button = luaL_checkinteger(L, 2);
	} else {
		button = luaL_checkinteger(L, 1);
	}

	padButtonStatus pad;

	unsigned char pressure = 255;

	int state = padGetState(port, 0);

	if ((state == PAD_STATE_STABLE) || (state == PAD_STATE_FINDCTP1)) padRead(port, 0, &pad);
	if (ds34bt_get_status(port) & DS34BT_STATE_RUNNING) ds34bt_get_data(port, (u8 *)&pad.btns);
	if (ds34usb_get_status(port) & DS34USB_STATE_RUNNING) ds34usb_get_data(port, (u8 *)&pad.btns);

	switch (button) {
	    case PAD_RIGHT:
	        pressure = pad.right_p;
	        break;
	    case PAD_LEFT:
	        pressure = pad.left_p;
	        break;
	    case PAD_UP:
	        pressure = pad.up_p;
	        break;
		case PAD_DOWN:
	        pressure = pad.down_p;
	        break;
		case PAD_TRIANGLE:
	        pressure = pad.triangle_p;
	        break;
		case PAD_CIRCLE:
	        pressure = pad.circle_p;
	        break;
		case PAD_CROSS:
	        pressure = pad.cross_p;
	        break;
		case PAD_SQUARE:
	        pressure = pad.square_p;
	        break;
		case PAD_L1:
	        pressure = pad.l1_p;
	        break;
		case PAD_R1:
	        pressure = pad.r1_p;
	        break;
		case PAD_L2:
	        pressure = pad.l2_p;
	        break;
		case PAD_R2:
	        pressure = pad.r2_p;
	        break;
	    default:
	        pressure = 0;
	        break;
    }

	lua_pushinteger(L, (uint32_t)pressure);
	return 1;
}

static int lua_rumble(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 2 && argc != 3) return luaL_error(L, "wrong number of arguments.");
	static char actAlign[6];
	int port = 0;
	if (argc == 3){
		port = luaL_checkinteger(L, 1);
		actAlign[0] = luaL_checkinteger(L, 2);
		actAlign[1] = luaL_checkinteger(L, 3);
	} else {
		actAlign[0] = luaL_checkinteger(L, 1);
		actAlign[1] = luaL_checkinteger(L, 2);
	}

	int state = padGetState(port, 0);
	if ((state == PAD_STATE_STABLE) || (state == PAD_STATE_FINDCTP1)) padSetActDirect(port, 0, actAlign);
	if (ds34bt_get_status(port) & DS34BT_STATE_RUNNING) ds34bt_set_rumble(port, actAlign[1], actAlign[1]);
	if (ds34usb_get_status(port) & DS34USB_STATE_RUNNING) ds34usb_set_rumble(port, actAlign[1], actAlign[1]);

	return 0;
}

static int lua_check(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 2) return luaL_error(L, "wrong number of arguments.");
	int pad = luaL_checkinteger(L, 1);
	int button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, (pad & button));
	return 1;
}


static int lua_set_led(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 3 && argc != 4) return luaL_error(L, "wrong number of arguments.");
	u8 led[4];
	int port = 0;
	if (argc == 4){
		port = luaL_checkinteger(L, 1);
		led[0] = luaL_checkinteger(L, 2);
		led[1] = luaL_checkinteger(L, 3);
		led[2] = luaL_checkinteger(L, 4);
	} else {
		led[0] = luaL_checkinteger(L, 1);
		led[1] = luaL_checkinteger(L, 2);
		led[2] = luaL_checkinteger(L, 3);
	}

	led[3] = 0;

	if (ds34bt_get_status(port) & DS34BT_STATE_RUNNING) ds34bt_set_led(port, led);
	if (ds34usb_get_status(port) & DS34USB_STATE_RUNNING) ds34usb_set_led(port, led);

	return 0;
}

//Register our Screen Functions
static const luaL_Reg Pads_functions[] = {
  {"get",              lua_getpad},
  {"getLeftStick",    lua_getleft},
  {"getRightStick",  lua_getright},
  {"getType",         lua_gettype},
  {"getPressure", lua_getpressure},
  {"rumble",           lua_rumble},
  {"setLED",          lua_set_led},
  {"check",             lua_check},
  {0, 0}
};

#define LUA_FORWARD_INTMACRO(macro) lua_pushinteger(L, macro); lua_setglobal (L, #macro)

void luaControls_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Pads_functions, 0);
	lua_setglobal(L, "Pads");

	LUA_FORWARD_INTMACRO(PAD_SELECT);
	LUA_FORWARD_INTMACRO(PAD_START);
	LUA_FORWARD_INTMACRO(PAD_UP);
	LUA_FORWARD_INTMACRO(PAD_RIGHT);
	LUA_FORWARD_INTMACRO(PAD_DOWN);
	LUA_FORWARD_INTMACRO(PAD_LEFT);
	LUA_FORWARD_INTMACRO(PAD_TRIANGLE);
	LUA_FORWARD_INTMACRO(PAD_CIRCLE);
	LUA_FORWARD_INTMACRO(PAD_CROSS);
	LUA_FORWARD_INTMACRO(PAD_SQUARE);
	LUA_FORWARD_INTMACRO(PAD_L1);
	LUA_FORWARD_INTMACRO(PAD_R1);
	LUA_FORWARD_INTMACRO(PAD_L2);
	LUA_FORWARD_INTMACRO(PAD_R2);
	LUA_FORWARD_INTMACRO(PAD_L3);
	LUA_FORWARD_INTMACRO(PAD_R3);

	LUA_FORWARD_INTMACRO(PAD_TYPE_NEJICON);
	LUA_FORWARD_INTMACRO(PAD_TYPE_KONAMIGUN);
	LUA_FORWARD_INTMACRO(PAD_TYPE_DIGITAL);
	LUA_FORWARD_INTMACRO(PAD_TYPE_ANALOG);
	LUA_FORWARD_INTMACRO(PAD_TYPE_NAMCOGUN);
	LUA_FORWARD_INTMACRO(PAD_TYPE_DUALSHOCK);
	LUA_FORWARD_INTMACRO(PAD_TYPE_JOGCON);
	LUA_FORWARD_INTMACRO(PAD_TYPE_EX_TSURICON);
	LUA_FORWARD_INTMACRO(PAD_TYPE_EX_JOGCON);
}
