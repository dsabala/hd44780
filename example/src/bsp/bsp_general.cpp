/* Dariusz Sabala, 2021, MIT License, https://github.com/dsabala */

#include "stm32f4xx_hal.h"
#include "bsp_general.hpp"

namespace bsp
{
    void Init()
    {
        HAL_Init();
        InitClocks();
        InitGpio();
    }
}

/**
  * Initializes the Global MSP.
  */
extern "C" void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}
