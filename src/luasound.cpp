#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "luaplayer.h"

#include "sound.h"

// Forward declaration
static Voice* pushVoice(lua_State *L);


// ===================================
// Sound through mikmodlib and sound.*
// ===================================


// Music
// ------------------------------

static const char *sound_meta   = ":sound";
static const char *voice_meta   = ":voice";

static int Music_loadAndPlay(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 && argc != 2) return luaL_error(L, "wrong number of arguments");
	
	const char *path = luaL_checkstring(L, 1);
	BOOL loop = false;
	if(argc == 2) loop = lua_toboolean(L, 2);
	
	char fullpath[512];
	getcwd(fullpath, 256);
	strcat(fullpath, "/");
	strcat(fullpath, path);

	FILE* soundFile = fopen(fullpath, "rb");
	if (!soundFile) return luaL_error(L, "can't open sound file %s.", fullpath);
	fclose(soundFile);
	
	lua_gc(L, LUA_GCCOLLECT, 0);
	loadAndPlayMusicFile(fullpath, loop);
	
	return 0;
}

static int Music_StopAndUnload(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 0 ) return luaL_error(L, "wrong number of arguments");
	
	stopAndUnloadMusic();
	
	return 0;
}

static int Music_playing(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 0 ) return luaL_error(L, "wrong number of arguments");
	
	BOOL result = musicIsPlaying();

	lua_pushboolean(L, result);
	return 1;
}

static int Music_pause(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 0 ) return luaL_error(L, "wrong number of arguments");
	musicPause();
	return 0;
}
static int Music_resume(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 0 ) return luaL_error(L, "wrong number of arguments");
	musicResume();
	return 0;
}


static int Music_volume(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 && argc != 0 ) return luaL_error(L, "wrong number of arguments");
	if(argc)
		lua_pushnumber(L, setMusicVolume((unsigned int) luaL_checknumber(L, 1)));
	else
		lua_pushnumber(L, setMusicVolume(9999));

	return 1;
}


static const luaL_Reg Music_functions[] = {
  {"playFile",          Music_loadAndPlay},
  {"stop",           	Music_StopAndUnload},
  {"pause",		 		Music_pause},
  {"resume",	 		Music_resume},
  {"playing", 			Music_playing},
  {"volume", 			Music_volume},
  {0, 0}
};


// Utility functions
// ------------------------------

static int lua_setSFXVolume(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 ) return luaL_error(L, "wrong number of arguments");
	int arg = (int) luaL_checknumber(L, 1);

	setSFXVolume(arg);

	return 0;
}
static int lua_setReverb(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 ) return luaL_error(L, "wrong number of arguments");
	int arg = (int) luaL_checknumber(L, 1);

	setReverb(arg);

	return 0;
}
static int lua_setPanSep(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 ) return luaL_error(L, "wrong number of arguments");
	int arg = (int) luaL_checknumber(L, 1);

	setPanSep(arg);

	return 0;
}

static const luaL_Reg SoundSystem_functions[] = {
  {"SFXVolume",          		lua_setSFXVolume},
  {"reverb",           			lua_setReverb},
  {"panoramicSeparation", 		lua_setPanSep},
  {0, 0}
};



static Sound** toSound (lua_State *L, int index)
{
  Sound** pi = (Sound**)lua_touserdata(L, index);
  if (pi == NULL) luaL_error(L, sound_meta);
  return pi;
}

static Sound** pushSound (lua_State *L)
{
  	Sound** sound = (Sound**)lua_newuserdata(L, sizeof(Sound**));
  	luaL_getmetatable(L, sound_meta);
  	lua_pushvalue(L, -1);
  	lua_setmetatable(L, -2); 
  	return sound;   
}


static int Sound_load(lua_State *L) {
	int argc = lua_gettop(L);
	if(argc != 1 && argc != 2)
		return luaL_error(L, "Argument error: Sound.load(filename, [bool loop]) takes one or two arguments.");
	
	// Load variables
	char fullpath[512];
	const char *path = luaL_checkstring(L, 1);
	if(!path) return luaL_error(L, "Argument must be a file path.");
	getcwd(fullpath, 256);
	strcat(fullpath, "/");
	strcat(fullpath, path);
	BOOL doloop = (argc==2)?lua_toboolean(L, 2):false;
	
	FILE* soundFile = fopen(fullpath, "rb");
	if (!soundFile) return luaL_error(L, "can't open sound file %s.", fullpath);
	fclose(soundFile);
	
	// Create the user object
	lua_gc(L, LUA_GCCOLLECT, 0);
	Sound* newsound = loadSound(fullpath);
	if (!newsound) return luaL_error(L, "error loading sound");
	Sound** luaNewsound = pushSound(L);
	*luaNewsound = newsound;
	if (doloop) setSoundLooping(newsound, 1, 0, 0);
	
	// Note: a userdata object has already been pushed.
	return 1;
}

static int Sound_gc(lua_State *L) // garbage collect
{
	Sound** handle= toSound(L, 1);
	unloadSound(*handle);
	return 0;
}

static int Sound_tostring (lua_State *L)
{
  char buff[32];
  sprintf(buff, "%p", *toSound(L, 1));
  lua_pushfstring(L, "Sound (%s)", buff);
  return 1;
}


static int Sound_play(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 ) return luaL_error(L, "Sound:play() takes no arguments. Also, call it with a colon, not a dot.");
	
	Sound** handle = toSound(L, 1);
	
	Voice *voice = pushVoice(L);
	*voice = playSound(*handle);
	
	// the Voice is already pushed
	return 1;
}

static const luaL_Reg Sound_methods[] = {
  {"load", Sound_load},
  {"play", Sound_play},
  {0, 0}
};

Voice *toVoice (lua_State *L, int index)
{
  Voice* handle  = (Voice*)lua_touserdata(L, index);
  if (handle == NULL) luaL_error(L, voice_meta);
  return handle;
}

Voice* pushVoice(lua_State *L) { \
	Voice* newvalue = (Voice*)lua_newuserdata(L, sizeof(Voice*)); \
  	luaL_getmetatable(L, voice_meta);
  	lua_pushvalue(L, -1);
  	lua_setmetatable(L, -2); 
	return newvalue; \
}

static int Voice_tostring (lua_State *L)
{
  lua_pushfstring(L, "Voice (%d)", *toVoice(L, 1));
  return 1;
}


static int Voice_stop(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 ) return luaL_error(L, "Voice:stop() takes no arguments. Also, call it with a colon, not a dot.");
	
	unsigned handle = *toVoice(L, 1);

	stopSound(handle);

	return 0;
}

static int Voice_resume(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 2 ) return luaL_error(L, "Voice:resume(Sound) one argument. Also, call it with a colon, not a dot.");
	
	unsigned handle = *toVoice(L, 1);
	Sound* soundhandle = *toSound(L, 1);
	resumeSound(handle, soundhandle);

	return 0;
}

static int Voice_setVolume(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 2 )
		return luaL_error(L, "Voice:setVolume() takes one argument.");
	
	unsigned handle = *toVoice(L, 1);
	int arg = (int) luaL_checknumber(L, 2);

	setVoiceVolume(handle, arg);

	return 0;
}

static int Voice_setPanning(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 2 )
		return luaL_error(L, "Voice:setPanning() takes one argument.");
	
	unsigned handle = *toVoice(L, 1);
	int arg = (int) luaL_checknumber(L, 2);
	
	setVoicePanning(handle, arg);

	return 0;
}

static int Voice_setFrequency(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 2 )
		return luaL_error(L, "Voice:setFrequency() takes one argument.");
	
	unsigned handle = *toVoice(L, 1);
	int arg = (int) luaL_checknumber(L, 2);

	
	setVoiceFrequency(handle, arg);

	return 0;
}

static int Voice_playing(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc != 1 )
		return luaL_error(L, "Voice:playing() takes no arguments. Also, call it with a colon, not a dot.");
	
	unsigned handle = *toVoice(L, 1);
	
	BOOL result = voiceIsPlaying(handle);

	lua_pushboolean(L, result);
	return 1;
}



static const luaL_Reg Voice_methods[] = {
  {"stop",          Voice_stop},
  {"resume",		Voice_resume},
  {"volume", 		Voice_setVolume},
  {"pan", 			Voice_setPanning},
  {"frequency", 	Voice_setFrequency},
  {"playing", 	 	Voice_playing},
  {0, 0}
};


void luaSound_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Music_functions, 0);
	lua_setglobal(L, "Music");
    lua_newtable(L);
	luaL_setfuncs(L, SoundSystem_functions, 0);
	lua_setglobal(L, "SoundSystem");
	
	luaL_newmetatable(L, sound_meta); // Stack: MyLib meta
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);             
	//lua_rawset(L, -3);   
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, Sound_gc); // Stack: MyLib meta "__gc" fptr
	lua_settable(L, -3); // Stack: MyLib meta
	lua_pop(L, 1); // Stack: MyLib
	luaL_setfuncs(L, Sound_methods, 0);
	
	luaL_newmetatable(L, voice_meta); // Stack: MyLib meta
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);               
	//lua_rawset(L, -3);   
	luaL_setfuncs(L, Voice_methods, 0);
	
}

