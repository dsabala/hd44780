add_library(hd44780 STATIC)

target_sources(hd44780
    PRIVATE
        hd44780.c
    PUBLIC
        hd44780.h
)

target_include_directories(hd44780
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
