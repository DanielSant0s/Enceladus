#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "include/render.h"
#include "include/luaplayer.h"

static int lua_initrender(lua_State *L) {
	int argc = lua_gettop(L);
  	if (argc != 1) return luaL_error(L, "wrong number of arguments.");
	float aspect = luaL_checknumber(L, 1);
  	init3D(aspect);
	return 0;
}

static int lua_loadobj(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if (argc != 2 && argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	const char *file_tbo = luaL_checkstring(L, 1); //Model filename
	
	// Loading texture
    GSTEXTURE* text = NULL;
	if(argc == 2) text = (GSTEXTURE*)(luaL_checkinteger(L, 2));
	
	model* res_m = loadOBJ(file_tbo, text);


	// Push model object into Lua stack
	lua_pushinteger(L, (uint32_t)res_m);
	return 1;
}


static int lua_freeobj(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
#endif
	
	model* m = (model*)(luaL_checkinteger(L, 1));

    free(m->idxList);
	m->idxList = NULL;

	free(m->positions);
	m->positions = NULL;

    free(m->colours);
	m->colours = NULL;

    free(m->normals);
	m->normals = NULL;

    free(m->texcoords);
	m->texcoords = NULL;
	
	free(m->bounding_box);
	m->bounding_box = NULL;
	
	free(m);
	m = NULL;

	return 0;
}

static int lua_drawobj(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 7) return luaL_error(L, "wrong number of arguments");
	model* m = (model*)luaL_checkinteger(L, 1);
	float pos_x = luaL_checknumber(L, 2);
	float pos_y = luaL_checknumber(L, 3);
	float pos_z = luaL_checknumber(L, 4);
	float rot_x = luaL_checknumber(L, 5);
	float rot_y = luaL_checknumber(L, 6);
	float rot_z = luaL_checknumber(L, 7);
	
	drawOBJ(m, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z);

	return 0;
}


static int lua_drawbbox(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 8) return luaL_error(L, "wrong number of arguments");
	model* m = (model*)luaL_checkinteger(L, 1);
	float pos_x = luaL_checknumber(L, 2);
	float pos_y = luaL_checknumber(L, 3);
	float pos_z = luaL_checknumber(L, 4);
	float rot_x = luaL_checknumber(L, 5);
	float rot_y = luaL_checknumber(L, 6);
	float rot_z = luaL_checknumber(L, 7);
	Color color = (Color)luaL_checkinteger(L, 8);
	
	draw_bbox(m, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, color);

	return 0;
}

//Register our Render Functions
static const luaL_Reg Render_functions[] = {
    {"init",           		    lua_initrender},
  	{"loadOBJ",           		   lua_loadobj},
    {"drawOBJ",           		   lua_drawobj},
	{"drawBbox",           		  lua_drawbbox},
    {"freeOBJ",           		   lua_freeobj},
  {0, 0}
};


static int lua_lightnumber(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");

	int lightcount = luaL_checkinteger(L, 1);

	setLightQuantity(lightcount);

	return 0;
}

static int lua_createlight(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 8) return luaL_error(L, "wrong number of arguments");
	int id = luaL_checkinteger(L, 1);
	float dir_x = luaL_checknumber(L, 2);
	float dir_y = luaL_checknumber(L, 3);
	float dir_z = luaL_checknumber(L, 4);
	float r = luaL_checknumber(L, 5);
	float g = luaL_checknumber(L, 6);
	float b = luaL_checknumber(L, 7);
	int type = luaL_checknumber(L, 8);
	
	
	createLight(id, dir_x, dir_y, dir_z, type, r, g, b);

	return 0;
}

static const luaL_Reg Lights_functions[] = {
  {"set",  		lua_createlight},
  {"create", 	lua_lightnumber},
  {0, 0}
};


static int lua_camposition(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 3) return luaL_error(L, "wrong number of arguments");
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);
	
	setCameraPosition(x, y, z);

	return 0;
}


static int lua_camrotation(lua_State *L){
	int argc = lua_gettop(L);
	if (argc != 3) return luaL_error(L, "wrong number of arguments");
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);
	
	setCameraRotation(x, y, z);

	return 0;
}

static const luaL_Reg Camera_functions[] = {
  {"position", 	lua_camposition},
  {"rotation", 	lua_camrotation},
  {0, 0}
};

void luaRender_init(lua_State *L) {
    lua_newtable(L);
	luaL_setfuncs(L, Render_functions, 0);
	lua_setglobal(L, "Render");

	lua_newtable(L);
	luaL_setfuncs(L, Lights_functions, 0);
	lua_setglobal(L, "Lights");

	lua_newtable(L);
	luaL_setfuncs(L, Camera_functions, 0);
	lua_setglobal(L, "Camera");

	lua_pushinteger(L, LIGHT_AMBIENT);
	lua_setglobal (L, "AMBIENT");

	lua_pushinteger(L, LIGHT_DIRECTIONAL);
	lua_setglobal (L, "DIRECTIONAL");

}