/* Dariusz Sabala, 2023, MIT License, https://github.com/dsabala/hd44780 */

#include "hd44780.h"

#include <stdint.h>
#include <string.h>

/* "Private" macrodefinitions */
#define REG_CLEAR            0x01

#define REG_EM               0x04
#define REG_EM_SHIFT_CURSOR  0x00
#define REG_EM_SHIFT_DISPLAY 0x01
#define REG_EM_DECREMENT     0x00
#define REG_EM_INCREMENT     0x02

#define REG_PWR_AND_CURSOR   0x08
#define REG_CURSOR_NOBLINK   0x00
#define REG_CURSOR_BLINK     0x01
#define REG_CURSOR_OFF       0x00
#define REG_CURSOR_ON        0x02
#define REG_DISPLAY_OFF      0x00
#define REG_DISPLAY_ON       0x04

#define REG_INTERFACE        0x20
#define REG_FONT_SIZE_5X8    0x00
#define REG_FONT_SIZE_5X10   0x04
#define REG_ONE_LINE         0x00
#define REG_TWO_LINES        0x08
#define REG_4_BIT_BUS        0x00
#define REG_8_BIT_BUS        0x10

#define REG_CGRAM_ADDR_SET   0x40

#define REG_DDRAM_ADDR_SET   0x80

/* Static, "private" functions declarations */

/**
 * @brief Prepare the bus for data read
 *
 * @param[in] ctx driver context
 */
static void s_config_bus_as_input(const hd44780_ctx* const ctx);

/**
 * @brief Prepare the bus for data write
 *
 * @param[in] ctx driver context
 */
static void s_config_bus_as_output(const hd44780_ctx* const ctx);

/**
 * @brief Perform read operation on bus
 *
 * @param[in] ctx driver context
 *
 * @return data byte, in case of 4-bit bus only higher nibble carries data
 */
static uint8_t s_read_operation(const hd44780_ctx* const ctx);

/**
 * @brief Read data byte independently from the bus width
 *
 * @param[in] ctx driver context
 *
 * @return data byte
 */
static uint8_t s_read_byte(const hd44780_ctx* const ctx);

/**
 * @brief Read memory address and busy flag
 *
 * @param[in] ctx driver context
 *
 * @return byte read (busy flag at most significant bit)
 */
static uint8_t s_read_address(const hd44780_ctx* const ctx);

/**
 * @brief Read memory data
 *
 * @param[in] ctx driver context
 *
 * @return byte read
 */
static uint8_t s_read_data(const hd44780_ctx* const ctx);

/**
 * @brief Writes byte of data on bus and sends enable pulse
 *
 * @param[in] ctx driver context
 * @param[in] data data to be send
 */
static void s_write_operation(const hd44780_ctx* const ctx, uint8_t data);

/**
 * @brief Writes data to the bus independently from bus width
 *
 * @param[in] ctx driver context
 * @param[in] data data to be send
 */
static void s_write_byte(const hd44780_ctx* const ctx, uint8_t data);

/**
 * @brief Writes instruction to lcd
 *
 * @param[in] ctx driver context
 * @param[in] instruction instruction
 *
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_TIMEOUT Timeout
 */
static hd44780_ret_e s_write_instruction(const hd44780_ctx* const ctx, uint8_t instruction);

/**
 * @brief Writes data to lcd
 *
 * @param[in] ctx driver context
 * @param[in] data data
 *
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_TIMEOUT Timeout
 */
static hd44780_ret_e s_write_data(const hd44780_ctx* const ctx, uint8_t data);

/**
 * @brief Upload custom characters to CGRAM memory
 * 
 * @param[in] ctx driver context
 * @return status
 * @retval HD44780_OK                 Success
 * @retval HD44780_TIMEOUT            Timeout
 * @retval HD44780_CUSTOM_CHARS_INV   Invalid custom characters array
 */
static hd44780_ret_e s_upload_custom_chars(const hd44780_ctx* const ctx);

/* Static functions implementation */

static void s_config_bus_as_input(const hd44780_ctx* const ctx) {
  ctx->cb_set_bus_direction(GPIO_DIR_IN);
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_RW, PIN_SET);
}

static void s_config_bus_as_output(const hd44780_ctx* const ctx) {
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_RW, PIN_RESET);
  ctx->cb_set_bus_direction(GPIO_DIR_OUT);
}

static hd44780_ret_e s_wait_till_busy(const hd44780_ctx* const ctx) {
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

static uint8_t s_read_operation(const hd44780_ctx* const ctx) {
  uint8_t data = 0UL;
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_E, PIN_SET);
  data = ctx->cb_read_bus();
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_E, PIN_RESET);
  return data;
}

static uint8_t s_read_byte(const hd44780_ctx* const ctx) {
  s_config_bus_as_input(ctx);
  uint8_t data = s_read_operation(ctx);
  if (ctx->interface == INTERFACE_4BIT) {
    uint8_t const lower_nibble = s_read_operation(ctx);
    data |= ((uint8_t)(lower_nibble >> 4U));
  }
  return data;
}

static uint8_t s_read_address(const hd44780_ctx* const ctx) {
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_RS, PIN_RESET);
  return s_read_byte(ctx);
}

static uint8_t s_read_data(const hd44780_ctx* const ctx) {
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_RS, PIN_SET);
  return s_read_byte(ctx);
}

static void s_write_operation(const hd44780_ctx* const ctx, uint8_t data) {
  s_config_bus_as_output(ctx);
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_E, PIN_SET);
  ctx->cb_write_bus(data);
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_E, PIN_RESET);
}

static void s_write_byte(const hd44780_ctx* const ctx, uint8_t data) {
  if (ctx->interface == INTERFACE_8BIT) {
    s_write_operation(ctx, data);
  } else {
    s_write_operation(ctx, data);
    s_write_operation(ctx, (uint8_t)(data << 4));
  }
}

static hd44780_ret_e s_write_instruction(const hd44780_ctx* const ctx, uint8_t instruction) {
  const hd44780_ret_e ret = ctx->cb_wait_for_busy_flag_clear(ctx);
  if (HD44780_OK == ret) {
    ctx->cb_set_ctrl_pin_state(HD44780_PIN_RS, PIN_RESET);
    s_write_byte(ctx, instruction);
  }
  return ret;
}

static hd44780_ret_e s_write_data(const hd44780_ctx* const ctx, uint8_t data) {
  const hd44780_ret_e ret = ctx->cb_wait_for_busy_flag_clear(ctx);
  if (HD44780_OK == ret) {
    ctx->cb_set_ctrl_pin_state(HD44780_PIN_RS, PIN_SET);
    s_write_byte(ctx, data);
  }
  return ret;
}

static hd44780_ret_e s_set_ddram_addr(const hd44780_ctx* const ctx, uint8_t address) {
  return s_write_instruction(ctx, REG_DDRAM_ADDR_SET | address);
}

static hd44780_ret_e s_set_cgram_addr(const hd44780_ctx* const ctx, uint8_t address) {
  return s_write_instruction(ctx, REG_CGRAM_ADDR_SET | address);
}

static hd44780_ret_e s_set_char_addr(const hd44780_ctx* const ctx, uint8_t index) {
  return s_set_cgram_addr(ctx, (index * 8));
}

static hd44780_ret_e s_upload_custom_chars(const hd44780_ctx* const ctx)
{
  hd44780_ret_e ret = HD44780_OK;
  if(ctx->custom_chars_map_len > 8) {
    ret = HD44780_CUSTOM_CHARS_INV;
    goto exit;
  }

  for(uint8_t i = 0; (i < ctx->custom_chars_map_len) && (ret == HD44780_OK); i++) {
    ret = hd44780_def_char(ctx, i, ctx->custom_chars_map[i].character_bitmap);
  }

exit:
  return ret;
}

static hd44780_ret_e s_find_character_by_code(const hd44780_ctx* const ctx, const uint32_t utf_8_code, uint8_t* const index) {
  hd44780_ret_e ret = HD44780_CHAR_NOT_FOUND;

  for (uint8_t i = 0; i < ctx->custom_chars_map_len; i++) {
    if (utf_8_code == ctx->custom_chars_map[i].utf_8_code) {
      *index = i;
      ret = HD44780_OK;
      break;
    }
  }

  return ret;
}

/* "Public" functions implementation */

hd44780_ret_e hd44780_clear(const hd44780_ctx* const ctx) { 
  return s_write_instruction(ctx, REG_CLEAR); 
}

hd44780_ret_e hd44780_write_text(const hd44780_ctx* const ctx, const char* text) {
  hd44780_ret_e ret = HD44780_OK;

  while ((HD44780_OK == ret) && (*text)) {
    uint32_t codepoint = 0U;
    if (*text <= 0x7f) {
        // Pure ASCII character
        ret = s_write_data(ctx, *text++);
        continue;
    } else if (*text <= 0xDF) {
        // Two byte UTF-8 character
        codepoint  = (*text++ & 0x1F) << 6;
        codepoint |= (*text++ & 0x3F);
    } else if (*text <= 0xEF) {
        // Three byte UTF-8 character
        codepoint  = (*text++ & 0x0F) << 12;
        codepoint |= (*text++ & 0x3F) << 6;
        codepoint |= (*text++ & 0x3F);
    } else {
        // Four byte UTF-8 character
        codepoint  = (*text++ & 0x07) << 18;
        codepoint |= (*text++ & 0x3F) << 12;
        codepoint |= (*text++ & 0x3F) << 6;
        codepoint |= (*text++ & 0x3F);
    }

    uint8_t custom_char_index = 0;
    ret = s_find_character_by_code(ctx, codepoint, &custom_char_index);
    if (HD44780_OK == ret) {
      uint32_t address = s_read_address(ctx);
      hd44780_disp_char(ctx, custom_char_index);
      s_set_ddram_addr(ctx, (++address));
    }
  }

exit:
  return ret;
}

hd44780_ret_e hd44780_set_pos(const hd44780_ctx* const ctx, uint8_t row, uint8_t column) {
  hd44780_ret_e ret = HD44780_OK;

  if ((ctx->column_width <= column) || (ctx->number_of_lines <= row)) {
    ret = HD44780_INV_ARG;
    goto exit;
  }

  uint8_t address = column;
  switch (row) {
    case 1:
      address += 0x40U;
      break;
    case 2:
      address += ctx->column_width;
      break;
    case 3:
      address += 0x40U;
      address += ctx->column_width;
      break;
    case 0: /* intentionally fallthrough */
    default:
      break;
  }

  ret = s_set_ddram_addr(ctx, address);

exit:
  return ret;
}

hd44780_ret_e hd44780_cursor_cfg(const hd44780_ctx* const ctx, hd44780_cursor cursor_cfg) {
  hd44780_ret_e ret = HD44780_OK;

  uint8_t instruction = REG_PWR_AND_CURSOR | REG_DISPLAY_ON;
  if (cursor_cfg == CURSOR_OFF) {
    instruction |= REG_CURSOR_OFF;
  } else if (cursor_cfg == CURSOR_ON) {
    instruction |= REG_CURSOR_ON;
  } else if (cursor_cfg == CURSOR_BLINK) {
    instruction |= REG_CURSOR_ON | REG_CURSOR_BLINK;
  } else {
    ret = HD44780_INV_ARG;
    goto exit;
  }
  ret = s_write_instruction(ctx, instruction);

exit:
  return ret;
}

hd44780_ret_e hd44780_init(const hd44780_ctx* const ctx) {
  hd44780_ret_e ret = HD44780_OK;
  ctx->cb_init_common();
  ctx->cb_set_ctrl_pin_state(HD44780_PIN_RS, PIN_RESET);

  s_write_operation(ctx, REG_INTERFACE | REG_8_BIT_BUS);
  ctx->cb_delay_ms(DELAY_INIT_SEQ_LONG_MS);
  for (uint8_t i = 0U; i < 2U; i++) {
    s_write_operation(ctx, REG_INTERFACE | REG_8_BIT_BUS);
    ctx->cb_delay_ms(DELAY_INIT_SEQ_SHORT_MS);
  }

  if (ctx->interface == INTERFACE_8BIT) {
    s_write_operation(ctx, REG_INTERFACE | REG_FONT_SIZE_5X8 | REG_TWO_LINES | REG_8_BIT_BUS);
  } else {
    s_write_operation(ctx, REG_INTERFACE | REG_4_BIT_BUS);
    ctx->cb_delay_ms(DELAY_INIT_SEQ_SHORT_MS);
    ret = s_write_instruction(ctx, REG_INTERFACE | REG_FONT_SIZE_5X8 | REG_TWO_LINES | REG_4_BIT_BUS);
    if (HD44780_OK != ret) {
      goto exit;
    }
  }

  ret = hd44780_display_off(ctx);
  if (HD44780_OK != ret) {
    goto exit;
  }
  ret = hd44780_clear(ctx);
  if (HD44780_OK != ret) {
    goto exit;
  }
  ret = s_write_instruction(ctx, REG_EM | REG_EM_SHIFT_CURSOR | REG_EM_INCREMENT);
  if (HD44780_OK != ret) {
    goto exit;
  }
  ret = hd44780_cursor_cfg(ctx, CURSOR_OFF);
  if (HD44780_OK != ret) {
    goto exit;
  }
  ret = s_upload_custom_chars(ctx);

exit:
  return ret;
}

hd44780_ret_e hd44780_display_off(const hd44780_ctx* const ctx) {
  return s_write_instruction(ctx, REG_PWR_AND_CURSOR | REG_DISPLAY_OFF);
}

hd44780_ret_e hd44780_def_char(const hd44780_ctx* const ctx, uint8_t index, const uint8_t* const pattern) {
  hd44780_ret_e ret = HD44780_OK;
  ret = s_set_char_addr(ctx, index);
  for (uint8_t i = 0; ((HD44780_OK == ret) && (i < 8)); i++) {
    ret = s_write_data(ctx, pattern[i]);
  }
  return ret;
}

hd44780_ret_e hd44780_disp_char(const hd44780_ctx* const ctx, uint8_t const index) {
  return s_write_data(ctx, index);
}

bool hd44780_is_busy(const hd44780_ctx* const ctx)
{
  bool ret = false;

  if (s_read_address(ctx) & 0x80) {
    ret = true;
  }

  return ret;
}
