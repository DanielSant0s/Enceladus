.SILENT:                                                                              

define HEADER
                                                                       
   @@@@@@@@*#                                                              
  @@@# @@@@@@@ @@@@%                                                    
   @@@.@@@@@@@@@@@@@@@@@@*       &&&&&&&.                                
     ,@@@@@@@@        @@@@@@&&&&&&&&&&&&&&&&                            
       *@@@@@@@          &&&&&&&&&@&&&&&&&&&&&&                         
          @@@@@@@      &&&&&&&&@@@@@@@@&&&&&&&&&&       @@@@@@       
             /@@@@@   &&&&&&@@@@@@@@@@@@@@&&&&  &&&   @@@@@@@@@@     
                 @@@@@@&&&&@@@@@@@@@@@@@@@@@@     &&  @@@@@@@@@@     
                    @@&@@@&&@@@@@@@@@@@@@@@@@      && @@@@@@@@@@     
                     &&&@@@@@@&@@@@@@@@@@@@@@@    &&&   @@@@@@.      
                      &&&&&@@@@@@&&@@@@@@@@@@@@@&&&&&               
                      &&&&&&&@@@@@@@@@@@@@@@@@@&&&&&&@@@                
                       (&&&&&&&@@@@&@@@@@@@@@@&&&&&& #@@@@@.            
                         &&&&&&&&&@@@@@&&@@@&&&&&&&     @@@@@@/         
                           &&&&&&&&@@@@@@@@@&&&&&         @@@@@@@       
                              &&&&&&&&&&&&@@@@@@@@@        @@@@@@@@,    
                                   &&&&&&&,     @@@@@@@@@@@@@@@@@@@@@@
                                                        &@@@@ @@@@@@@.

                                            
                            Enceladus project                                                               
                                                                                
endef
export HEADER

#------------------------------------------------------------------#
#----------------------- Configuration flags ----------------------#
#------------------------------------------------------------------#
#-------------------------- Reset the IOP -------------------------#
RESET_IOP = 1
#---------------------- enable DEBUGGING MODE ---------------------#
DEBUG = 0
#----------------------- Set IP for PS2Client ---------------------#
PS2LINK_IP = 192.168.1.10
#------------------------------------------------------------------#

EE_BIN = enceladus.elf
EE_BIN_PKD = enceladus_pkd.elf

EE_LIBS = -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib/ -Lmodules/ds34bt/ee/ -Lmodules/ds34usb/ee/ -lpatches -lfileXio -lpad -ldebug -llua -lmath3d -ljpeg -lfreetype -lgskit_toolkit -lgskit -ldmakit -lpng -lz -lmc -laudsrv -lelf-loader -lds34bt -lds34usb

EE_INCS += -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/freetype2 -I$(PS2SDK)/ports/include/zlib

EE_INCS += -Imodules/ds34bt/ee -Imodules/ds34usb/ee

EE_CFLAGS   += -Wno-sign-compare -fno-strict-aliasing -fno-exceptions -DLUA_USE_PS2
EE_CXXFLAGS += -Wno-sign-compare -fno-strict-aliasing -fno-exceptions -DLUA_USE_PS2

ifeq ($(RESET_IOP),1)
EE_CXXFLAGS += -DRESET_IOP
endif

ifeq ($(DEBUG),1)
EE_CXXFLAGS += -DDEBUG
endif

BIN2S = $(PS2SDK)/bin/bin2s

#-------------------------- App Content ---------------------------#
EXT_LIBS = modules/ds34usb/ee/libds34usb.a modules/ds34bt/ee/libds34bt.a

APP_CORE = main.o system.o pad.o graphics.o render.o \
		   calc_3d.o gsKit3d_sup.o atlas.o fntsys.o md5.o \
		   sound.o

LUA_LIBS =	luaplayer.o luasound.o luacontrols.o \
			luatimer.o luaScreen.o luagraphics.o \
			luasystem.o luaRender.o

IOP_MODULES = iomanx.o filexio.o \
			  sio2man.o mcman.o mcserv.o padman.o libsd.o \
			  usbd.o audsrv.o bdm.o bdmfs_vfat.o \
			  usbmass_bd.o cdfs.o ds34bt.o ds34usb.o

EMBEDDED_RSC = boot.o

EE_OBJS = $(IOP_MODULES) $(EMBEDDED_RSC) $(APP_CORE) $(LUA_LIBS)

EE_OBJS_DIR = obj/
EE_SRC_DIR = src/
EE_ASM_DIR = asm/
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%) # remap all EE_OBJ to obj subdir

#------------------------------------------------------------------#
all: $(EXT_LIBS) $(EE_BIN)
	@echo "$$HEADER"

	echo "Building $(EE_BIN)..."
	$(EE_STRIP) $(EE_BIN)

	echo "Compressing $(EE_BIN_PKD)...\n"
	ps2-packer $(EE_BIN) $(EE_BIN_PKD) > /dev/null
	
	mv $(EE_BIN) bin/
	mv $(EE_BIN_PKD) bin/
#--------------------- Embedded ressources ------------------------#

$(EE_ASM_DIR)boot.s: etc/boot.lua | $(EE_ASM_DIR)
	echo "Embedding boot script..."
	$(BIN2S) $< $@ bootString

# Images
EMBED/%.s: EMBED/%.png
	$(BIN2S) $< $@ $(shell basename $< .png)
	echo "Embedding $< Image..."
#------------------------------------------------------------------#


#-------------------- Embedded IOP Modules ------------------------#
$(EE_ASM_DIR)iomanx.s: $(PS2SDK)/iop/irx/iomanX.irx | $(EE_ASM_DIR)
	echo "Embedding iomanX Driver..."
	$(BIN2S) $< $@ iomanX_irx

$(EE_ASM_DIR)filexio.s: $(PS2SDK)/iop/irx/fileXio.irx | $(EE_ASM_DIR)
	echo "Embedding fileXio Driver..."
	$(BIN2S) $< $@ fileXio_irx

$(EE_ASM_DIR)sio2man.s: $(PS2SDK)/iop/irx/sio2man.irx | $(EE_ASM_DIR)
	echo "Embedding SIO2MAN Driver..."
	$(BIN2S) $< $@ sio2man_irx
	
$(EE_ASM_DIR)mcman.s: $(PS2SDK)/iop/irx/mcman.irx | $(EE_ASM_DIR)
	echo "Embedding MCMAN Driver..."
	$(BIN2S) $< $@ mcman_irx

$(EE_ASM_DIR)mcserv.s: $(PS2SDK)/iop/irx/mcserv.irx | $(EE_ASM_DIR)
	echo "Embedding MCSERV Driver..."
	$(BIN2S) $< $@ mcserv_irx

$(EE_ASM_DIR)padman.s: $(PS2SDK)/iop/irx/padman.irx | $(EE_ASM_DIR)
	echo "Embedding PADMAN Driver..."
	$(BIN2S) $< $@ padman_irx
	
$(EE_ASM_DIR)libsd.s: $(PS2SDK)/iop/irx/libsd.irx | $(EE_ASM_DIR)
	echo "Embedding LIBSD Driver..."
	$(BIN2S) $< $@ libsd_irx

$(EE_ASM_DIR)usbd.s: $(PS2SDK)/iop/irx/usbd.irx | $(EE_ASM_DIR)
	echo "Embedding USB Driver..."
	$(BIN2S) $< $@ usbd_irx

$(EE_ASM_DIR)audsrv.s: $(PS2SDK)/iop/irx/audsrv.irx | $(EE_ASM_DIR)
	echo "Embedding AUDSRV Driver..."
	$(BIN2S) $< $@ audsrv_irx

$(EE_ASM_DIR)bdm.s: $(PS2SDK)/iop/irx/bdm.irx | $(EE_ASM_DIR)
	echo "Embedding Block Device Manager(BDM)..."
	$(BIN2S) $< $@ bdm_irx

$(EE_ASM_DIR)bdmfs_vfat.s: $(PS2SDK)/iop/irx/bdmfs_vfat.irx | $(EE_ASM_DIR)
	echo "Embedding BDM VFAT Driver..."
	$(BIN2S) $< $@ bdmfs_vfat_irx

$(EE_ASM_DIR)usbmass_bd.s: $(PS2SDK)/iop/irx/usbmass_bd.irx | $(EE_ASM_DIR)
	echo "Embedding BD USB Mass Driver..."
	$(BIN2S) $< $@ usbmass_bd_irx

$(EE_ASM_DIR)cdfs.s: $(PS2SDK)/iop/irx/cdfs.irx | $(EE_ASM_DIR)
	$(BIN2S) $< $@ cdfs_irx

modules/ds34bt/ee/libds34bt.a: modules/ds34bt/ee
	$(MAKE) -C $<

modules/ds34bt/iop/ds34bt.irx: modules/ds34bt/iop
	$(MAKE) -C $<

$(EE_ASM_DIR)ds34bt.s: modules/ds34bt/iop/ds34bt.irx | $(EE_ASM_DIR)
	$(BIN2S) $< $@ ds34bt_irx

modules/ds34usb/ee/libds34usb.a: modules/ds34usb/ee
	$(MAKE) -C $<

modules/ds34usb/iop/ds34usb.irx: modules/ds34usb/iop
	$(MAKE) -C $<

$(EE_ASM_DIR)ds34usb.s: modules/ds34usb/iop/ds34usb.irx | $(EE_ASM_DIR)
	$(BIN2S) $< $@ ds34usb_irx
	
#------------------------------------------------------------------#

$(EE_OBJS_DIR):
	@mkdir -p $@

$(EE_ASM_DIR):
	@mkdir -p $@

debug: $(EE_BIN)
	echo "Building $(EE_BIN) with debug symbols..."

clean:

	@echo "\nCleaning $(EE_BIN)..."
	rm -f bin/$(EE_BIN)

	@echo "\nCleaning $(EE_BIN_PKD)..."
	rm -f bin/$(EE_BIN_PKD)

	@echo "Cleaning obj dir"
	@rm -rf $(EE_OBJS_DIR)
	@echo "Cleaning asm dir"
	@rm -rf $(EE_ASM_DIR)
	
	$(MAKE) -C modules/ds34usb clean
	$(MAKE) -C modules/ds34bt clean
	
	
	echo "Cleaning embedded Resources..."
	rm -f $(EMBEDDED_RSC)

rebuild: clean all

run:
	cd bin; ps2client -h $(PS2LINK_IP) execee host:$(EE_BIN)
       
reset:
	ps2client -h $(PS2LINK_IP) reset   

$(EE_OBJS_DIR)%.o: $(EE_SRC_DIR)%.c | $(EE_OBJS_DIR)
	@echo "  - $@"
	@$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_OBJS_DIR)%.o: $(EE_ASM_DIR)%.s | $(EE_OBJS_DIR)
	@echo "  - $@"
	@$(EE_AS) $(EE_ASFLAGS) $< -o $@

$(EE_OBJS_DIR)%.o: $(EE_SRC_DIR)%.cpp | $(EE_OBJS_DIR)
	@echo "  - $@"
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
