cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(stm32f407-training C CXX ASM) 

set(CPU_OPTIONS
    -mthumb 
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

add_compile_options(
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Debug>:-gdwarf-2>
    $<$<COMPILE_LANGUAGE:C>:-std=c17>
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    -Wall
    -Wextra
    -fdata-sections
    -ffunction-sections
    ${CPU_OPTIONS}
)

add_executable(stm32f407-training.elf)

target_include_directories(stm32f407-training.elf
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/bsp
        ${CMAKE_SOURCE_DIR}/src
)

target_sources(stm32f407-training.elf
    PRIVATE
        src/assertions.cpp
        src/bsp/bsp_clocks.cpp
        src/bsp/bsp_general.cpp
        src/bsp/bsp_gpio.cpp
        src/bsp/bsp_lcd.c
        src/interrupts.c
        src/main.cpp
)

target_link_libraries(stm32f407-training.elf
    PUBLIC
        stm32f4-drivers
        hd44780
)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VGTx_FLASH.ld)

target_link_options(stm32f407-training.elf PRIVATE -T${LINKER_SCRIPT} -specs=nosys.specs -lnosys -lc -lm -Wl,-Map=${PROJECT_NAME}.map,--gc-sections,--cref,--print-memory-usage ${CPU_OPTIONS})

add_custom_command(
    TARGET stm32f407-training.elf
    POST_BUILD
        COMMAND echo ***** Memory sections *****
        COMMAND ${CMAKE_SIZE_UTIL} stm32f407-training.elf --radix=10
        COMMAND ${CMAKE_OBJCOPY} -Osrec stm32f407-training.elf stm32f407-training.srec
        COMMAND ${CMAKE_OBJCOPY} -Oihex stm32f407-training.elf stm32f407-training.hex
)

add_subdirectory(src)
