#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "include/render.h"
#include "include/luaplayer.h"

static int lua_setview(lua_State *L) {
  	viewport_3d(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4));
	return 0;
}

static int lua_loadobj(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
	if (argc != 2 && argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	const char *file_tbo = luaL_checkstring(L, 1); //Model filename
	
	// Loading texture
    gl_texture_t* text = NULL;
	if(argc == 2) text = (gl_texture_t*)(luaL_checkinteger(L, 2));
	
	model* res_m = loadOBJ(file_tbo, text);


	// Push model object into Lua stack
	lua_pushinteger(L, (uint32_t)res_m);
	return 1;
}


static int lua_freeobj(lua_State *L) {
	model* m = (model*)(luaL_checkinteger(L, 1));

	free(m->positions);
    free(m->normals);
    free(m->texcoords);
    free(m->colours);
	free(m->ranges);

	for(int i = 0; i < m->materialCount; i++) {
		if(m->materials[i].texture) {
			UnloadTexture(m->materials[i].texture);
		}
	}

	free(m->materials);

	free(m);
	m = NULL;

	return 0;
}

static int lua_drawobj(lua_State *L){
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
	{"setView",           		   lua_setview},
  	{"loadOBJ",           		   lua_loadobj},
    {"drawOBJ",           		   lua_drawobj},
	{"drawBbox",           		  lua_drawbbox},
    {"freeOBJ",           		   lua_freeobj},
  {0, 0}
};


static int lua_createlight(lua_State *L){
	lua_pushinteger(L, createLight());
	return 1;
}

static int lua_setlight(lua_State *L){
	int id = luaL_checkinteger(L, 1);
	int attr = luaL_checkinteger(L, 2);
	float a = luaL_checknumber(L, 3);
	float b = luaL_checknumber(L, 4);
	float c = luaL_checknumber(L, 5);
	float d = luaL_checknumber(L, 6);
	
	setLightAttribute(id, a, b, c, d, attr);

	return 0;
}

static const luaL_Reg Lights_functions[] = {
  {"set",  		lua_setlight},
  {"new", 	    lua_createlight},
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

	lua_pushinteger(L, GL_POSITION);
	lua_setglobal (L, "LIGHT_POSITION");

	lua_pushinteger(L, GL_AMBIENT);
	lua_setglobal (L, "LIGHT_AMBIENT");

	lua_pushinteger(L, GL_SPECULAR);
	lua_setglobal (L, "LIGHT_SPECULAR");

	lua_pushinteger(L, GL_DIFFUSE);
	lua_setglobal (L, "LIGHT_DIFFUSE");
}