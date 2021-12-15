#include <unistd.h>
#include <libmc.h>
#include <malloc.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <loadfile.h>
#include "include/luaplayer.h"
#include "include/luaplayer.h"
#include "include/secrman_rpc.h"

static int SignKELF(void *buffer, int size, unsigned char port, unsigned char slot){
	int result;

	result=1;
	if(SecrDownloadFile(2+port, slot, buffer, size)==NULL){
		printf("Error signing file.\n");
		result=-EINVAL;
	}

	return result;
}

static int lua_initsecrman(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
#endif
	InitSECRMAN();
	return 0;
}

static int lua_deinitsecrman(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 0) return luaL_error(L, "wrong number of arguments");
#endif
	DeinitSECRMAN();
	return 0;
}

static int lua_secrdownloadfile(lua_State *L) {
	int argc = lua_gettop(L);
#ifndef SKIP_ERROR_HANDLING
	if (argc != 4) return luaL_error(L, "wrong number of arguments");
#endif
    int port = luaL_checkinteger(L, 1);
    int slot = luaL_checkinteger(L, 2);
    const char* file_tbo = luaL_checkstring(L, 3);
    const char* dest = luaL_checkstring(L, 4);

	void* buf;
	int result = 0;

	int fd = open(file_tbo, O_RDONLY, 0777);

	int size=lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	if((buf = memalign(64, size))!=NULL){
		if ((read(fd, buf, size)) != size) {
			printf("Error reading file %s.\n", file_tbo);
			close(fd);
    	} else {
			close(fd);

			if((result=SignKELF(buf, size, port, slot))<0){
				printf("Error signing file %s. Code: %d.\n", file_tbo, result);
				free(buf);
			}

			int McFileFD = open(dest, O_WRONLY|O_CREAT|O_TRUNC);
			
			write(McFileFD, buf, size);
			
			close(McFileFD);

		}
	} else {
		printf("Error allocating %u bytes of memory for file %s.\n", size, file_tbo);
	}
	
	free(buf);
	
    lua_pushinteger(L, (uint32_t)result);
	return 1;
}

static const luaL_Reg Secrman_functions[] = {
  	{"init",           		   lua_initsecrman},
    {"deinit",               lua_deinitsecrman},
    {"downloadFile",      lua_secrdownloadfile},
  {0, 0}
};


void luaSecrMan_init(lua_State *L) {

    lua_newtable(L);
	luaL_setfuncs(L, Secrman_functions, 0);
	lua_setglobal(L, "Secrman");

}