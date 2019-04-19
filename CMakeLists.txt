###############################################################################
##
#   USC EE459 Group 17 CMakeLists.txt
##
###############################################################################

cmake_minimum_required(VERSION 2.8)
# Project name
project(USC_EE459_GROUP_17)

# Product binary
set(PRODUCT_NAME USC_EE459_GROUP_17)

## AVR Chip Configuration
set(F_CPU 7372800)
set(MCU atmega1284p)
# Default Baudrate for UART0
set(BAUD0 34800)

## The programmer to use, read avrdude manual for list
set(PROG_TYPE usbtiny)

# AVR Fuses, must be in concordance with your hardware and F_CPU
# http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p
set(E_FUSE 0xfd)
set(H_FUSE 0xda)
set(L_FUSE 0xfd)
set(LOCK_BIT 0xff)


# Use AVR GCC toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)

# Remove '-rdynamic'
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

# Pass defines to compiler
add_definitions(
    -DF_CPU=${F_CPU}
    -DBAUD0=${BAUD0}
)
# mmcu MUST be passed to bot the compiler and linker, this handle the linker
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU} -Wl,-u,vfprintf")

add_compile_options(
    -mmcu=${MCU} # MCU
    -std=gnu99 # C99 standard
    -Os # optimize
    -Wall # enable warnings
    # -Wno-main
    # -Wundef
    # -pedantic
    # -Wl,--relax,--gc-sections
    # -g
    # -gdwarf-2
    # -funsigned-char # a few optimizations
    # -funsigned-bitfields
    # -fpack-struct
    # -fshort-enums
    # -ffunction-sections
    # -fdata-sections
    # -fno-split-wide-types
    # -fno-tree-scev-cprop
    -lm
    -Wl,-Map=myprog.map 
    -Wl,--cref 
)

file(GLOB SRC_FILES "src/*.c") # Load all files in src folder

# Create one target
add_executable(${PRODUCT_NAME} ${SRC_FILES})

# Include the header files
target_include_directories(${PRODUCT_NAME} PUBLIC src/include)

target_link_libraries(${PRODUCT_NAME} printf_flt)

# Rename the output to .elf as we will create multiple files
set_target_properties(${PRODUCT_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)

# Strip binary for upload
add_custom_target(strip ALL avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex DEPENDS strip)
# Transform binary into hex file, this is the eeprom part (empty if you don't
# use eeprom static variables)
add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)

# Upload the firmware with avrdude
add_custom_target(flash avrdude  -c ${PROG_TYPE} -p ${MCU} -U flash:w:${PRODUCT_NAME}.hex DEPENDS hex)

# Upload the eeprom with avrdude
add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)

# Burn fuses
add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )

# Remove cmake files
add_custom_target(distclean rm -rf CMakeFiles cmake_install.cmake CMakeCache.txt *.elf *.hex Makefile)

# Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")
