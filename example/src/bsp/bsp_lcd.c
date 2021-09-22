/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala */

#include "bsp_lcd.h"

#include "stm32f4xx_hal.h"

hd44780_cfg_t * hd44780_get_handle(void) {
  static hd44780_cfg_t config =
  {.cb_config_gpio = hd44780_cb_config_gpio,
   .cb_delay_ms = hd44780_cb_delay_ms,
   .cb_ctrl_pin = hd44780_cb_ctrl_pin,
   .cb_read_bus = hd44780_cb_read_bus,
   .cb_write_bus = hd44780_cb_write_bus,
   .cursor_config = CURSOR_OFF,
   .interface = INTERFACE_4BIT };

  return &config;
}

void hd44780_cb_config_gpio(hd44780_gpio_dir_t const direction) {
  /* Pins E, RW, RS are allways output */
  GPIO_InitTypeDef gpio;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  gpio.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOC, &gpio);
  gpio.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOC, &gpio);
  gpio.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOA, &gpio);

  /* Data bus pins direction are input/output depending on data direction */
  if (direction == GPIO_DIR_IN) {
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOC, &gpio);
    gpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &gpio);
    gpio.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOD, &gpio);
  } else {
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOC, &gpio);
    gpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &gpio);
    gpio.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOD, &gpio);
  }
}

void hd44780_cb_delay_ms(uint8_t const time_ms) { HAL_Delay(time_ms); }

void hd44780_cb_ctrl_pin(hd44780_ctrl_pin_t const pin, hd44780_pin_state_t const state) {
  GPIO_PinState const pin_state = (state == PIN_SET) ? GPIO_PIN_SET : GPIO_PIN_RESET;
  switch (pin) {
    case HD44780_PIN_RS:
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, pin_state);
      break;
    case HD44780_PIN_RW:
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, pin_state);
      break;
    case HD44780_PIN_E:
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, pin_state);
      break;
    default:
      break;
  }
}

uint8_t hd44780_cb_read_bus(void) {
  uint8_t data = 0;
  data |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 0U);
  data |= (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 1U);
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 2U);
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 3U);
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 4U);
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 5U);
  data |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 6U);
  data |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 7U);
  return data;
}

void hd44780_cb_write_bus(uint8_t const data) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (data & (1U << 0U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, (data & (1U << 1U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, (data & (1U << 2U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, (data & (1U << 3U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, (data & (1U << 4U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, (data & (1U << 5U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (data & (1U << 6U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (data & (1U << 7U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
