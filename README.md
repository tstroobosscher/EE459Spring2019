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
	#,	PORT_REG, #, ATMEL_OUTPUT/ATMEL_INPUT, PULL_UP/DN
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

SD cards use SPI, integrating SPI will eat up 4 pins:
	** MOSI
	** MISO
	** CLK
	** CS

### Serial Stream
This will need to stream realtime data over a serial interface that we can
use for both data projection (like on a screen or something) and debugging.
The 328 boards come with UART support, but we will need at least 2 UART
modules to balance between input and command processing on the 327 and
streaming output. 
Once this module is in place, extending some kind of complex interface over
this data stream should be pretty straightforward (Bluetooth, Wi-Fi, etc)

If we dont need to handle serial input asynchronously, then it would be 
possible to multiplex the uart hardware, but it might be a good idea to invest
the time now so we have that flexibility later on.

Ideally, we could extend the SPI interface used for the flash module and with
only one more pin we could add another UART interface that would be able to
support full, asynchronous duplex communication

SPI/I2C UART Extender: 
<a href="https://www.mouser.com/ProductDetail/Maxim-Integrated/MAX3100CPD%2b?qs=sGAEpiMZZMvslxq79%2fS5eSKNqE2Bo8gitsmTeCTA4pw%3d">
MAX3107</a>


### LCD
We are looking at different LCD layouts to find the correct option with regards
to out hardware and software constraints. The boards can be configured a number
of ways:<ul>
	<li>Half byte with control, 3 control pins, 4 data pins - 7 total
	<li>Full byte with control, 3 control pins, 8 data pins - 7 total
	<li>Serial?
</ul>>

### ECU Communication (Hardware and Software)
We have the broadstrokes of this one laid out. The ELM327 documentation comes
with more than enough information to lay out a complete circuit. This part just
needs the parts ordered, the connections routed, and the software written.

Luckily, we have an assembled 327 circuit that we can start to use for software
development in parallel to hardware development. Thus, this module can be 
broken up into 2 submodules, the hardware and the software.

This module follows a request, response architecture, so data is handled 
synchronously

UART - 2 pins

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
supports:<ul>
	<li>INT0_vect		: Generic external
	<li>INT1_vect		: Generic external
	<li>PCINT0_vect		: Pin change PORTB
	<li>PCINT1_vect		: Pin change PORTC
	<li>PCINT2_vect		: Pin change PORTD
	<li>SPI_STC_vect	: SPI transfer complete
	<li>USART_RX_vect	: UART RX complete
	<li>USART_UDRE_vect	: UART data register empty
	<li>USART_TX_vect	: UART TX complete
	<li>ADC_vect		: ADC conversion complete
	<li>TWI_vect		: I2C? Might be useful
	<li>and all the various timer interrupts
</ul>
I have no idea what this would entail, but it wouldn't hurt to do some research
on the topic to see if there are any open source libraries that we could 
possibly take inspiration from.

# Draft of our Block Diagram

<img src="resources/EE459_Diagram.png">

## Useful Commands

### Creating a snapshot of the repo
```
	git archive -o snapshot.zip --format zip HEAD
```
### Cloning repository
This creates a local repository identical to the remote one that you can test
with and make changes to.
You can grab the git URL from the "Clone or Download" tab on the repository 
front page
```
	git clone <http-url-git>
```
### Viewing repository changes, both staged and unstaged
```
	git status
```
### Adding changed files to the commit stage
Once you're comfortable with your changes you can mark then for addition to the
repository's database. To add all changed files you can substitute the path
argument with "." and execute from the project root.
```
	git add <path/to/file>
```
### Commiting changes
Once the changes you make are finalized and added to the commitment stage they
can be added to the permanent history of the repository.
```
	git commit -m "commit message, Hello!"
```
### Branching off on a working branch
Typically you don't just make changes directly to the same branch every one
else is working on. This can lead to data races and ends up being pretty
annoying to work out once code gets mixed up. So generally you branch off onto
your own section separate from everyone else, work there, and then merge in all
your changes to the master all in one motion.
```
	git checkout -b New-Branch-Name
```
### Synchronizing your local repository with the remote
This just pulls in other peoples changes into your own local repository.
```
	git pull
```

## Stats
| Date | Commit | Program Space | Data Space |
|---|---|---|---|
| February 3, 2019 | 1a580e57 | 3.2% | 7.1% |
| February 14, 2019 | 2ff6edde | 3.6% | 8.1% |