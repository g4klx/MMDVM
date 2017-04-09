/**
  ******************************************************************************
  * @file    system_stm32f1xx.c
  * @author  MCD Application Team, Wojciech Krutnik
  * @version V2.2.2
  * @date    26-June-2015
  * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer System Source File.
  *
  * 1. This file provides two functions and one global variable to be called from
  *    user application:
  *      - SystemInit(): This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f1xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  * 2. After each device reset the HSI (8 MHz) is used as system clock source.
  *    Then SystemInit() function is called, in "startup_stm32f1xx.s" file, to
  *    configure the system clock before to branch to main program.
  *
  * 3. This file configures the system clock and flash as follows:
  *=============================================================================
  *                         Supported STM32F1xx device
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL
  *-----------------------------------------------------------------------------
  *        SYSCLK                                 | 72MHz
  *-----------------------------------------------------------------------------
  *        HCLK                                   | 72Mhz
  *-----------------------------------------------------------------------------
  *        PCLK1                                  | 36MHz
  *-----------------------------------------------------------------------------
  *        PCLK2                                  | 72MHz
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *        PLLSRC                                 | HSE/1
  *-----------------------------------------------------------------------------
  *        PLLMUL                                 | 9
  *-----------------------------------------------------------------------------
  *        HSE                                    | 8MHz
  *-----------------------------------------------------------------------------
  *        Flash Latency(WS)                      | 2
  *-----------------------------------------------------------------------------
  *        Prefetch Buffer                        | ON
  *-----------------------------------------------------------------------------
  *=============================================================================
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#if defined(STM32F105xC)

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f1xx_system
  * @{
  */

/** @addtogroup STM32F1xx_System_Private_Includes
  * @{
  */

#include "stm32f1xx.h"

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)8000000) /*!< Default value of the External oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)8000000) /*!< Default value of the Internal oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSI_VALUE */
/**
  * @}
  */

/** @addtogroup STM32f1xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_Variables
  * @{
  */

uint32_t SystemCoreClock = 48000000;

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_FunctionPrototypes
  * @{
  */

static void SetSysClock(void);

/**
  * @}
  */

/** @addtogroup STM32F1xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  *         Initialize the default HSI clock source, vector table location and the PLL configuration is reset.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  /* Enable Prefetch Buffer, 2 wait states */
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;

  /* Configure the System clock frequency, AHB/APBx prescalers */
  SetSysClock();
}


/**
  * @brief  Configures the System clock frequency and AHB/APBx prescalers
  *         settings.
  * @param  None
  * @retval None
  */
static void SetSysClock(void)
{
  /* Enable HSE and Clock Security System */
  RCC->CR |= RCC_CR_CSSON | RCC_CR_HSEON;
  /* Wait for HSE startup */
  while((RCC->CR & RCC_CR_HSERDY) == 0)
    ;
  
  /* Enable PLL with 9x multiplier and HSE clock source
   * APB1 prescaler /2 */
  RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLMULL)) | RCC_CFGR_PLLMULL9
              | RCC_CFGR_PLLSRC;
  RCC->CR |= RCC_CR_PLLON;
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
    ;
  
  /* HCLK = SYSCLK */
  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV1;
  /* PCLK1 = HCLK/2; PCLK2 = HCLK */
  RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PPRE1|RCC_CFGR_PPRE2))
              | RCC_CFGR_PPRE1_DIV2;
  
  /* Switch SYSCLK to PLL */
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
  
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

