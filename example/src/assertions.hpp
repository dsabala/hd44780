/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala */

#pragma once

#include <cstdint>

void assert_failed_cpp(char const *file, std::uint32_t line);

#define a_assert(expr) ((expr) ? (void)0U : assert_failed_cpp((__FILE__), __LINE__))