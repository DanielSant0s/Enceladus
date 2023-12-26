
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libmc.h>
#include <libcdvd.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <smod.h>
#include <audsrv.h>
#include <sys/stat.h>

#include <dirent.h>

#include <sbv_patches.h>
#include <smem.h>

#include "include/graphics.h"
#include "include/sound.h"
#include "include/luaplayer.h"
#include "include/pad.h"

#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include <fileio.h>

extern "C"{
#include <libds34bt.h>
#include <libds34usb.h>
}

extern char bootString[];
extern unsigned int size_bootString;

#define IMPORT_BIN2C(_T) \
    extern unsigned char _T[]; \
    extern unsigned int size_##_T

IMPORT_BIN2C(iomanX_irx);
IMPORT_BIN2C(fileXio_irx);
IMPORT_BIN2C(sio2man_irx);
IMPORT_BIN2C(mcman_irx);
IMPORT_BIN2C(mcserv_irx);
IMPORT_BIN2C(padman_irx);
IMPORT_BIN2C(libsd_irx);
IMPORT_BIN2C(cdfs_irx);
IMPORT_BIN2C(usbd_irx);
IMPORT_BIN2C(bdm_irx);
IMPORT_BIN2C(bdmfs_fatfs_irx);
IMPORT_BIN2C(usbmass_bd_irx);
IMPORT_BIN2C(audsrv_irx);
IMPORT_BIN2C(ds34usb_irx);
IMPORT_BIN2C(ds34bt_irx);

char boot_path[255];

void setLuaBootPath(int argc, char ** argv, int idx)
{
    if (argc>=(idx+1))
    {

	char *p;
	if ((p = strrchr(argv[idx], '/'))!=NULL) {
	    snprintf(boot_path, sizeof(boot_path), "%s", argv[idx]);
	    p = strrchr(boot_path, '/');
	if (p!=NULL)
	    p[1]='\0';
	} else if ((p = strrchr(argv[idx], '\\'))!=NULL) {
	   snprintf(boot_path, sizeof(boot_path), "%s", argv[idx]);
	   p = strrchr(boot_path, '\\');
	   if (p!=NULL)
	     p[1]='\0';
	} else if ((p = strchr(argv[idx], ':'))!=NULL) {
	   snprintf(boot_path, sizeof(boot_path), "%s", argv[idx]);
	   p = strchr(boot_path, ':');
	   if (p!=NULL)
	   p[1]='\0';
	}

    }
    
    // check if path needs patching
    if( !strncmp( boot_path, "mass:/", 6) && (strlen (boot_path)>6))
    {
        strcpy((char *)&boot_path[5],(const char *)&boot_path[6]);
    }
      
    
}


void initMC(void)
{
   int ret;
   // mc variables
   int mc_Type, mc_Free, mc_Format;

   
   printf("initMC: Initializing Memory Card\n");

   ret = mcInit(MC_TYPE_XMC);
   
   if( ret < 0 ) {
	printf("initMC: failed to initialize memcard server.\n");
   } else {
       printf("initMC: memcard server started successfully.\n");
   }
   
   // Since this is the first call, -1 should be returned.
   // makes me sure that next ones will work !
   mcGetInfo(0, 0, &mc_Type, &mc_Free, &mc_Format); 
   mcSync(MC_WAIT, NULL, &ret);
}

int GLOBAL_STATE =0;

int main(int argc, char * argv[])
{
    const char * errMsg;

    #ifdef RESET_IOP  
    SifInitRpc(0);
    while (!SifIopReset("", 0)){};
    while (!SifIopSync()){};
    SifInitRpc(0);
    #endif
    
    // install sbv patch fix
    printf("Installing SBV Patches...\n");
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check(); 
    sbv_patch_fileio(); 

	DIR *directorytoverify;
	directorytoverify = opendir("host:.");
	if (directorytoverify==NULL) {
		SifExecModuleBuffer(&iomanX_irx, size_iomanX_irx, 0, NULL, NULL);
		SifExecModuleBuffer(&fileXio_irx, size_fileXio_irx, 0, NULL, NULL);
		fileXioInit();
		closedir(directorytoverify);
	}
  
    printf("Loading mc drivers\n");
	  SifExecModuleBuffer(&sio2man_irx, size_sio2man_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&mcman_irx, size_mcman_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&mcserv_irx, size_mcserv_irx, 0, NULL, NULL);
    printf("Initialize mc\n");
    initMC();

    printf("loading pad drivers\n");
    SifExecModuleBuffer(&padman_irx, size_padman_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&libsd_irx, size_libsd_irx, 0, NULL, NULL);

    // load USB modules    
    SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, NULL);

    printf("loading ds34(USB/bt) drivers\n");
    int ds3pads = 1;
    SifExecModuleBuffer(&ds34usb_irx, size_ds34usb_irx, 4, (char *)&ds3pads, NULL);
    SifExecModuleBuffer(&ds34bt_irx, size_ds34bt_irx, 4, (char *)&ds3pads, NULL);
    ds34usb_init();
    ds34bt_init();

    SifExecModuleBuffer(&bdm_irx, size_bdm_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&bdmfs_fatfs_irx, size_bdmfs_fatfs_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&usbmass_bd_irx, size_usbmass_bd_irx, 0, NULL, NULL);

    SifExecModuleBuffer(&cdfs_irx, size_cdfs_irx, 0, NULL, NULL);

    SifExecModuleBuffer(&audsrv_irx, size_audsrv_irx, 0, NULL, NULL);

    //waitUntilDeviceIsReady by fjtrujy

    struct stat buffer;
    int ret = -1;
    int retries = 50;

    while(ret != 0 && retries > 0)
    {
        ret = stat("mass:/", &buffer);
        /* Wait until the device is ready */
        nopdelay();

        retries--;
    }
	
        // if no parameters are specified, use the default boot
	if (argc < 2)
	{
	   // set boot path global variable based on the elf path
	   setLuaBootPath (argc, argv, 0);  
        }
        else // set path based on the specified script
        {
           if (!strchr(argv[1], ':')) // filename doesn't contain device
              // set boot path global variable based on the elf path
	      setLuaBootPath (argc, argv, 0);  
           else
              // set path global variable based on the given script path
	      setLuaBootPath (argc, argv, 1);
	}
	
	// Lua init
	// init internals library
    
    // graphics (gsKit)
    initGraphics();

    pad_init();

    // set base path luaplayer
    chdir(boot_path); 

    printf("boot path : %s\n", boot_path);
	dbgprintf("boot path : %s\n", boot_path);
    
    while (1)
    {
    
        // if no parameters are specified, use the default boot
        if (argc < 2) {
            errMsg = runScript(bootString, true); 
        } else {
            errMsg = runScript(argv[1], false);
        }   

        init_scr();

        if (errMsg != NULL)
        {
            scr_setfontcolor(0x0000ff);
            sleep(1); //ensures message is printed no matter what
		    scr_clear();
		    scr_setXY(5, 2);
		    scr_printf("Enceladus ERROR!\n");
		    scr_printf(errMsg);
		    puts(errMsg);
		    scr_printf("\nPress [start] to restart\n");
        	while (!isButtonPressed(PAD_START)) {
                	sleep(1);
		    }
            scr_setfontcolor(0xffffff);
        }

    }

	return 0;
}

