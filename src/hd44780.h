/* Dariusz Sabala, 2023, MIT License, https://github.com/dsabala/hd44780 */

#ifndef __HD44780__H__
#define __HD44780__H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HD44780_TIMEOUT_MS
  /** @brief Timeout on busy flag [ms] */
  #define HD44780_TIMEOUT_MS    (100U)
#endif

#ifndef HD44780_TIMEOUT_TICK_MS
  /** @brief Timeout check tick time [ms] */
  #define HD44780_TIMEOUT_TICK_MS    (1U)
#endif

#ifndef DELAY_INIT_SEQ_LONG_MS
  /** @brief Initialisation delay - long period length [ms] */
  #define DELAY_INIT_SEQ_LONG_MS    (50U)
#endif

#ifndef DELAY_INIT_SEQ_SHORT_MS
  /** @brief Initialisation delay - short period length [ms] */
  #define DELAY_INIT_SEQ_SHORT_MS    (10U)
#endif

/** @brief Status codes */
typedef enum {
  HD44780_OK = 0,                 /**< Success */
  HD44780_INV_ARG = 1,            /**< Invalid argument */
  HD44780_TIMEOUT = 2,            /**< Timeout */
  HD44780_CUSTOM_CHARS_INV = 3,   /**< Custom character array invalid */
  HD44780_CHAR_NOT_FOUND = 4,     /**< Custom character not found */
} hd44780_ret_e;

/** @brief Type of interface */
typedef enum { 
  INTERFACE_4BIT,   /**< 4-bit parallel interface (used pins D4 ... D7) */
  INTERFACE_8BIT    /**< 8-bit parallel interface (used pins D0 ... D7) */
} hd44780_interface;

/** @brief Setting of cursor */
typedef enum { 
  CURSOR_OFF,    /**< Cursor is turned off */
  CURSOR_ON,     /**< Cursor is turned on and it is steady */
  CURSOR_BLINK   /**< Cursor is turned on and it is blinking */
} hd44780_cursor;

/** @brief Control pins (E, RS, RW) */
typedef enum { 
  HD44780_PIN_RS,   /**< Register Select pin 0 = instruction, 1 = data */
  HD44780_PIN_RW,   /**< Read / Write pin 0 = write, 1 = read */
  HD44780_PIN_E     /**< Start data read write pin */
} hd44780_ctrl_pin;

/** @brief Control pins (E, RS, RW) possible state */
typedef enum { 
  PIN_RESET,   /**< Pin set to low state */
  PIN_SET      /**< Pin set to high state */
} hd44780_pin_state;

/** @brief Data bus GPIO pin direction */
typedef enum { 
  GPIO_DIR_IN,  /**< Configure pin as GPIO input */
  GPIO_DIR_OUT  /**< Configure pin as GPIO output */
} hd44780_gpio_dir;

/** @brief Custom character mapping array element */
typedef struct {
  uint32_t utf_8_code;            /**< UTF code */
  uint8_t character_bitmap[8U];   /**< Mapped character bitmap */
} character_mapping;

/* Forward declaration of struct */
struct hd44780_ctx_s;

/** @brief Driver context */
typedef struct hd44780_ctx_s {
  /**
   * @brief Callback used to initialise common resources
   * 
   * @details This callback is responsible for:
   *          - initialising E, RW and RS pins as GPIO output push-pull pins
   */
  void (*cb_init_common)(void);
  /**
   * @brief Callback used to change direction of LCD bus GPIO pins
   * 
   * @details callback responsibility:
   *          - configure LCD bus (pins D4 ... D7 or D0 ... D7) 
   *            as GPIO, push-pull outputs or high Z inputs
   */
  void (*cb_set_bus_direction)(hd44780_gpio_dir);
  /**
   * @brief Callback used to change state of one of control pins (E, RS or RW)
   * 
   * @details callback responsibility:
   *          - change output state of control pin (E, RS or RW)
   */
  void (*cb_set_ctrl_pin_state)(hd44780_ctrl_pin, hd44780_pin_state);
  /**
   * @brief Callback used read display bus
   * 
   * @details callback responsibility:
   *          - read state of bus GPIO pins
   *          - in case of 4 bit bus, shift D4 ... D7 bits 
   *            onto 4 ... 7 bits of return octet, bits 0 ... 3 are ignored
   */
  uint8_t (*cb_read_bus)(void);
  /**
   * @brief Callback used write data on display bus
   * 
   * @details callback responsibility:
   *          - set state of GPIO output pins of display bus
   *          - in case of 4 bit bus, bits 0 ... 3 are ignored
   */
  void (*cb_write_bus)(uint8_t);
  /**
   * @brief Callback used for miliseconds delay during LCD initialisation
   * 
   * @param[in] time_ms time [ms]
   */
  void (*cb_delay_ms)(uint8_t time_ms);

  /**
   * @brief Callback used to wait for busy flag clear
   * 
   * @param[in] ctx driver context
   * 
   * @return status
   * 
   * @retval HD44780_OK        Success
   * @retval HD44780_TIMEOUT   Timeout
   */
  hd44780_ret_e (*cb_wait_for_busy_flag_clear)(const struct hd44780_ctx_s* const ctx);
  /** 
   * @brief Custom character map pointer 
   */
  const character_mapping* custom_chars_map;
  /** 
   * @brief Custom character map length 
   */
  uint8_t custom_chars_map_len;
  /**
   * @brief Number of lines (1, 2 or 4)
   */
  uint8_t number_of_lines;
  /**
   * @brief Width of display column (usually 8, 10, 16, 20 or 40)
   */
  uint8_t column_width;
  /** 
   * @brief Interface bus width 
   */
  hd44780_interface interface;
} hd44780_ctx;

/**
 * @brief Initialise HD44780 LCD
 * 
 * @param[in] ctx driver context
 * 
 * @note Delay 15ms after power on have to be done prior to calling this function
 * 
 * @return status
 * @retval HD44780_OK               Success
 * @retval HD44780_TIMEOUT          Timeout
 * @retval HD44780_CUSTOM_CHARS_INV Custom characters array is invalid (too big)
 */
hd44780_ret_e hd44780_init(const hd44780_ctx* const ctx);

/**
 * @brief Clears whole display
 * 
 * @param[in] ctx driver context
 * 
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_TIMEOUT Timeout
 */
hd44780_ret_e hd44780_clear(const hd44780_ctx* const ctx);

/**
 * @brief Write string on LCD, starting from current position
 * 
 * @param[in] ctx driver context
 * @param[in] text null terminated string to be displayed
 * 
 * @note - This function will not stop the display from jumping to another line,
 *       for example in case of some displays it might jump two lines below, since
 *       this is how HD44780 map memory is organised
 *       - UTF-8 characters are supported. Characters are searched from 
 *       ctx->custom_chars_map map and displayed
 * 
 * @return status
 * @retval HD44780_OK                Success
 * @retval HD44780_TIMEOUT           Timeout
 * @retval HD44780_CHAR_NOT_FOUND    Character not found in custom chars array
 */
hd44780_ret_e hd44780_write_text(const hd44780_ctx* const ctx, const char* text);

/**
 * @brief Set cursor at desired position
 * 
 * @param[in] ctx driver context
 * @param[in] row row number (0 is at the top)
 * @param[in] column column number (0 is the leftmost)
 * 
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_INV_ARG Invalid position choosed
 * @retval HD44780_TIMEOUT Timeout
 */
hd44780_ret_e hd44780_set_pos(const hd44780_ctx* const ctx, uint8_t row, uint8_t column);

/**
 * @brief Configure cursor type
 * 
 * @param[in] ctx driver context
 * @param[in] cursor_cfg cursor configuration
 * 
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_INV_ARG Invalid cursor config
 * @retval HD44780_TIMEOUT Timeout
 */
hd44780_ret_e hd44780_cursor_cfg(const hd44780_ctx* const ctx, hd44780_cursor cursor_cfg);

/**
 * @brief Turn display off
 * 
 * @note To turn the display on again use init function
 * 
 * @param[in] ctx driver context
 *
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_TIMEOUT Timeout
 */
hd44780_ret_e hd44780_display_off(const hd44780_ctx* const ctx);

/**
 * @brief Define custom character
 * 
 * @param[in] ctx driver context
 * @param[in] index index of character in memory (starts with 0)
 * @param[in] pattern pointer to 8 byte character patern array
 * 
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_TIMEOUT Timeout
 */
hd44780_ret_e hd44780_def_char(const hd44780_ctx* const ctx, uint8_t index, const uint8_t* const pattern);

/**
 * @brief Show custom character
 * 
 * @param[in] ctx driver context
 * @param[in] index index of character in memory (starts with 0)
 * 
 * @return status
 * @retval HD44780_OK      Success
 * @retval HD44780_TIMEOUT Timeout
 */
hd44780_ret_e hd44780_disp_char(const hd44780_ctx* const ctx, uint8_t index);

/**
 * @brief Check if display is bussy
 * 
 * @details This function is intended to be used only in one of callback functions
 *          to make it easy to implement efficient wait mechanism 
 *
 * @param[in] ctx context 
 * 
 * @return true    display is busy (busy flag is set)
 * @return false   display is not busy (busy flag is cleared)
 */
bool hd44780_is_busy(const hd44780_ctx* const ctx);

#ifdef __cplusplus
}
#endif

#endif /* __HD44780__H__ */
