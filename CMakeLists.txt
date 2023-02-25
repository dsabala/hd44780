cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hd44780 C)

option(BUILD_EXAMPLE "Build library example" ON)

################################################################################
# HD44780 library
################################################################################

add_library(hd44780)
add_library(dsabala::hd44780 ALIAS hd44780)

target_sources(hd44780
    PRIVATE
        src/hd44780.c
    PUBLIC
        src/hd44780.h
)

target_include_directories(hd44780
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

################################################################################
# HD44780 library example
################################################################################

if(BUILD_EXAMPLE)
    enable_language(ASM)

    add_executable(hd44780-example)

    set(CPU_OPTIONS
        -mthumb 
        -mcpu=cortex-m4
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
    )

    target_compile_options(hd44780
        PRIVATE
            ${CPU_OPTIONS}
    )

    target_link_libraries(hd44780-example
        PRIVATE
            hd44780
    )

    target_compile_options(hd44780-example
        PRIVATE
            -Og
            -Wall
            -Wextra
            -fdata-sections
            -ffunction-sections
            -Wno-unused-parameter
            ${CPU_OPTIONS}
    )

    target_compile_definitions(hd44780-example
        PRIVATE
            STM32F407xx
            USE_HAL_DRIVER
            HSE_VALUE=8000000U
    )

    target_include_directories(hd44780-example
        PRIVATE 
            ${CMAKE_CURRENT_SOURCE_DIR}/example/src
            ${CMAKE_CURRENT_SOURCE_DIR}/example/stm32f4-drivers/include
    )

    target_sources(hd44780-example
        PRIVATE
            example/src/bsp_lcd.c
            example/src/main.c
            example/stm32f4-drivers/source/startup_stm32f407xx.s
            example/stm32f4-drivers/source/stm32f4xx_hal.c
            example/stm32f4-drivers/source/stm32f4xx_hal_cortex.c
            example/stm32f4-drivers/source/stm32f4xx_hal_gpio.c
            example/stm32f4-drivers/source/system_stm32f4xx.c
    )

    target_link_options(hd44780-example
        PRIVATE 
            -T${CMAKE_CURRENT_SOURCE_DIR}/example/stm32f4-drivers/STM32F407VGTx_FLASH.ld
            -specs=nosys.specs 
            -lnosys
            -lc
            -Wl,-Map=${PROJECT_NAME}.map,--gc-sections,--cref,--print-memory-usage 
            ${CPU_OPTIONS}
    )
    
endif()
