cmake_minimum_required(VERSION 3.16)

project(hd44780 C)

add_library(hd44780)
add_library(dsabala::hd44780 ALIAS hd44780)

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
