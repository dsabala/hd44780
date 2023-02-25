# Dependency free, UTF-8 ready, HD44780 driver

<img src="https://raw.githubusercontent.com/dsabala/hd44780/master/hd44780.gif">

[![Github Issues](https://img.shields.io/github/issues/dsabala/hd44780?style=plastic)](http://github.com/dsabala/hd44780/issues)
[![Github Releases](https://img.shields.io/github/v/release/dsabala/hd44780?style=plastic)](https://github.com/dsabala/hd44780/releases)
[![GitHub license](https://img.shields.io/github/license/dsabala/hd44780?style=plastic)](https://raw.githubusercontent.com/dsabala/hd44780/master/LICENSE)

# Overview

Yet another HD44780 C driver. It might fit your needs if you are interested in one of its core features:
- **dependency free** - its free from any Arduino or STM HAL headers, only standard library headers are used
- **fail-safe** - it will handle hardware fails gently and return error code
- **UTF-8 string support** - minimalistic support for UTF-8 strings (8 custom characters can be mapped) 
- **decoupled from underlying drivers** - by callback functions
- **multi-instantaneous** - more than one LCDs can be driven
- **support both communication modes** 4bit and 8bit interface with busy flag read
- **MIT license** - just fork this library and modify it to your needs
- **ready example** - for STM32F407G-DISC1 evalboard

Nothing comes without flaws, the disadvantages of this driver are:
- **arcane callbacks have to be implemented** - you might found it overcomplicated
- **binary size is not prority** - better choose other lib if your target platform lacks ROM memory
- **UTF-8 support might be too minimalistic** - its only 8 characters that can be mapped to CGRAM memory of HD44780
- **some features like shifting text are not implemented** - its still work in progrss

# Glimpse into features

Interface overview:
There are 8 poublic functions that covers 95% of HD44780 functionality
```c
hd44780_ret_e hd44780_init(const hd44780_ctx* const ctx);
hd44780_ret_e hd44780_clear(const hd44780_ctx* const ctx);
hd44780_ret_e hd44780_write_text(const hd44780_ctx* const ctx, const char* text);
hd44780_ret_e hd44780_set_pos(const hd44780_ctx* const ctx, uint8_t row, uint8_t column);
hd44780_ret_e hd44780_cursor_cfg(const hd44780_ctx* const ctx, hd44780_cursor cursor_cfg);
hd44780_ret_e hd44780_display_off(const hd44780_ctx* const ctx);
hd44780_ret_e hd44780_def_char(const hd44780_ctx* const ctx, uint8_t index, const uint8_t* const pattern);
hd44780_ret_e hd44780_disp_char(const hd44780_ctx* const ctx, uint8_t index);
```

Return codes:
```c
typedef enum {
  HD44780_OK = 0,                 /**< Success */
  HD44780_INV_ARG = 1,            /**< Invalid argument */
  HD44780_TIMEOUT = 2,            /**< Timeout */
  HD44780_CUSTOM_CHARS_INV = 3,   /**< Custom character array invalid */
  HD44780_CHAR_NOT_FOUND = 4,     /**< Custom character not found */
} hd44780_ret_e;
```

Usage examples:
```c
/* Get driver context from library - application glue code */
const hd44780_ctx* const lcd_ctx = hd44780_instance_ctx_get();

/* Initialise display */
hd44780_init(lcd_ctx);

/* Set positon to first line and write text */
hd44780_set_pos(lcd_ctx, 0, 0);
hd44780_write_text(lcd_ctx, "Bonjour collègues 🍌"); /* UTF-8 is supported */
```

Example of UTF-8 custom characters map:

```c
static const character_mapping mappings[3] = {
  {
      .utf_8_code = U'è',
      .character_bitmap = {0b01000, 0b00100, 0b01110, 0b10001, 0b11111, 0b10000, 0b01110, 0b00000},
  },
  {
      .utf_8_code = U'🍌',
      .character_bitmap = {0b01000, 0b00110, 0b00011, 0b00011, 0b00011, 0b00110, 0b01100, 0b10000},
  }
};
```

Full description of callbacks that user of library have to implement is available
in header file.

# Status
This library is not finished, there are surely bugs and things that can be simplified
or features that can be added. It was tested only on 4x20 display of one manufacturer. 
I decided to release it in this stage because 
I have no experience with UTF-8 and open source libraries and have no idea if
this library can be useful for someone.
