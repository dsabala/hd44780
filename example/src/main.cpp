/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala */

#include "bsp_general.hpp"
#include "hd44780.h"
#include "bsp_lcd.h"
#include "stm32f4xx_hal.h"

int main(void) {
  bsp::Init();

  /* Remember to wait 15ms after power up */
  HAL_Delay(15);

  hd44780_cfg_t * hd44780_cfg = hd44780_get_handle();

  hd44780_init(hd44780_cfg);

  static uint8_t const alarm_icon_index = 0U;
  static uint8_t const alarm_icon[8] = {0x04, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0x04, 0x00};
  hd44780_def_char(hd44780_cfg, alarm_icon_index, alarm_icon);

  hd44780_clear(hd44780_cfg);
  hd44780_goto(hd44780_cfg, 0, 0);
  hd44780_write_text(hd44780_cfg, "github.com/dsabala");

  hd44780_goto(hd44780_cfg, 1, 0);
  hd44780_write_text(hd44780_cfg, "dependency free,");

  hd44780_goto(hd44780_cfg, 2, 0);
  hd44780_write_text(hd44780_cfg, "multi-instantaneous,");

  hd44780_goto(hd44780_cfg, 3, 0);
  hd44780_write_text(hd44780_cfg, "HD44780 LCD driver");

  hd44780_disp_char(hd44780_cfg, alarm_icon_index);

  HAL_Delay(2000);

  hd44780_cfg->cursor_config = CURSOR_BLINK;
  hd44780_cursor_cfg(hd44780_cfg);

  while (1) {
    /* intentionally do nothing */
  }
}
