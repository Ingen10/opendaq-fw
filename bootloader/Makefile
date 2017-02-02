###############################################
# Makefile for building the openDAQ bootloader
###############################################

NAME=opendaq_boot
MCU=atmega644p

CPP=avr-g++
OBJCOPY=avr-objcopy

# Options common to compile, link and assembly rules
COMMON=-mmcu=$(MCU)

# Compile options common for all C compilation units.
CFLAGS= $(COMMON)
CFLAGS+=-Wall -gdwarf-2 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS+=-MD -MP -MT $(*F).o

# Linker flags
LDFLAGS=$(COMMON)
LDFLAGS+=-Wl,-Map=$(NAME).map
LDFLAGS+=-Wl,-section-start=.text=0xe000

# Intel Hex file production flags
HEX_FLASH_FLAGS=-R .eeprom -R .fuse -R .lock -R .signature
HEX_EEPROM_FLAGS=-j .eeprom
HEX_EEPROM_FLAGS+=--set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS+=--change-section-lma .eeprom=0 --no-change-warnings

## Objects that must be built in order to link
OBJECTS=$(NAME).o

all: $(NAME).hex $(NAME).eep $(NAME).lss

$(NAME).hex: $(NAME).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS) $< $@

$(NAME).elf: $(OBJECTS)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $@

.cpp.o:
	$(CPP) $(INCLUDES) $(CFLAGS) -c $<

%.eep: $(NAME).elf
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(NAME).elf
	avr-objdump -h -S $< > $@

.PHONY: clean
clean:
	rm -f $(OBJECTS) *.elf *.hex *.eep *.lss *.map *.d
