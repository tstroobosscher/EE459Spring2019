DEVICE     = atmega1284p

# 16 MHz is the arduino's pcb clock speed. Our clock is much slower than this
# and this line of code will actually cause bugs in our time sensitive routines
# (like the UART/Serial code) 

# 7.3728MHz oscillator !  

CLOCK      ?= 7372800
PROGRAMMER = -c usbtiny -p m1284p -F
PORT ?= /dev/ttyACM0
ARDUINO_PROGRAMMER = -c arduino -b 115200 -P $(PORT)
OBJECTS    = main.o \
             uart.o \
             pins.o \
             utils.o \
             list.o \
             elm.o \
             fifo.o \
             obd.o \
             astdio.o \
             spi.o \
             sd.o \
             io.o \
             fat.o
FUSES      = -U hfuse:w:0xde:m -U lfuse:w:0xff:m -U efuse:w:0x05:m

# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc \
			-std=c99 \
			-Wl,-u,vfprintf \
			-lprintf_flt \
			-lm \
			-Wall \
			-Os \
			-DF_CPU=$(CLOCK) \
			-mmcu=$(DEVICE) \
			-Wl,-Map=myprog.map \
			-Wl,--cref \


PWD = $(shell pwd)
COMMIT = $(shell git rev-parse HEAD)
BRANCH = $(shell git rev-parse --abbrev-ref HEAD)

STATS_FILE = $(PWD)/avr_size.txt

# symbolic targets:
all:	main.hex

.c.o:
	@$(COMPILE) -c $< -o $@

.S.o:
	@$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	@$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

arduino:  all
	avrdude $(ARDUINO_PROGRAMMER) -p $(DEVICE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	@rm -f main.hex main.elf $(OBJECTS)
	@rm -f $(STATS_FILE)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	@rm -f main.hex
	@avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	@echo "Commit: $(COMMIT)\n" > $(STATS_FILE)
	@avr-size --format=avr --mcu=$(DEVICE) main.elf >> $(STATS_FILE)
	@cat $(STATS_FILE)
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c

.PHONY: utils
utils:
	@echo Lines: $(shell git ls-files | grep -v resources | \
										grep -v test.img | \
										grep -v macbeth.txt | \
										xargs cat | wc -l)
