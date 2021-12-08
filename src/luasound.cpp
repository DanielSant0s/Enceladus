
#include <string.h>
#include <kernel.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/luaplayer.h"

#include <audsrv.h>

static int fillbuffer(void *arg)
{
	iSignalSema((int)arg);
	return 0;
}


static int lua_setformat(lua_State *L) {
	int argc = lua_gettop(L);
	//if (argc != 3) return luaL_error(L, "wrong number of arguments");
	struct audsrv_fmt_t format;

    format.bits = luaL_checkinteger(L, 1);
	format.freq = luaL_checkinteger(L, 2);
	format.channels = luaL_checkinteger(L, 3);
	
	audsrv_set_format(&format);

	return 0;
}


static int lua_adpcm_init(lua_State *L) {
	int argc = lua_gettop(L);
	//if (argc != 3) return luaL_error(L, "wrong number of arguments");
	
	audsrv_adpcm_init();

	return 0;
}

static int lua_setvolume(lua_State *L) {
	int argc = lua_gettop(L);
	//if (argc != 3) return luaL_error(L, "wrong number of arguments");

    int volume = luaL_checkinteger(L, 1);
	
	audsrv_set_volume(volume);

	return 0;
}

static int lua_setadpcmvolume(lua_State *L) {
	int argc = lua_gettop(L);
	//if (argc != 3) return luaL_error(L, "wrong number of arguments");
	
	int slot = luaL_checkinteger(L, 1);
    int volume = luaL_checkinteger(L, 2);
	
	audsrv_adpcm_set_volume(slot, volume);

	return 0;
}


static int lua_loadadpcm(lua_State *L) {
	int argc = lua_gettop(L);
	FILE* adpcm;
	audsrv_adpcm_t *sample = (audsrv_adpcm_t *)malloc(sizeof(audsrv_adpcm_t));
	int size;
	u8* buffer;

	const char* path = luaL_checkstring(L, 1);

	adpcm = fopen(path, "rb");

	fseek(adpcm, 0, SEEK_END);
	size = ftell(adpcm);
	fseek(adpcm, 0, SEEK_SET);



	buffer = (u8*)malloc(size);

	fread(buffer, 1, size, adpcm);
	fclose(adpcm);

	audsrv_load_adpcm(sample, buffer, size);

	free(buffer);

	lua_pushinteger(L, (uint32_t)sample);

	return 1;
}

static int lua_playadpcm(lua_State *L) {
	int argc = lua_gettop(L);

	int slot = luaL_checkinteger(L, 1);
	audsrv_adpcm_t *sample = (audsrv_adpcm_t *)luaL_checkinteger(L, 2);

	audsrv_ch_play_adpcm(slot, sample);
	return 0;
}


/*int main(int argc, char **argv)
{
	int ret;
	int played;
	int err;
	int bytes;
	char chunk[2048];
	FILE *wav;
	ee_sema_t sema;
	int fillbuffer_sema;

	sema.init_count = 0;
	sema.max_count = 1;
	sema.option = 0;
	fillbuffer_sema = CreateSema(&sema);

	audsrv_on_fillbuf(sizeof(chunk), fillbuffer, (void *)fillbuffer_sema);

	wav = fopen("host:song_22k.wav", "rb");

	fseek(wav, 0x30, SEEK_SET);

	printf("starting play loop\n");
	played = 0;
	bytes = 0;
	while (1)
	{
		ret = fread(chunk, 1, sizeof(chunk), wav);
		if (ret > 0)
		{
			WaitSema(fillbuffer_sema);
			audsrv_play_audio(chunk, ret);
		}

		if (ret < sizeof(chunk))
		{
			break;
		}

		played++;
		bytes = bytes + ret;

		if (played % 8 == 0)
		{
			printf("\r%d bytes sent..", bytes);
		}

		if (played == 512) break;
	}

	fclose(wav);

}*/



static const luaL_Reg Sound_functions[] = {
	{"setFormat",      							 lua_setformat},
	{"setVolume",      				   			 lua_setvolume},
	{"initADPCM",								lua_adpcm_init},
	{"setADPCMVolume",      				lua_setadpcmvolume},
	{"loadADPCM",      							 lua_loadadpcm},
	{"playADPCM",      							 lua_playadpcm},
	{0, 0}
};

void luaSound_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Sound_functions, 0);
	lua_setglobal(L, "Sound");
}