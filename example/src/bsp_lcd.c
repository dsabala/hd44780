/* Dariusz Sabala, 2023, MIT License, https://github.com/dsabala/hd44780 */

#include "bsp_lcd.h"
#include "stm32f4xx_hal.h"

/* Switch between interface types - 4bit and 8bit width bus */
#define INTERFACE_WIDTH  (4)
//#define INTERFACE_WIDTH  (8)

static void hd44780_cb_init_cotrol_pins(void);
static void hd44780_cb_config_gpio(hd44780_gpio_dir const direction);
static void hd44780_cb_delay_ms(uint8_t const time_ms);
static void hd44780_cb_ctrl_pin(hd44780_ctrl_pin const pin, hd44780_pin_state const state);
static uint8_t hd44780_cb_read_bus(void);
static void hd44780_cb_write_bus(uint8_t const data);
static hd44780_ret_e hd44780_wait_for_busy_flag_clear(const struct hd44780_ctx_s* const ctx);

static const character_mapping mappings[3] = {
  {
      .utf_8_code = U'è',
      .character_bitmap = {0b01000, 0b00100, 0b01110, 0b10001, 0b11111, 0b10000, 0b01110, 0b00000},
  },
  {
      .utf_8_code = U'↑',
      .character_bitmap = {0b00000, 0b00100, 0b01110, 0b10101, 0b00100, 0b00100, 0b00100, 0b00000},
  },
  {
      .utf_8_code = U'🍌',
      .character_bitmap = {0b01000, 0b00110, 0b00011, 0b00011, 0b00011, 0b00110, 0b01100, 0b10000},
  }
};

static void hd44780_cb_init_cotrol_pins(void)
{
  /* Initialise GPIO peripheral clocks */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Pins E, RW, RS are allways output */
  GPIO_InitTypeDef gpio_config = {
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW,
  };
  gpio_config.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOC, &gpio_config);
  gpio_config.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOC, &gpio_config);
  gpio_config.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOA, &gpio_config);
}

static void hd44780_cb_config_gpio(hd44780_gpio_dir const direction) {
  GPIO_InitTypeDef gpio = {
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW,
  };

  /* Data bus pins direction are input/output depending on data direction */
  if (direction == GPIO_DIR_IN) {
    gpio.Mode = GPIO_MODE_INPUT;
#if (INTERFACE_WIDTH == 8)
    gpio.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOC, &gpio);
    gpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &gpio);
    gpio.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &gpio);
#endif
    gpio.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &gpio);
  } else {
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
#if (INTERFACE_WIDTH == 8)
    gpio.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOC, &gpio);
    gpio.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &gpio);
    gpio.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &gpio);
#endif
    gpio.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &gpio);
  }
}

static void hd44780_cb_delay_ms(uint8_t const time_ms) {
  HAL_Delay(time_ms); 
}

static void hd44780_cb_ctrl_pin(hd44780_ctrl_pin const pin, hd44780_pin_state const state) {
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

static uint8_t hd44780_cb_read_bus(void) {
  uint8_t data = 0;
#if (INTERFACE_WIDTH == 8)
  data |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 0U);
  data |= (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 1U);
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 2U);
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 3U);
#endif
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 4U);
  data |= (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 5U);
  data |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 6U);
  data |= (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) ? 0x00U : (0x01U << 7U);
  return data;
}

static void hd44780_cb_write_bus(uint8_t const data) {
#if (INTERFACE_WIDTH == 8)
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (data & (1U << 0U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, (data & (1U << 1U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, (data & (1U << 2U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, (data & (1U << 3U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, (data & (1U << 4U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, (data & (1U << 5U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (data & (1U << 6U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (data & (1U << 7U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static hd44780_ret_e hd44780_wait_for_busy_flag_clear(const struct hd44780_ctx_s* const ctx)
{
  /* 
   * This is very simple implementation of wait function
   * 
   * If RTOS is available you can configure GPIO peripheral to trigger an interrupt
   * when D7 busy flag pin changes state to low and wait here on semaphore locked, 
   * then when pin changes state and interrupt triggers, in interrupt handler semaphore
   * is released and thread unblocked
   */
  hd44780_ret_e ret = HD44780_TIMEOUT;
  int timeout_ms = HD44780_TIMEOUT_MS;
  while ((0 <= timeout_ms)) {
    if (!(hd44780_is_busy(ctx))) {
      ret = HD44780_OK;
      break;
    }
    ctx->cb_delay_ms(HD44780_TIMEOUT_TICK_MS);
    timeout_ms -= HD44780_TIMEOUT_TICK_MS;
  };
  return ret;
}

hd44780_ctx * hd44780_instance_ctx_get(void) {
  static hd44780_ctx config =
  {
    .cb_init_common = hd44780_cb_init_cotrol_pins,
    .cb_set_bus_direction = hd44780_cb_config_gpio,
    .cb_set_ctrl_pin_state = hd44780_cb_ctrl_pin,
    .cb_read_bus = hd44780_cb_read_bus,
    .cb_write_bus = hd44780_cb_write_bus,
    .cb_delay_ms = hd44780_cb_delay_ms,
    .cb_wait_for_busy_flag_clear = hd44780_wait_for_busy_flag_clear,
    .custom_chars_map = mappings,
    .custom_chars_map_len = 3,
    .number_of_lines = 4,
    .column_width = 20,
#if INTERFACE_WIDTH == 4
    .interface = INTERFACE_4BIT 
#endif
#if INTERFACE_WIDTH == 8
    .interface = INTERFACE_8BIT 
#endif
  };

  return &config;
}
