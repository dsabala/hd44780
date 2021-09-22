/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala */

#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include <stdint.h>
#include "hd44780.h"

#ifdef __cplusplus
extern "C" {
#endif

hd44780_cfg_t * hd44780_get_handle(void);
void hd44780_cb_config_gpio(hd44780_gpio_dir_t const direction);
void hd44780_cb_delay_ms(uint8_t const time_ms);
void hd44780_cb_ctrl_pin(hd44780_ctrl_pin_t const pin,
                         hd44780_pin_state_t const state);
uint8_t hd44780_cb_read_bus(void);
void hd44780_cb_write_bus(uint8_t const data);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_LCD_H__ */
