###############################################################################
#
#	EE459 - 2560 test code
#
###############################################################################

SOURCE = hello_world.c

TARGET = hello_world

OBJECTS = hello_world.o

TARGET_FLAGS = -o

DEVICE =  atmega2560

CLOCK = 16000000

COMPILE = avr-gcc -std=c99 -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

PROGRAMMER = -c usbtiny -p m2560 -F

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

all: main.hex

.c.o:
	@$(COMPILE) -c $< -o $@

.S.o:
	@$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	@$(COMPILE) -S $< -o $@

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	@rm -f main.hex
	@avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	@avr-size --format=avr --mcu=$(DEVICE) main.elf

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	@rm -f main.hex main.elf $(OBJECTS)
