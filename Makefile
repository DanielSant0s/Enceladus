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

#------------------------------------------------------------------
# -- Configuration flags --
#------------------------------------------------------------------
# -- Reset the IOP --
RESET_IOP = 0
# -- enable DEBUGGING MODE --
DEBUG = 0
# -- Build an elf from a unique script --
STANDALONE = 0
# -------------------------

EE_BIN = bin/enceladus.elf
EE_BIN_PKD = bin/enceladus.elf

EE_LIBS = -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib/ -lpatches -lfileXio -lpad -ldebug -llua -ljpeg -lfreetype -ldraw -lmath3d -lpacket2 -lgskit_toolkit -lgskit -ldmakit -lpng -lz -lmc -lmikmod

EE_INCS += -I$(PS2SDK)/ports/include 
EE_INCS += -I$(PS2SDK)/ports/include/zlib -I$(PS2DEV)/gsKit/include
EE_INCS += -I$(PS2SDK)/ports/include/freetype2

EE_CFLAGS   += -fno-strict-aliasing
EE_CXXFLAGS += -fno-strict-aliasing -fno-exceptions -fno-rtti -DLUA_USE_PS2

ifeq ($(RESET_IOP),1)
EE_CXXFLAGS += -DRESET_IOP
endif

ifeq ($(DEBUG),1)
EE_CXXFLAGS += -DDEBUG
endif

BIN2S = $(PS2SDK)/bin/bin2s

# -- PS2 specific source code --
EE_OBJS += src/md5.o
EE_OBJS += src/usbd.o
EE_OBJS += src/usbhdfsd.o

# -- LuaPlayer specific source code --
EE_OBJS += src/main.o
EE_OBJS += src/utility.o
EE_OBJS += src/graphics.o
EE_OBJS += src/atlas.o
EE_OBJS += src/fntsys.o
#EE_OBJS += src/sound.o
EE_OBJS += src/luaplayer.o
#EE_OBJS += src/luasound.o
EE_OBJS += src/luacontrols.o
EE_OBJS += src/luatimer.o
EE_OBJS += src/luaScreen.o
EE_OBJS += src/luagraphics.o
EE_OBJS += src/lua3d.o
EE_OBJS += src/luasystem.o

ifeq ($(STANDALONE), 1)
EE_OBJS += standalone/app/luaScript.o
endif

# -- Embedded ressources ---
src/main.o: src/boot.cpp src/lualogo.cpp

src/lualogo.cpp: etc/lualogo.raw
	echo "Embedding splash screen..."
	$(PS2SDK)/bin/bin2c $< $@ rawlualogo

src/boot.cpp: etc/boot.lua
	echo "Embedding Lua boot script..."
	$(PS2SDK)/bin/bin2c $< $@ bootString

ifeq ($(STANDALONE), 1)
standalone/app/luaScript.o: standalone/app/script.lua
	$(PS2SDK)/bin/bin2o $< $@ scriptLua
endif	


# -- Embedded Irx(s) ---------
src/usbhdfsd.s: $(PS2SDK)/iop/irx/usbhdfsd.irx
	echo "Embedding IOP Modules..."
	$(BIN2S) $< $@ usbhdfsd_irx

src/usbd.s: $(PS2SDK)/iop/irx/usbd.irx
	$(BIN2S) $< $@ usbd_irx
# ----------------------------

all: $(EE_BIN)
	@echo "$$HEADER"
	echo "Building $(EE_BIN)..."
	$(EE_STRIP) $(EE_BIN)

	echo "Compressing $(EE_BIN_PKD)...\n"
	ps2-packer $(EE_BIN) $(EE_BIN_PKD) > /dev/null

debug: $(EE_BIN)
	echo "Building $(EE_BIN) with debug symbols..."

clean:
	echo "\nCleaning ELFs and objects..."
	rm -f $(EE_BIN) $(EE_OBJS) $(EE_BIN_PKD)
	echo "Cleaning embedded IOP modules..."
	rm -f src/usbhdfsd.s
	rm -f src/usbd.s
	echo "Cleaning embedded boot script..."
	rm -f src/boot.cpp
	echo "Cleaning embedded splash screen...\n"
	rm -f src/lualogo.cpp

rebuild: clean all

run:
	ps2client -h 192.168.1.10 execee host:$(EE_BIN)
	
run_usb:
	ps2client -h 192.168.0.100 execee host:$(EE_BIN) mass:/System/system.lua
       
reset:
	ps2client -h 192.168.1.10 reset   


include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
