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

EE_LIBS = -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib/ -Lmodules/ds34bt/ee/ -Lmodules/ds34usb/ee/ -lpatches -lfileXio -lpad -ldebug -llua -ljpeg -lfreetype -lgskit_toolkit -lgskit -ldmakit -lpng -lz -lmc -laudsrv -lelf-loader -lds34bt -lds34usb

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

APP_CORE = src/main.o src/graphics.o src/atlas.o \
		   src/fntsys.o src/md5.o

LUA_LIBS =	src/luaplayer.o src/luasound.o src/luacontrols.o \
			src/luatimer.o src/luaScreen.o src/luagraphics.o \
			src/luasystem.o

IOP_MODULES = src/usbd.o src/audsrv.o src/bdm.o src/bdmfs_vfat.o \
			  src/usbmass_bd.o src/cdfs.o src/ds34bt.o src/ds34usb.o

EMBEDDED_RSC = src/boot.o

EE_OBJS = $(IOP_MODULES) $(EMBEDDED_RSC) $(APP_CORE) $(LUA_LIBS)

#------------------------------------------------------------------#


#--------------------- Embedded ressources ------------------------#

src/boot.s: etc/boot.lua
	echo "Embedding boot script..."
	$(BIN2S) $< $@ bootString

#------------------------------------------------------------------#


#-------------------- Embedded IOP Modules ------------------------#

src/usbd.s: $(PS2SDK)/iop/irx/usbd.irx
	echo "Embedding USB Driver..."
	$(BIN2S) $< $@ usbd_irx

src/audsrv.s: $(PS2SDK)/iop/irx/audsrv.irx
	echo "Embedding AUDSRV Driver..."
	$(BIN2S) $< $@ audsrv_irx

src/bdm.s: $(PS2SDK)/iop/irx/bdm.irx
	echo "Embedding Block Device Manager(BDM)..."
	$(BIN2S) $< $@ bdm_irx

src/bdmfs_vfat.s: $(PS2SDK)/iop/irx/bdmfs_vfat.irx
	echo "Embedding BDM VFAT Driver..."
	$(BIN2S) $< $@ bdmfs_vfat_irx

src/usbmass_bd.s: $(PS2SDK)/iop/irx/usbmass_bd.irx
	echo "Embedding BD USB Mass Driver..."
	$(BIN2S) $< $@ usbmass_bd_irx

src/cdfs.s: $(PS2SDK)/iop/irx/cdfs.irx
	echo "Embedding CDFS Driver..."
	$(BIN2S) $< $@ cdfs_irx

modules/ds34bt/ee/libds34bt.a: modules/ds34bt/ee
	$(MAKE) -C $<

modules/ds34bt/iop/ds34bt.irx: modules/ds34bt/iop
	echo "Building DS3/4 Bluetooth Driver..."
	$(MAKE) -C $<

src/ds34bt.s: modules/ds34bt/iop/ds34bt.irx
	echo "Embedding DS3/4 Bluetooth Driver..."
	$(BIN2S) $< $@ ds34bt_irx

modules/ds34usb/ee/libds34usb.a: modules/ds34usb/ee
	$(MAKE) -C $<

modules/ds34usb/iop/ds34usb.irx: modules/ds34usb/iop
	echo "Building DS3/4 USB Driver..."
	$(MAKE) -C $<

src/ds34usb.s: modules/ds34usb/iop/ds34usb.irx
	echo "Embedding DS3/4 USB Driver..."
	$(BIN2S) $< $@ ds34usb_irx
	
#------------------------------------------------------------------#

all: $(EXT_LIBS) $(EE_BIN)
	@echo "$$HEADER"

	echo "Building $(EE_BIN)..."
	$(EE_STRIP) $(EE_BIN)

	echo "Compressing $(EE_BIN_PKD)...\n"
	ps2-packer $(EE_BIN) $(EE_BIN_PKD) > /dev/null
	
	mv $(EE_BIN) bin/
	mv $(EE_BIN_PKD) bin/

debug: $(EE_BIN)
	echo "Building $(EE_BIN) with debug symbols..."

clean:

	echo "\nCleaning $(EE_BIN)..."
	rm -f bin/$(EE_BIN)

	echo "\nCleaning $(EE_BIN_PKD)..."
	rm -f bin/$(EE_BIN_PKD)

	echo "\nCleaning objects..."
	rm -f $(EE_OBJS)
	
	echo "Cleaning Block Device Manager(BDM)..."
	rm -f src/bdm.s
	
	echo "Cleaning USB Driver..."
	rm -f src/usbd.s
	
	echo "Embedding AUDSRV Driver..."
	rm -f src/audsrv.s
	
	echo "Cleaning BDM VFAT Driver..."
	rm -f src/bdmfs_vfat.s
	
	echo "Embedding BD USB Mass Driver..."
	rm -f src/usbmass_bd.s
	
	echo "Cleaning CDFS Driver..."
	rm -f src/cdfs.s

	echo "Cleaning DS3/4 Drivers..."
	rm -f src/ds34bt.s
	rm -f src/ds34usb.s
	$(MAKE) -C modules/ds34usb clean
	$(MAKE) -C modules/ds34bt clean
	
	echo "Cleaning embedded boot script..."
	rm -f src/boot.s

rebuild: clean all

run:
	cd bin; ps2client -h $(PS2LINK_IP) execee host:$(EE_BIN)
       
reset:
	ps2client -h $(PS2LINK_IP) reset   

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
