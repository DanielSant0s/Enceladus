#include <audsrv.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/luaplayer.h"

static int lua_setvolume(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
    int volume = luaL_checkinteger(L, 1);
	audsrv_set_volume(volume);
	return 0;
}

static int lua_loadsound(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "wrong number of arguments");
	const char* path = luaL_checkstring(L, 1);

	int ret;
	int played;
	char chunk[2048];
	FILE *wav;

	wav = fopen("host:song_22k.wav", "rb");
	if (wav == NULL)
	{
		printf("failed to open wav file\n");
		audsrv_quit();
		return 1;
	}

	fseek(wav, 0x30, SEEK_SET);

	printf("starting play loop\n");
	played = 0;
	while (1)
	{
		ret = fread(chunk, 1, sizeof(chunk), wav);
		if (ret > 0)
		{
			audsrv_wait_audio(ret);
			audsrv_play_audio(chunk, ret);
		}

		if (ret < sizeof(chunk))
		{
			/* no more data */
			break;
		}

		played++;
		if (played % 8 == 0)
		{
			printf(".");
		}

		if (played == 512) break;
	}

	fclose(wav);

	lua_pushinteger(L, ret);
	return 1;
}

static const luaL_Reg Sound_functions[] = {

  {0, 0}
};

void luaSound_init(lua_State *L) {

    lua_newtable(L);
	luaL_setfuncs(L, Sound_functions, 0);
	lua_setglobal(L, "Sound");
}