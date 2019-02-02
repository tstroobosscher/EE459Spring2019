# EE459Spring2019
Group 17 code

## Makefile/Compiler

The compiler flags were set up to target the ISP programmer from a linux machine
so it might not work on other systems. For some reason the USBtiny doesn't 
enumerate a tty device, the avr toolchain interfaces with it through the usb bus 
infrastructure with the VID:PID numbers. The programmer still works but its a
little different than the 328.

C compiler set to C99
```
	-std=c99
```

## Pins

Pin definitions are set in the pin structure of the pins.c file. The structure 
maps the physical pin numbers to their individual registers. So instead of 
manually bit shifting into the PORTX reg's, we can spend a few extra bytes of
code to abstract the bit level writing. Now, all we do to write to a pin is:
```
	assert_pin([physical pin number])
	deassert_pin([physical pin number])
```
The register assignments in that file should remail the same, but we should
absolutely change the default data direction at some point. Right now they are
all configured to output. To switch just write:
```
	struct {
	...
	#,	PORT_REG, #, ATMEL_OUTPUT/ATMEL_INPUT
	...
	}
```

## Useful Commands

### Creating a snapshot of the repo
```
	git archive -o snapshot.zip --format zip HEAD
```