find_program(AVR_CC avr-gcc REQUIRED)
find_program(AVR_CXX avr-g++ REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_SIZE_TOOL avr-size REQUIRED)
find_program(AVR_OBJDUMP avr-objdump REQUIRED)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})
set(CMAKE_ASM_COMPILER ${AVR_CC})

set(elf_file ${PROJECT_NAME}.elf)
set(hex_file ${PROJECT_NAME}.hex)
set(lst_file ${PROJECT_NAME}.lst)
set(map_file ${PROJECT_NAME}.map)
set(eeprom_image ${PROJECT_NAME}-eeprom.hex)

add_compile_options(
   -DF_CPU=${AVR_MCU_SPEED}
   -mmcu=${AVR_MCU_TYPE}
   -fpack-struct
   -fshort-enums
   -Wall
   -Werror
   -pedantic
   -pedantic-errors
   -funsigned-char
   -funsigned-bitfields
   -ffunction-sections
   -c
   -std=gnu99
   -Os
)

add_link_options(
   -mmcu=${AVR_MCU_TYPE} 
   -Wl,--gc-sections 
   -mrelax 
   -Wl,-Map,${map_file}
)

add_custom_command(
   OUTPUT ${hex_file}
   COMMAND
      ${AVR_OBJCOPY} -j .text -j .data -O ihex ${elf_file} ${hex_file}
   COMMAND
      ${AVR_SIZE_TOOL} ${AVR_SIZE_ARGS} ${elf_file}
   DEPENDS ${elf_file}
)

add_custom_command(
   OUTPUT ${lst_file}
   COMMAND
      ${AVR_OBJDUMP} -d ${elf_file} > ${lst_file}
   DEPENDS ${elf_file}
)

add_custom_target(
   ${PROJECT_NAME}
   ALL
   DEPENDS ${hex_file} ${lst_file} ${eeprom_image}
)

set_target_properties(
   ${PROJECT_NAME}
   PROPERTIES
      OUTPUT_NAME "${elf_file}"
)

get_directory_property(clean_files ADDITIONAL_MAKE_CLEAN_FILES)
set_directory_properties(
   PROPERTIES
      ADDITIONAL_MAKE_CLEAN_FILES "${map_file}"
)

set(AVR_UPLOADTOOL_BASE_OPTIONS -p ${AVR_MCU_TYPE} -c ${AVR_PROGRAMMER})

if(AVR_UPLOADTOOL_BAUDRATE)
    set(AVR_UPLOADTOOL_BASE_OPTIONS ${AVR_UPLOADTOOL_BASE_OPTIONS} -b ${AVR_UPLOADTOOL_BAUDRATE})
endif()

add_custom_target(
    upload
    ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} ${AVR_UPLOADTOOL_OPTIONS}
       -U flash:w:${hex_file}
       -P ${AVR_UPLOADTOOL_PORT}
       -B ${AVR_UPLOADTOOL_BIT_RATE}
    DEPENDS ${hex_file}
    COMMENT "Uploading ${hex_file} to ${AVR_MCU_TYPE} using ${AVR_PROGRAMMER}"
 )

 add_custom_command(
    OUTPUT ${eeprom_image}
    COMMAND
       ${AVR_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load
          --change-section-lma .eeprom=0 --no-change-warnings
          -O ihex ${elf_file} ${eeprom_image}
    DEPENDS ${elf_file}
 )

 add_custom_target(
    upload_eeprom
    ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} ${AVR_UPLOADTOOL_OPTIONS}
       -U eeprom:w:${eeprom_image}
       -P ${AVR_UPLOADTOOL_PORT}
       -B ${AVR_UPLOADTOOL_BIT_RATE}
    DEPENDS ${eeprom_image}
    COMMENT "Uploading ${eeprom_image} to ${AVR_MCU_TYPE} using ${AVR_PROGRAMMER}"
 )

add_custom_target(
    get_status
    ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE} -n -v
    COMMENT "Get status from ${AVR_MCU_TYPE}"
)

add_custom_target(
    get_fuses
    ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE} -n
        -U lfuse:r:-:b
        -U hfuse:r:-:b
    COMMENT "Get fuses from ${AVR_MCU_TYPE}"
)

add_custom_target(
    set_fuses
    ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE}
        -U lfuse:w:${AVR_L_FUSE}:m
        -U hfuse:w:${AVR_H_FUSE}:m
        COMMENT "Setup: High Fuse: ${AVR_H_FUSE} Low Fuse: ${AVR_L_FUSE}"
)

add_custom_target(
    get_calibration
        ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE}
        -U calibration:r:${AVR_MCU_TYPE}_calib.tmp:r
        COMMENT "Write calibration status of internal oscillator to ${AVR_MCU_TYPE}_calib.tmp."
)

add_custom_target(
    set_calibration
    ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE}
        -U calibration:w:${AVR_MCU_TYPE}_calib.hex
        COMMENT "Program calibration status of internal oscillator from ${AVR_MCU_TYPE}_calib.hex."
)