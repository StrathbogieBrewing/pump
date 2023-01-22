cmake_minimum_required(VERSION 3.18)

set(AVR_MCU_TYPE atmega8)
set(AVR_H_FUSE 0xDA)
set(AVR_L_FUSE 0xFF)
# set(AVR_EX_FUSE 0×05)
set(AVR_MCU_SPEED "1000000UL")

### printf support
#set(EXTRA_FLAGS "-Wl,-u,vfprintf -lprintf_min") 

set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER usbasp)
#set(AVR_PROGRAMMER arduino)
set(AVR_UPLOADTOOL_PORT usb)
#set(AVR_UPLOADTOOL_PORT /dev/ttyACM0)
#set(AVR_UPLOADTOOL_BITRATE 57600)
set(AVR_UPLOADTOOL_BIT_RATE 4)

project(pump C ASM)

include(avr-gcc.cmake)

# include(FetchContent)
# FetchContent_Declare(
#    cobsm
#    GIT_REPOSITORY https://github.com/StrathbogieBrewing/cobsm.git
#    GIT_TAG        main
# )
# FetchContent_MakeAvailable(cobsm)
# include_directories(${cobsm_SOURCE_DIR}/include)

# include(FetchContent)
# FetchContent_Declare(
#    acetrace
#    GIT_REPOSITORY https://github.com/StrathbogieBrewing/AceTrace.git
#    GIT_TAG        main
# )
# FetchContent_MakeAvailable(acetrace)
   
# include_directories(temp)
add_executable(${elf_file} src/main.c )

# target_link_libraries(${elf_file} acetrace)







