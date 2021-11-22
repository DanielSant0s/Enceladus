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

static int SyncMCFileWrite(int fd, int size, void *Buffer){
	int result;

	result=0;
	mcSync(0, NULL, &result);

	mcClose(fd);

	/* Free the buffer. */
	if(Buffer!=NULL) free(Buffer);

	if(result!=size){
		printf("Write fault.\n");
		result=-EIO;
	}

	mcSync(0, NULL, NULL);

	printf((result>=0)?"Done!\n":"failed!\n");

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

	int McFileFD;

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

			result = SyncMCFileWrite(McFileFD, size, buf);

			mcOpen(port, slot, dest, O_WRONLY|O_CREAT|O_TRUNC);
			mcSync(0, NULL, &McFileFD);

			if(McFileFD<0){
				printf("Error creating file %s.\n", dest);
				result=McFileFD;
			}
			else{
				mcWrite(McFileFD, buf, size);
				//PrevFileSize=size;
				}
		}
	} else {
		printf("Error allocating %u bytes of memory for file %s.\n", size, file_tbo);
	}

	if(result>=0){	/* After the copying process has begun for the last file, we need to synchronize writing. */
		result=SyncMCFileWrite(McFileFD, size, buf);
	}



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