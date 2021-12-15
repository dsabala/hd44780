# Dependency free HD44780 driver

![LCD-gif-animation](hd44780.gif)

[![Github Issues](https://img.shields.io/github/issues-raw/dsabala/hd44780)](http://github.com/dsabala/hd44780/issues)
[![Github Releases](https://img.shields.io/github/v/release/dsabala/hd44780)](https://github.com/dsabala/hd44780/releases)
[![GitHub license](https://img.shields.io/github/license/dsabala/hd44780)](https://raw.githubusercontent.com/dsabala/hd44780/master/LICENSE)

Yet another HD44780 C driver. It might fit your needs if you are interested in one of its core features:
- **dependency free** - its free from any Arduino or STM HAL headers, neither standard library is used
- **decoupled from underlying drivers** - using pointers to callback functions
- **multi-instantaneous** - so more than one LCDs can be driven
- **delays reduced to the minimum** - both 4bit and 8bit interface with busy flag polling is supported
- **one source file and one header file**
- **CMake support**
- **MIT license**
