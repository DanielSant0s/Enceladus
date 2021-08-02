
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

#include <sbv_patches.h>
#include "smem.h"

#include "graphics.h"
#include "sound.h"
#include "luaplayer.h"

/* the boot.lua */
#include "boot.cpp"

extern void *usbhdfsd_irx;
extern int size_usbhdfsd_irx;

extern void *usbd_irx;
extern int size_usbd_irx;


#ifdef STANDALONE
extern u8 scriptLua_start[];
extern int scriptLua_size;
#endif

char boot_path[255];

/*
static int _bootDevice ;

typedef enum {
	BOOT_HOST,
	BOOT_MC,
	BOOT_HDD,
	BOOT_USB,
	BOOT_CD
} deviceType_t;

void setBootDevice(const char *path)
{

	if( !strcmp( path, "host:" ) ) {
		_bootDevice  = BOOT_HOST;
	}
	else if( !strcmp( path, "mc:" ) ) {
		_bootDevice  = BOOT_MC;
	}
	else if( !strcmp( path, "mass:" ) ) {
		_bootDevice  = BOOT_USB;
	}
	else if( !strcmp( path, "cdrom0:" ) || !strcmp( path, "cdfs" ) ) {
		_bootDevice  = BOOT_CD;
	}
}

int getBootDevice(void)
{
    return _bootDevice ;
}
*/

void initMC(void)
{
   int ret;
   // mc variables
   int mc_Type, mc_Free, mc_Format;

   
   printf("Initializing Memory Card\n");

   ret = mcInit(MC_TYPE_MC);
   
   if( ret < 0 ) {
	printf("MC_Init : failed to initialize memcard server.\n");
	dbgprintf("MC_Init : failed to initialize memcard server.\n");
   }
   
   // Since this is the first call, -1 should be returned.
   // makes me sure that next ones will work !
   mcGetInfo(0, 0, &mc_Type, &mc_Free, &mc_Format); 
   mcSync(MC_WAIT, NULL, &ret);
}

void Init_Usb_Modules(void)
{
    int ret;

    SifExecModuleBuffer(&usbhdfsd_irx, size_usbhdfsd_irx, 0, NULL, &ret);
    SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, &ret);

}

void resetIOP(void)
{
    SifInitRpc(0);
    #ifdef RESET_IOP        
    printf("Resetting IOP...\n");
    while (!SifIopReset("", 0)) {
	};
	while (!SifIopSync()) {
	};
    printf("IOP synced\n");
    SifInitRpc(0);
    SifLoadFileInit();
    #endif
}



void systemInit()
{
    int ret;

    resetIOP();
    
    // install sbv patch fix
    printf("Installing SBV Patch...\n");
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check(); 
    sbv_patch_fileio(); 

    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);

    // load pad & mc modules 
    printf("Installing Pad & MC modules...\n");

    // load USB modules    
    Init_Usb_Modules();

    initMC();

    init_scr();
    sleep(1); 

}


void setLuaBootPath(int argc, char ** argv, int idx)
{
    // detect host : strip elf from path
    if (argc == 0)  // argc == 0 usually means naplink..
    {
	 strcpy (boot_path,"host:");
	 
    }
    else if (argc>=(idx+1))
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
    
    //strcpy (boot_path,"mass:LUA/");
    
    // check if path needs patching
    if( !strncmp( boot_path, "mass:/", 6) && (strlen (boot_path)>6))
    {
        strcpy((char *)&boot_path[5],(const char *)&boot_path[6]);
    }
      
    
}



int main(int argc, char * argv[])
{
    const char * errMsg;

	// PS2 specific initialization 
	systemInit();

    #ifndef STANDALONE	
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
	#else
	// set boot path global variable based on the elf path
	setLuaBootPath (argc, argv, 0);
    #endif
	
	printf("boot path : %s\n", boot_path);
	dbgprintf("boot path : %s\n", boot_path);
	
	// Lua init
	    // init internals library
    
    // graphics (gsKit)
    initGraphics();

    pad_init();
    
    // Init Libmikmod
    //initMikmod();

    // set base path luaplayer
    chdir(boot_path); 
    
    while (1)
    {
    
       #ifndef STANDALONE
       // if no parameters are specified, use the default boot
       if (argc < 2) 
       {    
            // execute Lua script (according to boot sequence)
            char* bootStringWith0 = (char*) malloc(size_bootString + 1);
	        memcpy(bootStringWith0, bootString, size_bootString);
	        bootString[size_bootString] = 0;
            // execute the boot script
            errMsg = runScript(bootStringWith0, true);
            free(bootStringWith0);
       }       
       else
          // execute the script gived as parameter
          errMsg = runScript(argv[1], false);       
    
       // Standalone elf with an embedded script
       #else
       errMsg = runScript((char*)scriptLua_start, true);
       #endif

       gsKit_clear_screens();
       
       if ( errMsg != NULL)
       {
        printf("Debug\n");
   	   scr_printf("Error: %s\n", errMsg );
       }
       
       scr_printf("\nPress [start] to restart\n");
       while (1/*!isButtonPressed(PAD_START)*/) graphicWaitVblankStart();
    }

	return 0;
}

