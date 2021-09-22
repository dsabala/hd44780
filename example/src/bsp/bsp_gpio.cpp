/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala */

#include "assertions.hpp"
#include "stm32f4xx_hal.h"
#include "bsp_gpio.hpp"

namespace bsp
{
    // clang-format off
    #define PC14_OSC32_IN_Pin        GPIO_PIN_14
    #define PC14_OSC32_IN_GPIO_Port  GPIOC
    #define PC15_OSC32_OUT_Pin       GPIO_PIN_15
    #define PC15_OSC32_OUT_GPIO_Port GPIOC
    #define PH0_OSC_IN_Pin           GPIO_PIN_0
    #define PH0_OSC_IN_GPIO_Port     GPIOH
    #define PH1_OSC_OUT_Pin          GPIO_PIN_1
    #define PH1_OSC_OUT_GPIO_Port    GPIOH
    #define BUTTON_BLUE_Pin          GPIO_PIN_0
    #define BUTTON_BLUE_GPIO_Port    GPIOA
    #define LED_GREEN_Pin            GPIO_PIN_12
    #define LED_GREEN_GPIO_Port      GPIOD
    #define LED_ORANGE_Pin           GPIO_PIN_13
    #define LED_ORANGE_GPIO_Port     GPIOD
    #define LED_RED_Pin              GPIO_PIN_14
    #define LED_RED_GPIO_Port        GPIOD
    #define LED_BLUE_Pin             GPIO_PIN_15
    #define LED_BLUE_GPIO_Port       GPIOD
    #define SWDIO_Pin                GPIO_PIN_13
    #define SWDIO_GPIO_Port          GPIOA
    #define SWCLK_Pin                GPIO_PIN_14
    #define SWCLK_GPIO_Port          GPIOA
    #define SWO_Pin                  GPIO_PIN_3
    #define SWO_GPIO_Port            GPIOB
    // clang-format on

    void InitGpio()
    {
        GPIO_InitTypeDef GPIO_InitStruct = {};

        /* GPIO Ports Clock Enable */
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(GPIOD, LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin, GPIO_PIN_RESET);

        /* Configure GPIO pin : BUTTON_BLUE_Pin */
        GPIO_InitStruct.Pin  = BUTTON_BLUE_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(BUTTON_BLUE_GPIO_Port, &GPIO_InitStruct);

        /* Configure GPIO pins : LED_GREEN_Pin LED_ORANGE_Pin LED_RED_Pin LED_BLUE_Pin */
        GPIO_InitStruct.Pin   = LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
} // namespace bsp
