/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala/hd44780 */

#include "hd44780.h"

/* Register addresses and bitfields bellow */
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

/* Blocking delay times */
#define DELAY_INIT_SEQ_LONG_MS  5
#define DELAY_INIT_SEQ_SHORT_MS 1

/**
 * @brief Prepares the bus for reading data from LCD
 * @param cfg [in] pointer to configuration
 */
static void hd44780_set_bus_in(hd44780_hdl const* const cfg) {
  cfg->cb_config_gpio(GPIO_DIR_IN);
  cfg->cb_ctrl_pin(HD44780_PIN_RW, PIN_SET);
}

/**
 * @brief Prepares the bus for writing data to LCD
 * @param cfg [in] pointer to configuration
 */
static void hd44780_set_bus_out(hd44780_hdl const* const cfg) {
  cfg->cb_ctrl_pin(HD44780_PIN_RW, PIN_RESET);
  cfg->cb_config_gpio(GPIO_DIR_OUT);
}

/**
 * @brief Waits until busy flag is set
 * @param cfg [in] pointer to configuration
 */
static void hd44780_wait_till_busy(hd44780_hdl const* const cfg) {
  while (hd44780_read_address(cfg) & 0x80) {
    // blocking wait until flag is set
  };
}

/**
 * @brief Read byte from lcd
 * @param cfg [in] pointer to configuration
 * @return byte read
 */
static unsigned char hd44780_read_byte(hd44780_hdl const* const cfg) {
  unsigned char data = 0;

  hd44780_set_bus_in(cfg);
  cfg->cb_ctrl_pin(HD44780_PIN_E, PIN_SET);
  data = cfg->cb_read_bus();
  cfg->cb_ctrl_pin(HD44780_PIN_E, PIN_RESET);
  if (cfg->interface == INTERFACE_4BIT) {
    cfg->cb_ctrl_pin(HD44780_PIN_E, PIN_SET);
    unsigned char const lower_nibble = (cfg->cb_read_bus());
    cfg->cb_ctrl_pin(HD44780_PIN_E, PIN_RESET);
    data |= ((unsigned char)(lower_nibble >> 4U));
  }

  return data;
}

unsigned char hd44780_read_address(hd44780_hdl const* const cfg) {
  cfg->cb_ctrl_pin(HD44780_PIN_RS, PIN_RESET);
  return hd44780_read_byte(cfg);
}

unsigned char hd44780_read_data(hd44780_hdl const* const cfg) {
  cfg->cb_ctrl_pin(HD44780_PIN_RS, PIN_SET);
  return hd44780_read_byte(cfg);
}

/**
 * @brief Writes byte of data on bus and sends enable pulse
 * @param cfg [in] pointer to configuration
 * @param byte [in] byte to be send
 */
static void hd44780_send_byte_raw(hd44780_hdl const* const cfg, unsigned char byte) {
  hd44780_set_bus_out(cfg);
  cfg->cb_ctrl_pin(HD44780_PIN_E, PIN_SET);
  cfg->cb_write_bus(byte);
  cfg->cb_ctrl_pin(HD44780_PIN_E, PIN_RESET);
}

/**
 * @brief Writes instruction to lcd
 * @param cfg [in] pointer to configuration
 * @param instruction [in] instruction
 */
static void hd44780_send_instruction(hd44780_hdl const* const cfg, unsigned char instruction) {
  hd44780_wait_till_busy(cfg);
  if (cfg->interface == INTERFACE_8BIT) {
    cfg->cb_ctrl_pin(HD44780_PIN_RS, PIN_RESET);
    hd44780_send_byte_raw(cfg, instruction);
  } else {
    cfg->cb_ctrl_pin(HD44780_PIN_RS, PIN_RESET);
    hd44780_send_byte_raw(cfg, instruction);
    hd44780_send_byte_raw(cfg, (unsigned char)(instruction << 4));
  }
}

/**
 * @brief Writes data to lcd
 * @param cfg [in] pointer to configuration
 * @param instruction [in] data
 */
static void hd44780_send_data(hd44780_hdl const* const cfg, unsigned char data) {
  hd44780_wait_till_busy(cfg);
  if (cfg->interface == INTERFACE_8BIT) {
    cfg->cb_ctrl_pin(HD44780_PIN_RS, PIN_SET);
    hd44780_send_byte_raw(cfg, data);
  } else {
    cfg->cb_ctrl_pin(HD44780_PIN_RS, PIN_SET);
    hd44780_send_byte_raw(cfg, data);
    hd44780_send_byte_raw(cfg, (unsigned char)(data << 4));
  }
}

void hd44780_write_text(hd44780_hdl const* const cfg, char const* text) {
  while (*text) {
    hd44780_send_data(cfg, *text++);
  }
}

void hd44780_set_ddram_addr(hd44780_hdl const* const cfg, unsigned char address) {
  hd44780_send_instruction(cfg, REG_DDRAM_ADDR_SET | address);
}

void hd44780_set_pos(hd44780_hdl const* const cfg, unsigned char row, unsigned char column) {
  unsigned char address = column;
  switch (row) {
    case 1:
      address += 0x40U;
      break;
    case 2:
      address += 20U;
      break;
    case 3:
      address += 0x40U;
      address += 20U;
      break;
    case 0: /* intentionally fallthrough */
    default:
      break;
  }
  hd44780_set_ddram_addr(cfg, address);
}

void hd44780_clear(hd44780_hdl const* const cfg) { hd44780_send_instruction(cfg, REG_CLEAR); }

void hd44780_cursor_cfg(hd44780_hdl const* const cfg, hd44780_cursor_t const cursor_cfg) {
  unsigned char instruction = REG_PWR_AND_CURSOR | REG_DISPLAY_ON;
  if (cursor_cfg == CURSOR_OFF) {
    instruction |= REG_CURSOR_OFF;
  } else if (cursor_cfg == CURSOR_ON) {
    instruction |= REG_CURSOR_ON;
  } else {
    instruction |= REG_CURSOR_ON | REG_CURSOR_BLINK;
  }
  hd44780_send_instruction(cfg, instruction);
}

void hd44780_init(hd44780_hdl const* const cfg) {
  cfg->cb_ctrl_pin(HD44780_PIN_RS, PIN_RESET);

  hd44780_send_byte_raw(cfg, REG_INTERFACE | REG_8_BIT_BUS);
  cfg->cb_delay_ms(DELAY_INIT_SEQ_LONG_MS);
  for (unsigned char i = 0U; i < 2U; i++) {
    hd44780_send_byte_raw(cfg, REG_INTERFACE | REG_8_BIT_BUS);
    cfg->cb_delay_ms(DELAY_INIT_SEQ_SHORT_MS);
  }

  if (cfg->interface == REG_4_BIT_BUS) {
    hd44780_send_byte_raw(cfg, REG_INTERFACE | REG_4_BIT_BUS);
    cfg->cb_delay_ms(DELAY_INIT_SEQ_SHORT_MS);
    hd44780_send_instruction(cfg, REG_INTERFACE | REG_FONT_SIZE_5X8 | REG_TWO_LINES | REG_4_BIT_BUS);
  } else {
    hd44780_send_byte_raw(cfg, REG_INTERFACE | REG_FONT_SIZE_5X8 | REG_TWO_LINES | REG_8_BIT_BUS);
  }

  hd44780_display_off(cfg);
  hd44780_clear(cfg);
  hd44780_send_instruction(cfg, REG_EM | REG_EM_SHIFT_CURSOR | REG_EM_INCREMENT);
  hd44780_cursor_cfg(cfg, CURSOR_OFF);
}

void hd44780_display_off(hd44780_hdl const* const cfg) {
  hd44780_send_instruction(cfg, REG_PWR_AND_CURSOR | REG_DISPLAY_OFF);
}

void hd44780_def_char(hd44780_hdl const* const cfg, unsigned char const index, unsigned char const* const pattern) {
  hd44780_send_instruction(cfg, REG_CGRAM_ADDR_SET + (index * 8));
  for (unsigned char i = 0; i < 8; i++) {
    hd44780_send_data(cfg, pattern[i]);
  }
}

void hd44780_disp_char(hd44780_hdl const* const cfg, unsigned char const index) { hd44780_send_data(cfg, index); }
