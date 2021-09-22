/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala */

#include <cstdint>

extern "C" void assert_failed(std::uint8_t *file, std::uint32_t line)
{
    (void)file;
    (void)line;
    while (1) {
        // do nothing
    }
}

void assert_failed_cpp(char const *file, std::uint32_t line)
{
    (void)file;
    (void)line;
    while (1) {
        // do nothing
    }
}
