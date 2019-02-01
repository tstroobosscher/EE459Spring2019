# EE459Spring2019
Group 17 code

The compiler flags were set up to target the ISP programmer from a linux machine
so it might not work on other systems. For some reason the USBtiny doesn't 
enumerate a tty device, the avr toolchain interfaces with it through the usb bus 
infrastructure with the VID:PID numbers. The programmer still works but its a
little different than the 328.

C compiler set to C99

Useful commands:
	git archive -o snapshot.zip --format zip
