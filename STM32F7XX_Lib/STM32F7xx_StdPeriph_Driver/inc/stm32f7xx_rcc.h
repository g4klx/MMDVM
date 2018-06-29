
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_RCC_H
#define __STM32F7xx_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"

/** @addtogroup STM32F7xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup RCC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint32_t SYSCLK_Frequency; /*!<  SYSCLK clock frequency expressed in Hz */
  uint32_t HCLK_Frequency;   /*!<  HCLK clock frequency expressed in Hz   */
  uint32_t PCLK1_Frequency;  /*!<  PCLK1 clock frequency expressed in Hz  */
  uint32_t PCLK2_Frequency;  /*!<  PCLK2 clock frequency expressed in Hz  */
}RCC_ClocksTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup RCC_Exported_Constants
  * @{
  */
  
/** @defgroup RCC_HSE_configuration 
  * @{
  */
#define RCC_HSE_OFF                      ((uint8_t)0x00)
#define RCC_HSE_ON                       ((uint8_t)0x01)
#define RCC_HSE_Bypass                   ((uint8_t)0x05)
#define IS_RCC_HSE(HSE)                  (((HSE) == RCC_HSE_OFF) || ((HSE) == RCC_HSE_ON) || \
                                         ((HSE) == RCC_HSE_Bypass))
/**
  * @}
  */ 

/** @defgroup RCC_LSE_Dual_Mode_Selection
  * @{
  */
#define RCC_LSE_LOWPOWER_MODE            ((uint32_t)0x00000000)
#define RCC_LSE_MEDIUMLOW_MODE           RCC_BDCR_LSEDRV_1
#define RCC_LSE_MEDIUMHIGH_MODE          RCC_BDCR_LSEDRV_0
#define RCC_LSE_HIGHDRIVE_MODE           RCC_BDCR_LSEDRV
#define IS_RCC_LSE_MODE(MODE)           (((MODE) == RCC_LSE_LOWPOWER_MODE) || \
                                         ((MODE) == RCC_LSE_MEDIUMLOW_MODE) || \
                                         ((MODE) == RCC_LSE_MEDIUMHIGH_MODE) || \
                                         ((MODE) == RCC_LSE_HIGHDRIVE_MODE))


/** @defgroup RCC_PLLSAIDivR_Factor
  * @{
  */
#define RCC_PLLSAIDivR_Div2                ((uint32_t)0x00000000)
#define RCC_PLLSAIDivR_Div4                ((uint32_t)0x00010000)
#define RCC_PLLSAIDivR_Div8                ((uint32_t)0x00020000)
#define RCC_PLLSAIDivR_Div16               ((uint32_t)0x00030000)
#define IS_RCC_PLLSAI_DIVR_VALUE(VALUE) (((VALUE) == RCC_PLLSAIDivR_Div2) ||\
                                        ((VALUE) == RCC_PLLSAIDivR_Div4)  ||\
                                        ((VALUE) == RCC_PLLSAIDivR_Div8)  ||\
                                        ((VALUE) == RCC_PLLSAIDivR_Div16))
/**
  * @}
  */

/** @defgroup RCC_PLL_Clock_Source 
  * @{
  */
#define RCC_PLLSource_HSI                ((uint32_t)0x00000000)
#define RCC_PLLSource_HSE                ((uint32_t)0x00400000)
#define IS_RCC_PLL_SOURCE(SOURCE) (((SOURCE) == RCC_PLLSource_HSI) || \
                                   ((SOURCE) == RCC_PLLSource_HSE))

#define IS_RCC_PLLM_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 63))
#define IS_RCC_PLLN_VALUE(VALUE) ((192 <= (VALUE)) && ((VALUE) <= 432))

/** @defgroup RCC_PLLP_Clock_Divider PLLP Clock Divider
  * @{
  */
#define RCC_PLLP_DIV2                  ((uint32_t)0x00000002)
#define RCC_PLLP_DIV4                  ((uint32_t)0x00000004)
#define RCC_PLLP_DIV6                  ((uint32_t)0x00000006)
#define RCC_PLLP_DIV8                  ((uint32_t)0x00000008)
#define IS_RCC_PLLP_VALUE(VALUE) (((VALUE) == RCC_PLLP_DIV2) || ((VALUE) == RCC_PLLP_DIV4) || \
                                  ((VALUE) == RCC_PLLP_DIV6) || ((VALUE) == RCC_PLLP_DIV8))
/**
  * @}
  */

#define IS_RCC_PLLQ_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 15))

#define IS_RCC_PLLI2SN_VALUE(VALUE) ((49 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLI2SP_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 8))
#define IS_RCC_PLLI2SQ_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 15))
#define IS_RCC_PLLI2SR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))

#define IS_RCC_PLLSAIN_VALUE(VALUE) ((49 <= (VALUE)) && ((VALUE) <= 432))
#define IS_RCC_PLLSAIP_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 8))
#define IS_RCC_PLLSAIQ_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 15))
#define IS_RCC_PLLSAIR_VALUE(VALUE) ((2 <= (VALUE)) && ((VALUE) <= 7))  

#define IS_RCC_PLLSAI_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))
#define IS_RCC_PLLI2S_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))

/**
  * @}
  */

/** @defgroup RCC_System_Clock_Source 
  * @{
  */

#define RCC_SYSCLKSource_HSI             ((uint32_t)RCC_CFGR_SW_HSI)
#define RCC_SYSCLKSource_HSE             ((uint32_t)RCC_CFGR_SW_HSE)
#define RCC_SYSCLKSource_PLLPCLK         ((uint32_t)RCC_CFGR_SW_PLL)

/* Add legacy definition */
#define  RCC_SYSCLKSource_PLLCLK    RCC_SYSCLKSource_PLLPCLK  

#define IS_RCC_SYSCLK_SOURCE(SOURCE) (((SOURCE) == RCC_SYSCLKSource_HSI) || \
                                      ((SOURCE) == RCC_SYSCLKSource_HSE) || \
                                      ((SOURCE) == RCC_SYSCLKSource_PLLPCLK))
/**
  * @}
  */

/** @defgroup RCC_AHB_Clock_Source
  * @{
  */
#define RCC_SYSCLK_Div1                  ((uint32_t)RCC_CFGR_HPRE_DIV1)
#define RCC_SYSCLK_Div2                  ((uint32_t)RCC_CFGR_HPRE_DIV2)
#define RCC_SYSCLK_Div4                  ((uint32_t)RCC_CFGR_HPRE_DIV4)
#define RCC_SYSCLK_Div8                  ((uint32_t)RCC_CFGR_HPRE_DIV8)
#define RCC_SYSCLK_Div16                 ((uint32_t)RCC_CFGR_HPRE_DIV16)
#define RCC_SYSCLK_Div64                 ((uint32_t)RCC_CFGR_HPRE_DIV64)
#define RCC_SYSCLK_Div128                ((uint32_t)RCC_CFGR_HPRE_DIV128)
#define RCC_SYSCLK_Div256                ((uint32_t)RCC_CFGR_HPRE_DIV256)
#define RCC_SYSCLK_Div512                ((uint32_t)RCC_CFGR_HPRE_DIV512)
#define IS_RCC_HCLK(HCLK) (((HCLK) == RCC_SYSCLK_Div1) || ((HCLK) == RCC_SYSCLK_Div2) || \
                           ((HCLK) == RCC_SYSCLK_Div4) || ((HCLK) == RCC_SYSCLK_Div8) || \
                           ((HCLK) == RCC_SYSCLK_Div16) || ((HCLK) == RCC_SYSCLK_Div64) || \
                           ((HCLK) == RCC_SYSCLK_Div128) || ((HCLK) == RCC_SYSCLK_Div256) || \
                           ((HCLK) == RCC_SYSCLK_Div512))
/**
  * @}
  */

/** @defgroup RCC_APB1_APB2_Clock_Source
  * @{
  */
#define RCC_HCLK_Div1                    ((uint32_t)RCC_CFGR_PPRE1_DIV1)
#define RCC_HCLK_Div2                    ((uint32_t)RCC_CFGR_PPRE1_DIV2)
#define RCC_HCLK_Div4                    ((uint32_t)RCC_CFGR_PPRE1_DIV4)
#define RCC_HCLK_Div8                    ((uint32_t)RCC_CFGR_PPRE1_DIV8)
#define RCC_HCLK_Div16                   ((uint32_t)RCC_CFGR_PPRE1_DIV16)
#define IS_RCC_PCLK(PCLK) (((PCLK) == RCC_HCLK_Div1) || ((PCLK) == RCC_HCLK_Div2) || \
                           ((PCLK) == RCC_HCLK_Div4) || ((PCLK) == RCC_HCLK_Div8) || \
                           ((PCLK) == RCC_HCLK_Div16))
/**
  * @}
  */ 
  
/** @defgroup RCC_Interrupt_Source 
  * @{
  */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01)
#define RCC_IT_LSERDY                    ((uint8_t)0x02)
#define RCC_IT_HSIRDY                    ((uint8_t)0x04)
#define RCC_IT_HSERDY                    ((uint8_t)0x08)
#define RCC_IT_PLLRDY                    ((uint8_t)0x10)
#define RCC_IT_PLLI2SRDY                 ((uint8_t)0x20) 
#define RCC_IT_PLLSAIRDY                 ((uint8_t)0x40)
#define RCC_IT_CSS                       ((uint8_t)0x80)

#define IS_RCC_IT(IT) ((((IT) & (uint8_t)0x80) == 0x00) && ((IT) != 0x00))
#define IS_RCC_GET_IT(IT) (((IT) == RCC_IT_LSIRDY) || ((IT) == RCC_IT_LSERDY) || \
                           ((IT) == RCC_IT_HSIRDY) || ((IT) == RCC_IT_HSERDY) || \
                           ((IT) == RCC_IT_PLLRDY) || ((IT) == RCC_IT_CSS) || \
                           ((IT) == RCC_IT_PLLSAIRDY) || ((IT) == RCC_IT_PLLI2SRDY))
#define IS_RCC_CLEAR_IT(IT)((IT) != 0x00)

/**
  * @}
  */ 

/** @defgroup RCC_LSE_Config RCC LSE Config
  * @{
  */
#define RCC_LSE_OFF                      ((uint8_t)0x00)
#define RCC_LSE_ON                       ((uint8_t)0x01)
#define RCC_LSE_Bypass                   ((uint8_t)0x04)
#define IS_RCC_LSE(LSE) (((LSE) == RCC_LSE_OFF) || ((LSE) == RCC_LSE_ON) || \
                         ((LSE) == RCC_LSE_Bypass))
 
/** @defgroup RCC_RTC_Clock_Source
  * @{
  */
#define RCC_RTCCLKSource_LSE             ((uint32_t)0x00000100)
#define RCC_RTCCLKSource_LSI             ((uint32_t)0x00000200)
#define RCC_RTCCLKSource_HSE_Div2        ((uint32_t)0x00020300)
#define RCC_RTCCLKSource_HSE_Div3        ((uint32_t)0x00030300)
#define RCC_RTCCLKSource_HSE_Div4        ((uint32_t)0x00040300)
#define RCC_RTCCLKSource_HSE_Div5        ((uint32_t)0x00050300)
#define RCC_RTCCLKSource_HSE_Div6        ((uint32_t)0x00060300)
#define RCC_RTCCLKSource_HSE_Div7        ((uint32_t)0x00070300)
#define RCC_RTCCLKSource_HSE_Div8        ((uint32_t)0x00080300)
#define RCC_RTCCLKSource_HSE_Div9        ((uint32_t)0x00090300)
#define RCC_RTCCLKSource_HSE_Div10       ((uint32_t)0x000A0300)
#define RCC_RTCCLKSource_HSE_Div11       ((uint32_t)0x000B0300)
#define RCC_RTCCLKSource_HSE_Div12       ((uint32_t)0x000C0300)
#define RCC_RTCCLKSource_HSE_Div13       ((uint32_t)0x000D0300)
#define RCC_RTCCLKSource_HSE_Div14       ((uint32_t)0x000E0300)
#define RCC_RTCCLKSource_HSE_Div15       ((uint32_t)0x000F0300)
#define RCC_RTCCLKSource_HSE_Div16       ((uint32_t)0x00100300)
#define RCC_RTCCLKSource_HSE_Div17       ((uint32_t)0x00110300)
#define RCC_RTCCLKSource_HSE_Div18       ((uint32_t)0x00120300)
#define RCC_RTCCLKSource_HSE_Div19       ((uint32_t)0x00130300)
#define RCC_RTCCLKSource_HSE_Div20       ((uint32_t)0x00140300)
#define RCC_RTCCLKSource_HSE_Div21       ((uint32_t)0x00150300)
#define RCC_RTCCLKSource_HSE_Div22       ((uint32_t)0x00160300)
#define RCC_RTCCLKSource_HSE_Div23       ((uint32_t)0x00170300)
#define RCC_RTCCLKSource_HSE_Div24       ((uint32_t)0x00180300)
#define RCC_RTCCLKSource_HSE_Div25       ((uint32_t)0x00190300)
#define RCC_RTCCLKSource_HSE_Div26       ((uint32_t)0x001A0300)
#define RCC_RTCCLKSource_HSE_Div27       ((uint32_t)0x001B0300)
#define RCC_RTCCLKSource_HSE_Div28       ((uint32_t)0x001C0300)
#define RCC_RTCCLKSource_HSE_Div29       ((uint32_t)0x001D0300)
#define RCC_RTCCLKSource_HSE_Div30       ((uint32_t)0x001E0300)
#define RCC_RTCCLKSource_HSE_Div31       ((uint32_t)0x001F0300)
#define IS_RCC_RTCCLK_SOURCE(SOURCE) (((SOURCE) == RCC_RTCCLKSource_LSE) || \
                                      ((SOURCE) == RCC_RTCCLKSource_LSI) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div2) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div3) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div4) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div5) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div6) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div7) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div8) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div9) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div10) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div11) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div12) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div13) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div14) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div15) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div16) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div17) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div18) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div19) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div20) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div21) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div22) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div23) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div24) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div25) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div26) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div27) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div28) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div29) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div30) || \
                                      ((SOURCE) == RCC_RTCCLKSource_HSE_Div31))
/**
  * @}
  */ 
    
/** @defgroup RCC_I2S_Clock_Source
  * @{
  */
#define RCC_I2SCLKSource_PLLI2S             ((uint32_t)0x00)
#define RCC_I2SCLKSource_Ext                ((uint32_t)RCC_CFGR_I2SSRC)


#define IS_RCC_I2SCLK_SOURCE(SOURCE) (((SOURCE) == RCC_I2SCLKSource_PLLI2S) || ((SOURCE) == RCC_I2SCLKSource_Ext))                                
/**
  * @}
  */

/** @defgroup RCC_SAI_Clock_Source
  * @{
  */
#define RCC_SAICLKSource_PLLSAI             ((uint32_t)0x00)
#define RCC_SAICLKSource_PLLI2S             ((uint32_t)RCC_DCKCFGR1_SAI2SEL_0)
#define RCC_SAICLKSource_CKIN               ((uint32_t)RCC_DCKCFGR1_SAI2SEL_1)

#define IS_RCC_SAICLK_SOURCE(SOURCE) (((SOURCE) == RCC_SAICLKSource_PLLSAI) || ((SOURCE) == RCC_SAICLKSource_PLLI2S) || \
                                      ((SOURCE) == RCC_SAICLKSource_CKIN))                                
/**
  * @}
  */    
    
/** @defgroup RCC_SAI_Instance
  * @{
  */
#define RCC_SAIInstance_SAI1             ((uint8_t)0x00)
#define RCC_SAIInstance_SAI2             ((uint8_t)0x01)
#define IS_RCC_SAI_INSTANCE(BUS) (((BUS) == RCC_SAIInstance_SAI1) || ((BUS) == RCC_SAIInstance_SAI2))                                
/**
  * @}
  */

/** @defgroup RCC_TIM_PRescaler_Selection
  * @{
  */
#define RCC_TIMPrescDesactivated             ((uint8_t)0x00)
#define RCC_TIMPrescActivated                ((uint8_t)0x01)

#define IS_RCC_TIMCLK_PRESCALER(VALUE) (((VALUE) == RCC_TIMPrescDesactivated) || ((VALUE) == RCC_TIMPrescActivated))
/**
  * @}
  */

/** @defgroup RCC_SDIO_Clock_Source_Selection
  * @{
  */
#define RCC_SDMMCCLKSource_48MHZ              ((uint8_t)0x00)
#define RCC_SDMMCCLKSource_SYSCLK             ((uint8_t)0x01)
#define IS_RCC_SDMMC_CLOCKSOURCE(CLKSOURCE)   (((CLKSOURCE) == RCC_SDMMCCLKSource_48MHZ) || \
                                              ((CLKSOURCE) == RCC_SDMMCCLKSource_SYSCLK))
/**
  * @}
  */

/** @defgroup RCC_48MHZ_Clock_Source_Selection
  * @{
  */
#define RCC_48MHZCLKSource_PLL                ((uint8_t)0x00)
#define RCC_48MHZCLKSource_PLLSAI             ((uint8_t)0x01)
#define IS_RCC_48MHZ_CLOCKSOURCE(CLKSOURCE)   (((CLKSOURCE) == RCC_48MHZCLKSource_PLL) || \
                                               ((CLKSOURCE) == RCC_48MHZCLKSource_PLLSAI))
/**
  * @}
  */

/** @defgroup RCC_SPDIFRX_Clock_Source_Selection
  * @{
  */
#define RCC_SPDIFRXCLKSource_PLLR                 ((uint8_t)0x00)
#define RCC_SPDIFRXCLKSource_PLLI2SP              ((uint8_t)0x01)
#define IS_RCC_SPDIFRX_CLOCKSOURCE(CLKSOURCE)     (((CLKSOURCE) == RCC_SPDIFRXCLKSource_PLLR) || \
                                                   ((CLKSOURCE) == RCC_SPDIFRXCLKSource_PLLI2SP))
/**
  * @}
  */

/** @defgroup RCC_CEC_Clock_Source_Selection
  * @{
  */
#define RCC_CECCLKSource_HSIDiv488            ((uint8_t)0x00)
#define RCC_CECCLKSource_LSE                  ((uint8_t)0x01)
#define IS_RCC_CEC_CLOCKSOURCE(CLKSOURCE)     (((CLKSOURCE) == RCC_CECCLKSource_HSIDiv488) || \
                                               ((CLKSOURCE) == RCC_CECCLKSource_LSE))
/**
  * @}
  */

/** @defgroup RCC_I2C_Clock_Source
  * @{
  */
#define RCC_I2CCLKSource_APB                   ((uint32_t)0x00)
#define RCC_I2CCLKSource_SYSCLK                ((uint32_t)RCC_DCKCFGR2_I2C1SEL_0)
#define RCC_I2CCLKSource_HSI                   ((uint32_t)RCC_DCKCFGR2_I2C1SEL_1)
    
#define IS_RCC_I2C_CLOCKSOURCE(SOURCE) (((SOURCE) == RCC_I2CCLKSource_APB) || ((SOURCE) == RCC_I2CCLKSource_SYSCLK) || \
                                         ((SOURCE) == RCC_I2CCLKSource_HSI))                                
/**
  * @}
  */


/** @defgroup RCC_USART_UART_Clock_Source
  * @{
  */
#define RCC_USARTCLKSource_APB                  ((uint32_t)0x00)
#define RCC_USARTCLKSource_SYSCLK               ((uint32_t)RCC_DCKCFGR2_USART1SEL_0)
#define RCC_USARTCLKSource_HSI                  ((uint32_t)RCC_DCKCFGR2_USART1SEL_1)
#define RCC_USARTCLKSource_LSE                  ((uint32_t)RCC_DCKCFGR2_USART1SEL)
#define IS_RCC_USART_CLOCKSOURCE(SOURCE) (((SOURCE) == RCC_USARTCLKSource_APB) || ((SOURCE) == RCC_USARTCLKSource_SYSCLK) || \
                                         ((SOURCE) == RCC_USARTCLKSource_HSI)) || ((SOURCE) == RCC_USARTCLKSource_LSE)
/**
  * @}
  */

/** @defgroup RCCEx_LPTIM1_Clock_Source RCCEx LPTIM1 Clock Source
  * @{
  */
#define RCC_LPTIMCLKSource_APB                 ((uint32_t)0x00000000)
#define RCC_LPTIMCLKSource_LSI                 RCC_DCKCFGR2_LPTIM1SEL_0
#define RCC_LPTIMCLKSource_HSI                 RCC_DCKCFGR2_LPTIM1SEL_1
#define RCC_LPTIMCLKSource_LSE                 RCC_DCKCFGR2_LPTIM1SEL

#define IS_RCC_LPTIM_CLOCKSOURCE(SOURCE) (((SOURCE) == RCC_LPTIMCLKSource_APB) || ((SOURCE) == RCC_LPTIMCLKSource_LSI) || \
                                         ((SOURCE) == RCC_LPTIMCLKSource_HSI) || ((SOURCE) == RCC_LPTIMCLKSource_LSE))   
/**
  * @}
  */
    
/** @defgroup RCC_AHB1_Peripherals 
  * @{
  */ 
#define RCC_AHB1Periph_GPIOA             ((uint32_t)RCC_AHB1ENR_GPIOAEN)
#define RCC_AHB1Periph_GPIOB             ((uint32_t)RCC_AHB1ENR_GPIOBEN)
#define RCC_AHB1Periph_GPIOC             ((uint32_t)RCC_AHB1ENR_GPIOCEN)
#define RCC_AHB1Periph_GPIOD             ((uint32_t)RCC_AHB1ENR_GPIODEN)
#define RCC_AHB1Periph_GPIOE             ((uint32_t)RCC_AHB1ENR_GPIOEEN)
#define RCC_AHB1Periph_GPIOF             ((uint32_t)RCC_AHB1ENR_GPIOFEN)
#define RCC_AHB1Periph_GPIOG             ((uint32_t)RCC_AHB1ENR_GPIOGEN)
#define RCC_AHB1Periph_GPIOH             ((uint32_t)RCC_AHB1ENR_GPIOHEN)
#define RCC_AHB1Periph_GPIOI             ((uint32_t)RCC_AHB1ENR_GPIOIEN)
#if defined(STM32F767xx)
#define RCC_AHB1Periph_GPIOJ             ((uint32_t)RCC_AHB1ENR_GPIOJEN)
#define RCC_AHB1Periph_GPIOK             ((uint32_t)RCC_AHB1ENR_GPIOKEN)
#endif
#define RCC_AHB1Periph_CRC               ((uint32_t)RCC_AHB1ENR_CRCEN)
#define RCC_AHB1Periph_BKPSRAM           ((uint32_t)RCC_AHB1ENR_BKPSRAMEN)
#define RCC_AHB1Periph_DTCMRAM           ((uint32_t)RCC_AHB1ENR_DTCMRAMEN)
#define RCC_AHB1Periph_DMA1              ((uint32_t)RCC_AHB1ENR_DMA1EN)
#define RCC_AHB1Periph_DMA2              ((uint32_t)RCC_AHB1ENR_DMA2EN)
#define RCC_AHB1Periph_DMA2D             ((uint32_t)RCC_AHB1ENR_DMA2DEN)
#define RCC_AHB1Periph_ETH_MAC           ((uint32_t)RCC_AHB1ENR_ETHMACEN)
#define RCC_AHB1Periph_ETH_MAC_Tx        ((uint32_t)RCC_AHB1ENR_ETHMACTXEN)
#define RCC_AHB1Periph_ETH_MAC_Rx        ((uint32_t)RCC_AHB1ENR_ETHMACRXEN)
#define RCC_AHB1Periph_ETH_MAC_PTP       ((uint32_t)RCC_AHB1ENR_ETHMACPTPEN)
#define RCC_AHB1Periph_OTG_HS            ((uint32_t)RCC_AHB1ENR_OTGHSEN)
#define RCC_AHB1Periph_OTG_HS_ULPI       ((uint32_t)RCC_AHB1ENR_OTGHSULPIEN)

#define IS_RCC_AHB1_CLOCK_PERIPH(PERIPH) ((((PERIPH) & 0x810BE800) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_AHB1_RESET_PERIPH(PERIPH) ((((PERIPH) & 0xDD1FE800) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_AHB1_LPMODE_PERIPH(PERIPH) ((((PERIPH) & 0x81106800) == 0x00) && ((PERIPH) != 0x00))

/**
  * @}
  */ 

/** @defgroup RCC_AHB2_Peripherals 
  * @{
  */  
#define RCC_AHB2Periph_DCMI              ((uint32_t)RCC_AHB2ENR_DCMIEN)
#define RCC_AHB2Periph_RNG               ((uint32_t)RCC_AHB2ENR_RNGEN)
#define RCC_AHB2Periph_OTG_FS            ((uint32_t)RCC_AHB2ENR_OTGFSEN)

#if defined (STM32F756xx)
#define RCC_AHB2Periph_CRYP              ((uint32_t)RCC_AHB2ENR_CRYPEN)
#define RCC_AHB2Periph_HASH              ((uint32_t)RCC_AHB2ENR_HASHEN)
#endif /* STM32F756xx */

#define IS_RCC_AHB2_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFF0E) == 0x00) && ((PERIPH) != 0x00))
/**
  * @}
  */ 

/** @defgroup RCC_AHB3_Peripherals 
  * @{
  */ 
#define RCC_AHB3Periph_FMC                 ((uint32_t)RCC_AHB3ENR_FMCEN)
#define RCC_AHB3Periph_QSPI                ((uint32_t)RCC_AHB3ENR_QSPIEN)
#define IS_RCC_AHB3_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFFC) == 0x00) && ((PERIPH) != 0x00))
    
/**
  * @}
  */ 

/** @defgroup RCC_APB1_Peripherals 
  * @{
  */ 
#define RCC_APB1Periph_TIM2              ((uint32_t)RCC_APB1ENR_TIM2EN)
#define RCC_APB1Periph_TIM3              ((uint32_t)RCC_APB1ENR_TIM3EN)
#define RCC_APB1Periph_TIM4              ((uint32_t)RCC_APB1ENR_TIM4EN)
#define RCC_APB1Periph_TIM5              ((uint32_t)RCC_APB1ENR_TIM5EN)
#define RCC_APB1Periph_TIM6              ((uint32_t)RCC_APB1ENR_TIM6EN)
#define RCC_APB1Periph_TIM7              ((uint32_t)RCC_APB1ENR_TIM7EN)
#define RCC_APB1Periph_TIM12             ((uint32_t)RCC_APB1ENR_TIM12EN)
#define RCC_APB1Periph_TIM13             ((uint32_t)RCC_APB1ENR_TIM13EN)
#define RCC_APB1Periph_TIM14             ((uint32_t)RCC_APB1ENR_TIM14EN)
#define RCC_APB1Periph_LPTIM1            ((uint32_t)RCC_APB1ENR_LPTIM1EN)
#define RCC_APB1Periph_WWDG              ((uint32_t)RCC_APB1ENR_WWDGEN)
#define RCC_APB1Periph_SPI2              ((uint32_t)RCC_APB1ENR_SPI2EN)
#define RCC_APB1Periph_SPI3              ((uint32_t)RCC_APB1ENR_SPI3EN)
#define RCC_APB1Periph_SPDIFRX           ((uint32_t)RCC_APB1ENR_SPDIFRXEN)
#define RCC_APB1Periph_USART2            ((uint32_t)RCC_APB1ENR_USART2EN)
#define RCC_APB1Periph_USART3            ((uint32_t)RCC_APB1ENR_USART3EN)
#define RCC_APB1Periph_UART4             ((uint32_t)RCC_APB1ENR_UART4EN)
#define RCC_APB1Periph_UART5             ((uint32_t)RCC_APB1ENR_UART5EN)
#define RCC_APB1Periph_I2C1              ((uint32_t)RCC_APB1ENR_I2C1EN)
#define RCC_APB1Periph_I2C2              ((uint32_t)RCC_APB1ENR_I2C2EN)
#define RCC_APB1Periph_I2C3              ((uint32_t)RCC_APB1ENR_I2C3EN)
#define RCC_APB1Periph_I2C4              ((uint32_t)RCC_APB1ENR_I2C4EN)
#define RCC_APB1Periph_CAN1              ((uint32_t)RCC_APB1ENR_CAN1EN)
#define RCC_APB1Periph_CAN2              ((uint32_t)RCC_APB1ENR_CAN2EN)
#define RCC_APB1Periph_CEC               ((uint32_t)RCC_APB1ENR_CECEN)
#define RCC_APB1Periph_PWR               ((uint32_t)RCC_APB1ENR_PWREN)
#define RCC_APB1Periph_DAC               ((uint32_t)RCC_APB1ENR_DACEN)
#define RCC_APB1Periph_UART7             ((uint32_t)RCC_APB1ENR_UART7EN)
#define RCC_APB1Periph_UART8             ((uint32_t)RCC_APB1ENR_UART8EN)
#define IS_RCC_APB1_PERIPH(PERIPH) ((((PERIPH) & 0x00003600) == 0x00) && ((PERIPH) != 0x00))
/**
  * @}
  */ 

/** @defgroup RCC_APB2_Peripherals 
  * @{
  */ 
#define RCC_APB2Periph_TIM1              ((uint32_t)RCC_APB2ENR_TIM1EN)
#define RCC_APB2Periph_TIM8              ((uint32_t)RCC_APB2ENR_TIM8EN)
#define RCC_APB2Periph_USART1            ((uint32_t)RCC_APB2ENR_USART1EN)
#define RCC_APB2Periph_USART6            ((uint32_t)RCC_APB2ENR_USART6EN)
#define RCC_APB2Periph_ADC               ((uint32_t)RCC_APB2ENR_ADC1EN)
#define RCC_APB2Periph_ADC1              ((uint32_t)RCC_APB2ENR_ADC1EN)
#define RCC_APB2Periph_ADC2              ((uint32_t)RCC_APB2ENR_ADC2EN)
#define RCC_APB2Periph_ADC3              ((uint32_t)RCC_APB2ENR_ADC3EN)
#define RCC_APB2Periph_SDMMC1            ((uint32_t)RCC_APB2ENR_SDMMC1EN)
#define RCC_APB2Periph_SPI1              ((uint32_t)RCC_APB2ENR_SPI1EN)
#define RCC_APB2Periph_SPI4              ((uint32_t)RCC_APB2ENR_SPI4EN)
#define RCC_APB2Periph_SYSCFG            ((uint32_t)RCC_APB2ENR_SYSCFGEN)
#define RCC_APB2Periph_TIM9              ((uint32_t)RCC_APB2ENR_TIM9EN)
#define RCC_APB2Periph_TIM10             ((uint32_t)RCC_APB2ENR_TIM10EN)
#define RCC_APB2Periph_TIM11             ((uint32_t)RCC_APB2ENR_TIM11EN)
#define RCC_APB2Periph_SPI5              ((uint32_t)RCC_APB2ENR_SPI5EN)
#define RCC_APB2Periph_SPI6              ((uint32_t)RCC_APB2ENR_SPI6EN)
#define RCC_APB2Periph_SAI1              ((uint32_t)RCC_APB2ENR_SAI1EN)
#define RCC_APB2Periph_SAI2              ((uint32_t)RCC_APB2ENR_SAI2EN)
#define RCC_APB2Periph_LTDC              ((uint32_t)RCC_APB2ENR_LTDCEN)

#define IS_RCC_APB2_PERIPH(PERIPH) ((((PERIPH) & 0xF30880CC) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_APB2_RESET_PERIPH(PERIPH) ((((PERIPH) & 0xF30886CC) == 0x00) && ((PERIPH) != 0x00))

/**
  * @}
  */ 
    
    
    
/** @defgroup RCC_MCO1_Clock_Source_Prescaler
  * @{
  */
#define RCC_MCO1Source_HSI               ((uint32_t)0x00000000)
#define RCC_MCO1Source_LSE               ((uint32_t)RCC_CFGR_MCO1_0)
#define RCC_MCO1Source_HSE               ((uint32_t)RCC_CFGR_MCO1_1)
#define RCC_MCO1Source_PLLCLK            ((uint32_t)RCC_CFGR_MCO1)
#define RCC_MCO1Div_1                    ((uint32_t)0x00000000)
#define RCC_MCO1Div_2                    ((uint32_t)RCC_CFGR_MCO1PRE_2)
#define RCC_MCO1Div_3                    ((uint32_t)RCC_CFGR_MCO1PRE_0 | RCC_CFGR_MCO1PRE_2)
#define RCC_MCO1Div_4                    ((uint32_t)RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_2)
#define RCC_MCO1Div_5                    ((uint32_t)RCC_CFGR_MCO1PRE)
#define IS_RCC_MCO1SOURCE(SOURCE) (((SOURCE) == RCC_MCO1Source_HSI) || ((SOURCE) == RCC_MCO1Source_LSE) || \
                                   ((SOURCE) == RCC_MCO1Source_HSE) || ((SOURCE) == RCC_MCO1Source_PLLCLK))
                                   
#define IS_RCC_MCO1DIV(DIV) (((DIV) == RCC_MCO1Div_1) || ((DIV) == RCC_MCO1Div_2) || \
                             ((DIV) == RCC_MCO1Div_3) || ((DIV) == RCC_MCO1Div_4) || \
                             ((DIV) == RCC_MCO1Div_5)) 
/**
  * @}
  */ 
  
/** @defgroup RCC_MCO2_Clock_Source_Prescaler
  * @{
  */
#define RCC_MCO2Source_SYSCLK            ((uint32_t)0x00000000)
#define RCC_MCO2Source_PLLI2SCLK         ((uint32_t)RCC_CFGR_MCO2_0)
#define RCC_MCO2Source_HSE               ((uint32_t)RCC_CFGR_MCO2_1)
#define RCC_MCO2Source_PLLCLK            ((uint32_t)RCC_CFGR_MCO2)
#define RCC_MCO2Div_1                    ((uint32_t)0x00000000)
#define RCC_MCO2Div_2                    ((uint32_t)RCC_CFGR_MCO2PRE_2)
#define RCC_MCO2Div_3                    ((uint32_t)RCC_CFGR_MCO2PRE_0 | RCC_CFGR_MCO2PRE_2)
#define RCC_MCO2Div_4                    ((uint32_t)RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2)
#define RCC_MCO2Div_5                    ((uint32_t)RCC_CFGR_MCO2PRE)
#define IS_RCC_MCO2SOURCE(SOURCE) (((SOURCE) == RCC_MCO2Source_SYSCLK) || ((SOURCE) == RCC_MCO2Source_PLLI2SCLK)|| \
                                   ((SOURCE) == RCC_MCO2Source_HSE) || ((SOURCE) == RCC_MCO2Source_PLLCLK))
                                   
#define IS_RCC_MCO2DIV(DIV) (((DIV) == RCC_MCO2Div_1) || ((DIV) == RCC_MCO2Div_2) || \
                             ((DIV) == RCC_MCO2Div_3) || ((DIV) == RCC_MCO2Div_4) || \
                             ((DIV) == RCC_MCO2Div_5))                             
/**
  * @}
  */ 

/** @defgroup RCC_Flag RCC Flags
  *        Elements values convention: 0XXYYYYYb
  *           - YYYYY  : Flag position in the register
  *           - 0XX  : Register index
  *                 - 01: CR register
  *                 - 10: BDCR register
  *                 - 11: CSR register
  * @{
  */
/* Flags in the CR register */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_PLLI2SRDY               ((uint8_t)0x3B)
#define RCC_FLAG_PLLSAIRDY               ((uint8_t)0x3D)

/* Flags in the BDCR register */
#define RCC_FLAG_LSERDY                  ((uint8_t)0x41)

/* Flags in the CSR register */
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_BORRST                  ((uint8_t)0x79)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)

#define IS_RCC_FLAG(FLAG) (((FLAG) == RCC_FLAG_HSIRDY)   || ((FLAG) == RCC_FLAG_HSERDY) || \
                           ((FLAG) == RCC_FLAG_PLLRDY)   || ((FLAG) == RCC_FLAG_LSERDY) || \
                           ((FLAG) == RCC_FLAG_LSIRDY)   || ((FLAG) == RCC_FLAG_BORRST) || \
                           ((FLAG) == RCC_FLAG_PINRST)   || ((FLAG) == RCC_FLAG_PORRST) || \
                           ((FLAG) == RCC_FLAG_SFTRST)   || ((FLAG) == RCC_FLAG_IWDGRST)|| \
                           ((FLAG) == RCC_FLAG_WWDGRST)  || ((FLAG) == RCC_FLAG_LPWRRST)|| \
                           ((FLAG) == RCC_FLAG_PLLI2SRDY)|| ((FLAG) == RCC_FLAG_PLLSAIRDY))

#define IS_RCC_CALIBRATION_VALUE(VALUE) ((VALUE) <= 0x1F)

/**
  * @}
  */ 

/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 

/* Function used to set the RCC clock configuration to the default reset state */
void        RCC_DeInit(void);

/* Internal/external clocks, PLL, CSS and MCO configuration functions *********/
void        RCC_HSEConfig(uint8_t RCC_HSE);
ErrorStatus RCC_WaitForHSEStartUp(void);
void        RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue);
void        RCC_HSICmd(FunctionalState NewState);
void        RCC_LSEConfig(uint8_t RCC_LSE);
void        RCC_LSICmd(FunctionalState NewState);

void        RCC_PLLCmd(FunctionalState NewState);
void        RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ);

void        RCC_PLLI2SCmd(FunctionalState NewState);
void        RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SP, uint32_t PLLI2SQ, uint32_t PLLI2SR);

void        RCC_PLLSAICmd(FunctionalState NewState);
void        RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIP, uint32_t PLLSAIQ, uint32_t PLLSAIR);

void        RCC_ClockSecuritySystemCmd(FunctionalState NewState);
void        RCC_MCO1Config(uint32_t RCC_MCO1Source, uint32_t RCC_MCO1Div);
void        RCC_MCO2Config(uint32_t RCC_MCO2Source, uint32_t RCC_MCO2Div);

/* System, AHB and APB busses clocks configuration functions ******************/
void        RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
uint8_t     RCC_GetSYSCLKSource(void);
void        RCC_HCLKConfig(uint32_t RCC_SYSCLK);
void        RCC_PCLK1Config(uint32_t RCC_HCLK);
void        RCC_PCLK2Config(uint32_t RCC_HCLK);
void        RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks);

/* Peripheral clocks configuration functions **********************************/
void        RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
void        RCC_RTCCLKCmd(FunctionalState NewState);
void        RCC_BackupResetCmd(FunctionalState NewState);

void        RCC_SAICLKConfig(uint32_t RCC_SAIInstance, uint32_t RCC_SAICLKSource);
void        RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource);

void        RCC_SAIPLLI2SClkDivConfig(uint32_t RCC_PLLI2SDivQ);
void        RCC_SAIPLLSAIClkDivConfig(uint32_t RCC_PLLSAIDivQ);

void        RCC_LTDCCLKDivConfig(uint32_t RCC_PLLSAIDivR);
void        RCC_TIMCLKPresConfig(uint32_t RCC_TIMCLKPrescaler);

void        RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
void        RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
void        RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
void        RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

void        RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
void        RCC_AHB2PeriphResetCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
void        RCC_AHB3PeriphResetCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
void        RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

void        RCC_AHB1PeriphClockLPModeCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
void        RCC_AHB2PeriphClockLPModeCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
void        RCC_AHB3PeriphClockLPModeCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
void        RCC_APB1PeriphClockLPModeCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_APB2PeriphClockLPModeCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

void        RCC_LSEModeConfig(uint32_t RCC_Mode);

void        RCC_48MHzClockSourceConfig(uint8_t RCC_ClockSource);
void        RCC_SDMMCClockSourceConfig(uint8_t RCC_ClockSource);

void        RCC_CECClockSourceConfig(uint8_t RCC_ClockSource);
void        RCC_I2CClockSourceConfig(I2C_TypeDef * I2Cx, uint32_t RCC_ClockSource);
void        RCC_USARTClockSourceConfig(USART_TypeDef * USARTx, uint32_t RCC_ClockSource);
void        RCC_LPTIMClockSourceConfig(LPTIM_TypeDef * LPTIMx, uint32_t RCC_ClockSource);

/* Interrupts and flags management functions **********************************/
void        RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState);
FlagStatus  RCC_GetFlagStatus(uint8_t RCC_FLAG);
void        RCC_ClearFlag(void);
ITStatus    RCC_GetITStatus(uint8_t RCC_IT);
void        RCC_ClearITPendingBit(uint8_t RCC_IT);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_RCC_H */