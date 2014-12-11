# Makefile for building Arduino sketches with Arduino 1.0.
# Makefile-arduino v0.5 by Akkana Peck <akkana@shallowsky.com>
#
# Adapted from a long-ago Arduino 0011 Makefile by mellis, eighthave, oli.keller
# Modified by juanmb
#
# This Makefile allows you to build sketches from the command line
# without the Arduino environment (or Java).
#
# Detailed instructions for using this Makefile:
#
#  1. Copy this file into the folder with your sketch.
#	  There should be a file with the extension .ino (e.g. blink.ino).
#	  cd into this directory.
#
#  2. Below, modify the line containing "TARGET" to refer to the name of
#	  of your program's file without an extension (e.g. TARGET = foo).
#
#  3. Modify the line containg "ARDUINO_DIR" to point to the directory that
#	  contains the Arduino installation (for example, under Mac OS X, this
#	  might be /Applications/arduino-0022). If it's in your home directory,
#	  you can include $(HOME) as part of the path.
#
#  4. Set ARDUINO_MODEL to your Arduino model.
#	  I have only tested uno, atmega328, and diecimila,
#	  but there are lots of other options:
#	  see $(ARDUINO_DIR)/hardware/arduino/boards.txt for a list.
#	  If you're using one of the other models you might have to modify
#	  PORT; let me know if you do, so I can update this Makefile.
#
#  5. Run "make" to compile/verify your program.
#
#  6. Run "make program" (and reset your Arduino if it requires it)
#	  to flash your program to the Arduino board.
#

# Name of the program and source .pde file:
TARGET = opendaq
VERSION = 0.1

# Standard Arduino libraries it will import, e.g. LiquidCrystal:
ARDLIBS =

# User-specified (in ~/sketchbook/libraries/) libraries (untested):
USERLIBS =

# Where do you keep the official Arduino software package?
ARDUINO_DIR = /usr/share/arduino

# Arduino model:
ARDUINO_MODEL = opendaq_m

# Where are tools like avr-gcc located on your system?
#AVR_TOOLS_PATH = $(ARDUINO_DIR)/hardware/tools/avr/bin
AVR_TOOLS_PATH = /usr/bin

############################################################################
# Below here nothing should need to be changed. Cross your fingers!

#
# Set up values according to what the IDE uses:
#
DOWNLOAD_RATE = $(shell grep $(ARDUINO_MODEL) $(ARDUINO_DIR)/hardware/arduino/boards.txt | grep upload.speed | sed 's/.*=//')
MCU = $(shell grep $(ARDUINO_MODEL) $(ARDUINO_DIR)/hardware/arduino/boards.txt | grep build.mcu | sed 's/.*=//')
F_CPU = $(shell grep $(ARDUINO_MODEL) $(ARDUINO_DIR)/hardware/arduino/boards.txt | grep build.f_cpu | sed 's/.*=//')

# man avrdude says to use arduino, but the IDE mostly uses stk500.
# One rumor says that the difference is that arduino does an auto-reset,
# stk500 doesn't.
# Might want to grep for upload.protocol as with previous 3 values.
AVRDUDE_PROGRAMMER = arduino

# Port isn't set anywhere I can find in the IDE, so have to hardwire it here.
ifeq "$(ARDUINO_MODEL)" "uno"
PORT = /dev/ttyACM*
else
PORT = /dev/ttyUSB*
endif

# This has only been tested on standard variants. I'm guessing
# at what mega and micro might need; other possibilities are
# leonardo and "eightanaloginputs".
VARIANTS_PATH=$(ARDUINO_DIR)/hardware/arduino/variants
ifeq "$(ARDUINO_MODEL)" "mega"
ARDUINO_VARIANT=$(VARIANTS_PATH)/mega
else
ifeq "$(ARDUINO_MODEL)" "micro"
ARDUINO_VARIANT=$(VARIANTS_PATH)/micro
else
ifeq "$(ARDUINO_MODEL)" "opendaq_m"
ARDUINO_VARIANT=$(VARIANTS_PATH)/opendaq_m
else
ifeq "$(ARDUINO_MODEL)" "opendaq_s"
ARDUINO_VARIANT=$(VARIANTS_PATH)/opendaq_s
else
ARDUINO_VARIANT=$(VARIANTS_PATH)/standard
endif
endif
endif
endif

CWD = $(shell pwd)
CWDBASE = $(shell basename `pwd`)
TARFILE = $(TARGET)-$(VERSION).tar.gz

ARDUINO_CORE = $(ARDUINO_DIR)/hardware/arduino/cores/arduino
SRC = \
	$(ARDUINO_CORE)/wiring.c \
	$(ARDUINO_CORE)/wiring_analog.c $(ARDUINO_CORE)/wiring_digital.c \
	$(ARDUINO_CORE)/wiring_pulse.c \
	$(ARDUINO_CORE)/wiring_shift.c $(ARDUINO_CORE)/WInterrupts.c \
	$(wildcard *.c)

CXXSRC = $(ARDUINO_CORE)/HardwareSerial.cpp $(ARDUINO_CORE)/WMath.cpp \
	$(ARDUINO_CORE)/WString.cpp $(ARDUINO_CORE)/Print.cpp \
	$(foreach l,$(ARDLIBS),$(ARDUINO_DIR)/libraries/$l/$l.cpp) \
	$(foreach l,$(USERLIBS),$(HOME)/sketchbook/libraries/$l/$l.cpp) \
	$(wildcard *.cpp)

# Tried to use patsubst, but gmake ignores the second occurrence of %.
# http://www.mail-archive.com/bug-make@gnu.org/msg00426.html
#	 $(patsubst %,$(ARDUINO_DIR)/libraries/%/%.cpp,$(ARDLIBS)) \
#	 $(patsubst %,$(HOME)/sketchbook/libraries/%/%.cpp,$(USERLIBS))

FORMAT = ihex

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU) #-DSERIAL_DEBUG

# Include directories
CINCS = -I$(ARDUINO_CORE) -I$(ARDUINO_VARIANT)  $(patsubst %,-I$(ARDUINO_DIR)/libraries/%,$(ARDLIBS)) $(patsubst %,-I$(HOME)/sketchbook/libraries/%,$(USERLIBS))

# Compiler flag to set the C Standard level.
# c89	- "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99	- ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
#CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
#CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEFS) $(CINCS) -O$(OPT)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
LDFLAGS = -lm

# Programming support using avrdude. Settings and variables.
AVRDUDE_WRITE_FLASH = -U flash:w:applet/$(TARGET).hex
#AVRDUDE_FLAGS = -V -F -C $(ARDUINO_DIR)/hardware/tools/avr/etc/avrdude.conf
AVRDUDE_FLAGS = -V -F -C /etc/avrdude.conf \
	-p $(MCU) -P $(PORT) -c $(AVRDUDE_PROGRAMMER) \
	-b $(DOWNLOAD_RATE)

# Program settings
CC = $(AVR_TOOLS_PATH)/avr-gcc
CXX = $(AVR_TOOLS_PATH)/avr-g++
OBJCOPY = $(AVR_TOOLS_PATH)/avr-objcopy
OBJDUMP = $(AVR_TOOLS_PATH)/avr-objdump
AR	= $(AVR_TOOLS_PATH)/avr-ar
SIZE = $(AVR_TOOLS_PATH)/avr-size
NM = $(AVR_TOOLS_PATH)/avr-nm
AVRDUDE = $(AVR_TOOLS_PATH)/avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)

# Default target.
all: build sizeafter

test:
	@echo CXXSRC = $(CXXSRC)

build: elf hex

applet/$(TARGET).cpp: $(TARGET).ino
	# Here is the "preprocessing".
	# It creates a .cpp file based with the same name as the .ino file.
	# On top of the new .cpp file comes the WProgram.h header.
	# At the end there is a generic main() function attached,
	# plus special magic to get around the pure virtual error
	# undefined reference to `__cxa_pure_virtual' from Print.o.
	# Then the .cpp file will be compiled. Errors during compile will
	# refer to this new, automatically generated, file.
	# Not the original .ino file you actually edit...

	test -d applet || mkdir applet
	echo '#include "Arduino.h"' > applet/$(TARGET).cpp
	cat $(TARGET).ino >> applet/$(TARGET).cpp
	echo 'extern "C" void __cxa_pure_virtual() { while (1) ; }' >> applet/$(TARGET).cpp
	cat $(ARDUINO_CORE)/main.cpp >> applet/$(TARGET).cpp

elf: applet/$(TARGET).elf
hex: applet/$(TARGET).hex
eep: applet/$(TARGET).eep
lss: applet/$(TARGET).lss
sym: applet/$(TARGET).sym

# Program the device.
program: applet/$(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

	# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) applet/$(TARGET).hex
ELFSIZE = $(SIZE)  applet/$(TARGET).elf
sizebefore:
	@if [ -f applet/$(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(HEXSIZE); echo; fi

sizeafter:
	@if [ -f applet/$(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(HEXSIZE); echo; fi

# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
	--change-section-address .data-0x800000 \
	--change-section-address .bss-0x800000 \
	--change-section-address .noinit-0x800000 \
	--change-section-address .eeprom-0x810000

coff: applet/$(TARGET).elf
	$(COFFCONVERT) -O coff-avr applet/$(TARGET).elf $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr applet/$(TARGET).elf $(TARGET).cof

.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@

	# Link: create ELF output file from library.
applet/$(TARGET).elf: applet/$(TARGET).cpp applet/core.a
	$(CC) $(ALL_CFLAGS) -o $@ applet/$(TARGET).cpp -L. applet/core.a $(LDFLAGS)


print:
	@echo $(OBJ)

applet/core.a: $(OBJ)
	@for i in $(OBJ); do echo $(AR) rcs applet/core.a $$i; $(AR) rcs applet/core.a $$i; done

# Compile: create object files from C++ source files.
.cpp.o:
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Target: clean project.
clean:
	$(REMOVE) applet/$(TARGET).cpp applet/$(TARGET).hex applet/$(TARGET).eep applet/$(TARGET).cof applet/$(TARGET).elf \
	applet/$(TARGET).map applet/$(TARGET).sym applet/$(TARGET).lss applet/core.a \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)

tar: $(TARFILE)

$(TARFILE):
	( cd .. && \
		tar czvf $(TARFILE) --exclude=applet --owner=root $(CWDBASE) && \
		mv $(TARFILE) $(CWD) && \
		echo Created $(TARFILE) \
	)

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend sizebefore sizeafter
