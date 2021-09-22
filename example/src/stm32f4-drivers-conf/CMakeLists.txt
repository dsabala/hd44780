add_library(stm32f4-drivers-conf INTERFACE)

target_include_directories(stm32f4-drivers-conf 
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/stm32f4-drivers-conf/>
)

target_compile_definitions(stm32f4-drivers-conf
    INTERFACE
        STM32F407xx
        USE_HAL_DRIVER
        HSE_VALUE=8000000U
        HSE_STARTUP_TIMEOUT=100U
)
