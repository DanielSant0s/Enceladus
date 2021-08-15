
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

#include <sbv_patches.h>
#include <smem.h>

#include "include/graphics.h"
#include "include/sound.h"
#include "include/luaplayer.h"

extern unsigned char bootString[];
extern unsigned int size_bootString;

extern unsigned char usbd_irx;
extern unsigned int size_usbd_irx;

extern unsigned char bdm_irx;
extern unsigned int size_bdm_irx;

extern unsigned char bdmfs_vfat_irx;
extern unsigned int size_bdmfs_vfat_irx;

extern unsigned char usbmass_bd_irx;
extern unsigned int size_usbmass_bd_irx;

extern unsigned char audsrv_irx;
extern unsigned int size_audsrv_irx;

char boot_path[255];

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


void systemInit()
{

    int ret;

    #ifdef RESET_IOP  
    SifInitRpc(0);
    while (!SifIopReset("", 0)){};
    while (!SifIopSync()){};
    SifInitRpc(0);
    #endif
    
    // install sbv patch fix
    printf("Installing SBV Patch...\n");
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check(); 
    sbv_patch_fileio(); 

    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
    SifLoadModule("rom0:LIBSD", 0, NULL);

    // load pad & mc modules 
    printf("Installing Pad & MC modules...\n");

    // load USB modules    
    SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&bdm_irx, size_bdm_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&bdmfs_vfat_irx, size_bdmfs_vfat_irx, 0, NULL, NULL);
    SifExecModuleBuffer(&usbmass_bd_irx, size_usbmass_bd_irx, 0, NULL, NULL);

    //load audio
    SifExecModuleBuffer(&audsrv_irx, size_audsrv_irx, 0, NULL, NULL);

    ret = audsrv_init();
	if (ret != 0)
	{
		printf("sample: failed to initialize audsrv\n");
		printf("audsrv returned error string: %s\n", audsrv_get_error_string());
	}

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
    
       // if no parameters are specified, use the default boot
       if (argc < 2) 
       {    
            // execute Lua script (according to boot sequence)
            errMsg = runScript((char*)bootString, true);
       }       
       else
        // execute the script gived as parameter
        errMsg = runScript(argv[1], false);   
    

       gsKit_clear_screens();
       
       if ( errMsg != NULL)
       {;
   	   scr_printf("Error: %s\n", errMsg );
       }
       
       scr_printf("\nPress [start] to restart\n");
       while (1/*!isButtonPressed(PAD_START)*/) graphicWaitVblankStart();
    }

	return 0;
}

