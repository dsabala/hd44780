/* Dariusz Sabala, 2023, MIT License, https://github.com/dsabala/hd44780 */

#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include "hd44780.h"

#ifdef __cplusplus
extern "C" {
#endif

hd44780_ctx* hd44780_instance_ctx_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_LCD_H__ */
