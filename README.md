# Dependency free HD44780 driver

<img src="https://raw.githubusercontent.com/dsabala/hd44780/master/hd44780.gif" width="300" height="169">

[![Github Issues](https://img.shields.io/github/issues-raw/dsabala/hd44780)](http://github.com/dsabala/hd44780/issues)
[![Github Releases](https://img.shields.io/github/v/release/dsabala/hd44780)](https://github.com/dsabala/hd44780/releases)
[![GitHub license](https://img.shields.io/github/license/dsabala/hd44780)](https://raw.githubusercontent.com/dsabala/hd44780/master/LICENSE)

Yet another HD44780 C driver. It might fit your needs if you are interested in one of its core features:
- **dependency free** - its free from any Arduino or STM HAL headers, neither standard library is used
- **decoupled from underlying drivers** - using pointers to callback functions
- **multi-instantaneous** - so more than one LCDs can be driven
- **support all communication modes** 4bit and 8bit interface with busy flag read
- **CMake support**
- **MIT license**

## Example
Usage example is available at https://github.com/dsabala/hd44780-example

Short:
- implement all the callback functions being members of handler structure
- pass handler to public API

## Todo
- bound blocking API block time and support assertions or returning fail status
- support various LCD size (now it is fixed to 4x20)
- support shifting text
- single source file examples for popular platforms like Arduino or STM32
