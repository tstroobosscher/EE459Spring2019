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
Note, any argument of an invalid pin type (The arg number is out of range or
addresses an invalid pin or addresses and input pin will cause the function to
return -1, indicating a failure, and no action will be performed)

Also, the register assignments in that file should remail the same, but we should
absolutely change the default data direction at some point. Right now they are
all configured to output. To switch just write:
```
	struct {
	...
	#,	PORT_REG, #, ATMEL_OUTPUT/ATMEL_INPUT
	...
	}
```

And somewhat confusingly, in the pins header file, there are another set of
definitions for the physical pins. This just defines Macros to use instead of
actual integer numbers for spcific pin operations. I try as hard as possible
to avoid unitless and dimensionless "magic numbers" in any programming
language. It just ends up being confusing later on. But to reiterate, the
structure definition in pins.c defines the actual memory allocation and pin
behavior that the board will initialize itself with

## Modules

### Flash Storage
This will probably incorporate some peripheral ICs to facilitate some flavor
of multiplexed IO (SPI, I2C, UART) 

Will need to research the requirements and design this module

### Serial Stream
This will need to stream realtime data over a serial interface that we can
use for both data projection (like on a screen or something) and debugging.
The 328 boards come with UART support, but we will need at least 2 UART
modules to balance between input and command processing on the 327 and
streaming output. 
Once this module is in place, extending some kind of complex interface over
this data stream should be pretty straightforward (Bluetooth, Wi-Fi, etc)

### ECU Communication (Hardware and Software)
We have the broadstrokes of this one laid out. The ELM327 documentation comes
with more than enough information to lay out a complete circuit. This part just
needs the parts ordered, the connections routed, and the software written.

Luckily, we have an assembled 327 circuit that we can start to use for software
development in parallel to hardware development. Thus, this module can be 
broken up into 2 submodules, the hardware and the software.

### Extra Sensory Information
Here we can leverage the 328s on board ADC and input support to maximize our
data collection. What else could we add? GPS? Temp? Acceleration? Magnometry?
What would our wish list for peripheral sensory input look like?

### RTOS Optimization
Since many of these I/O devices have varying amounts of latency and bandwidth
requirements (A write operation to the flash can be stalled to process a read
operation from the ELM327), it may be useful to implement some kind of
scheduling mechanism to ensure a balanced and flexible distribution of I/O and
CPU resources.

EDIT:
We can emulate this functionality with the various hardware interrupts the 328
supports:\s\s
	+ INT0_vect		: Generic external\s\s
	+ INT1_vect		: Generic external\s\s
	+ PCINT0_vect		: Pin change PORTB\s\s
	+ PCINT1_vect		: Pin change PORTC\s\s
	+ PCINT2_vect		: Pin change PORTD\s\s
	+ SPI_STC_vect	: SPI transfer complete\s\s
	+ USART_RX_vect	: UART RX complete\s\s
	+ USART_UDRE_vect	: UART data register empty\s\s
	+ USART_TX_vect	: UART TX complete\s\s
	+ ADC_vect		: ADC conversion complete\s\s
	+ TWI_vect		: I2C? Might be useful\s\s
	+ and all the various timer interrupts\s\s

I have no idea what this would entail, but it wouldn't hurt to do some research
on the topic to see if there are any open source libraries that we could 
possibly take inspiration from.

## Useful Commands

### Creating a snapshot of the repo
```
	git archive -o snapshot.zip --format zip HEAD
```