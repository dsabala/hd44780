/* Dariusz Sabala, 2023, MIT License, https://github.com/dsabala/hd44780 */

#include "hd44780.h"
#include "bsp_lcd.h"
#include "stm32f4xx_hal.h"
#include "assert.h"
#include "string.h"

int main(void) {
  HAL_Init();

  /* Remember to wait 15ms after power up */
  HAL_Delay(15);

  /* Get display instance context */
  const hd44780_ctx* const lcd_ctx = hd44780_instance_ctx_get();

  /* Initialisation of display */
  hd44780_ret_e retval = hd44780_init(lcd_ctx);
  assert(HD44780_OK == retval);

  /* HD44780 can be safely initialised even if it was initialised prior */
  retval = hd44780_init(lcd_ctx);
  assert(HD44780_OK == retval);

  /* Clear whole LCD */
  retval = hd44780_clear(lcd_ctx);
  assert(HD44780_OK == retval);

  /* Set positon to first line and write text */
  retval = hd44780_set_pos(lcd_ctx, 0, 0);
  assert(HD44780_OK == retval);
  retval = hd44780_write_text(lcd_ctx, "Bonjour collègues 🍌"); /* UTF-8 is supported */
  assert(HD44780_OK == retval);

  /* Write some more text */
  retval = hd44780_set_pos(lcd_ctx, 1, 0);
  assert(HD44780_OK == retval);
  retval = hd44780_write_text(lcd_ctx, "dependency free,");
  assert(HD44780_OK == retval);
  retval = hd44780_set_pos(lcd_ctx, 2, 0);
  assert(HD44780_OK == retval);
  retval = hd44780_write_text(lcd_ctx, "utf8 ready, failsafe");
  assert(HD44780_OK == retval);
  retval = hd44780_set_pos(lcd_ctx, 3, 0);
  assert(HD44780_OK == retval);
  retval = hd44780_write_text(lcd_ctx, "HD44780 driver ↑");
  assert(HD44780_OK == retval);

  /* Change cursor to blinking one */
  retval = hd44780_set_pos(lcd_ctx, 3, 17);
  assert(HD44780_OK == retval);
  retval = hd44780_cursor_cfg(lcd_ctx, CURSOR_BLINK);
  assert(HD44780_OK == retval);

  while (1) {
    /* intentionally do nothing */
  }
}

/* Own systick implementation due to HAL delivered delay function usage */
void SysTick_Handler(void);
void SysTick_Handler(void)
{
  HAL_IncTick();
}
