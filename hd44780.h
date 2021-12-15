/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala/hd44780 */

#ifndef __HD44780__H__
#define __HD44780__H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of interface
 */
typedef enum { INTERFACE_4BIT, INTERFACE_8BIT } hd44780_interface_t;

/**
 * @brief Setting of cursor
 */
typedef enum { CURSOR_OFF, CURSOR_ON, CURSOR_BLINK } hd44780_cursor_t;

/**
 * @brief Control pins (E, RS, RW)
 */
typedef enum { HD44780_PIN_RS, HD44780_PIN_RW, HD44780_PIN_E } hd44780_ctrl_pin_t;

/**
 * @brief Control pins (E, RS, RW) possible state
 */
typedef enum { PIN_RESET, PIN_SET } hd44780_pin_state_t;

/**
 * @brief Data bus GPIO pin direction
 */
typedef enum { GPIO_DIR_IN, GPIO_DIR_OUT } hd44780_gpio_dir_t;

/**
 * @brief Callback function setting GPIO pin direction
 * @note This function manage both data pins D0...D7 (8bit transfer)
 * or D4...D7 (4bit transfer) as well as E, RW, RS and those 3 control
 * pins should be allways set as output regardless function argument
 */
typedef void (*cb_config_gpio_t)(hd44780_gpio_dir_t);

/**
 * @brief Callback function providing miliseconds long waiting during init
 */
typedef void (*cb_delay_ms_t)(unsigned char);

/**
 * @brief Callback function setting control pins (E, RS, RW) to given state
 */
typedef void (*cb_ctrl_pin_t)(hd44780_ctrl_pin_t, hd44780_pin_state_t);

/**
 * @brief Callback function reading data bus
 */
typedef unsigned char (*cb_read_bus_t)(void);

/**
 * @brief Callback function writing data byte on bus
 * @note When using 4bit transfer ignore the 4 least significant bits
 */
typedef void (*cb_write_bus_t)(unsigned char);

/**
 * @brief Config struct
 * @note Five callback function pointers have to be set
 */
typedef struct {
  cb_config_gpio_t cb_config_gpio;
  cb_ctrl_pin_t cb_ctrl_pin;
  cb_read_bus_t cb_read_bus;
  cb_write_bus_t cb_write_bus;
  cb_delay_ms_t cb_delay_ms;
  hd44780_interface_t interface;
} hd44780_hdl;

/**
 * @brief Initialise HD44780 LCD
 * @param cfg [in] pointer to configuration
 * @note Delay 15ms after power on have to be done prior to calling this function
 */
void hd44780_init(hd44780_hdl const *const cfg);

/**
 * @brief Read memory address and busy flag
 * @param cfg [in] pointer to configuration
 * @return byte read (busy flag at most significant bit)
 */
unsigned char hd44780_read_address(hd44780_hdl const *const cfg);

/**
 * @brief Read memory data
 * @param cfg [in] pointer to configuration
 * @return byte read
 */
unsigned char hd44780_read_data(hd44780_hdl const *const cfg);

/**
 * @brief Write string to lcd
 * @param cfg [in] pointer to configuration
 * @param text [in] null terminated string
 */
void hd44780_write_text(hd44780_hdl const *const cfg, char const *);

/**
 * @brief Sets data display ram address
 * @note It is inteded to provide you direct access to DDRAM
 * @param cfg [in] pointer to configuration
 * @param address [in] address
 */
void hd44780_set_ddram_addr(hd44780_hdl const *const cfg, unsigned char address);

/**
 * @brief Set cursos in desired position
 * @param cfg [in] pointer to configuration
 * @param row [in] row number (0 is at the top)
 * @param column [in] column number (0 is the leftmost)
 */
void hd44780_goto(hd44780_hdl const *const cfg, unsigned char row, unsigned char column);

/**
 * @brief Clears whole display
 * @param cfg [in] pointer to configuration
 */
void hd44780_clear(hd44780_hdl const *const cfg);

/**
 * @brief Configure cursor type
 * @note Cursor setting is one of settings that can be changed after init
 * @param cfg [in] pointer to configuration
 * @param cursor_cfg [in] cursor configuration
 */
void hd44780_cursor_cfg(hd44780_hdl const *const cfg, hd44780_cursor_t const cursor_cfg);

/**
 * @brief Turns of display
 * @note To turn the display on again use init function
 * @param cfg [in] pointer to configuration
 */
void hd44780_display_off(hd44780_hdl const *const cfg);

/**
 * @brief Defines custom character
 * @param cfg [in] pointer to configuration
 * @param index [in] index of character in memory (starts with 0)
 * @param pattern [in] address to 8byte lenght character patern array
 */
void hd44780_def_char(hd44780_hdl const *const cfg, unsigned char const index, unsigned char const *const pattern);

/**
 * @brief Shows previously defined custom character
 * @param cfg [in] pointer to configuration
 * @param index [in] index of character in memory (starts with 0)
 */
void hd44780_disp_char(hd44780_hdl const *const cfg, unsigned char const index);

#ifdef __cplusplus
}
#endif

#endif /* __HD44780__H__ */
