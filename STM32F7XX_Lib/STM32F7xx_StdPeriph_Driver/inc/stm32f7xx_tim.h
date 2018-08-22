
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_TIM_H
#define __STM32F7xx_TIM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"

/** @addtogroup STM32F7xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup TIM
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  TIM Time Base Init structure definition  
  * @note   This structure is used with all TIMx except for TIM6 and TIM7.  
  */

typedef struct
{
  uint32_t TIM_Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                       This parameter can be a number between 0x0000 and 0xFFFF */

  uint32_t TIM_CounterMode;       /*!< Specifies the counter mode.
                                       This parameter can be a value of @ref TIM_Counter_Mode */

  uint32_t TIM_Period;            /*!< Specifies the period value to be loaded into the active
                                       Auto-Reload Register at the next update event.
                                       This parameter must be a number between 0x0000 and 0xFFFF.  */ 

  uint32_t TIM_ClockDivision;     /*!< Specifies the clock division.
                                      This parameter can be a value of @ref TIM_Clock_Division_CKD */

  uint32_t TIM_RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
                                       reaches zero, an update event is generated and counting restarts
                                       from the RCR value (N).
                                       This means in PWM mode that (N+1) corresponds to:
                                          - the number of PWM periods in edge-aligned mode
                                          - the number of half PWM period in center-aligned mode
                                       This parameter must be a number between 0x00 and 0xFF. 
                                       @note This parameter is valid only for TIM1 and TIM8. */
} TIM_TimeBaseInitTypeDef; 

/** 
  * @brief  TIM Output Compare Init structure definition  
  */

typedef struct
{
  uint32_t TIM_OCMode;        /*!< Specifies the TIM mode.
                                   This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

  uint32_t TIM_OutputState;   /*!< Specifies the TIM Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_State */

  uint32_t TIM_OutputNState;  /*!< Specifies the TIM complementary Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_State
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint32_t TIM_Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register. 
                                   This parameter can be a number between 0x0000 and 0xFFFF */

  uint32_t TIM_OCPolarity;    /*!< Specifies the output polarity.
                                   This parameter can be a value of @ref TIM_Output_Compare_Polarity */

  uint32_t TIM_OCNPolarity;   /*!< Specifies the complementary output polarity.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint32_t TIM_OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
                                   This parameter can be a value of @ref TIM_Output_Compare_Idle_State
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint32_t TIM_OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
                                   @note This parameter is valid only for TIM1 and TIM8. */
} TIM_OCInitTypeDef;

/** 
  * @brief  TIM Input Capture Init structure definition  
  */

typedef struct
{

  uint32_t TIM_Channel;      /*!< Specifies the TIM channel.
                                  This parameter can be a value of @ref TIM_Channel */

  uint32_t TIM_ICPolarity;   /*!< Specifies the active edge of the input signal.
                                  This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint32_t TIM_ICSelection;  /*!< Specifies the input.
                                  This parameter can be a value of @ref TIM_Input_Capture_Selection */

  uint32_t TIM_ICPrescaler;  /*!< Specifies the Input Capture Prescaler.
                                  This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint32_t TIM_ICFilter;     /*!< Specifies the input capture filter.
                                  This parameter can be a number between 0x0 and 0xF */
} TIM_ICInitTypeDef;

/** 
  * @brief  BDTR structure definition 
  * @note   This structure is used only with TIM1 and TIM8.    
  */

typedef struct
{

  uint32_t TIM_OSSRState;        /*!< Specifies the Off-State selection used in Run mode.
                                      This parameter can be a value of @ref TIM_OSSR_Off_State_Selection_for_Run_mode_state */

  uint32_t TIM_OSSIState;        /*!< Specifies the Off-State used in Idle state.
                                      This parameter can be a value of @ref TIM_OSSI_Off_State_Selection_for_Idle_mode_state */

  uint32_t TIM_LOCKLevel;        /*!< Specifies the LOCK level parameters.
                                      This parameter can be a value of @ref TIM_Lock_level */ 

  uint32_t TIM_DeadTime;         /*!< Specifies the delay time between the switching-off and the
                                      switching-on of the outputs.
                                      This parameter can be a number between 0x00 and 0xFF  */

  uint32_t TIM_Break;            /*!< Specifies whether the TIM Break input is enabled or not. 
                                      This parameter can be a value of @ref TIM_Break_Input_enable_disable */

  uint32_t TIM_BreakPolarity;    /*!< Specifies the TIM Break Input pin polarity.
                                      This parameter can be a value of @ref TIM_Break_Polarity */

  uint32_t TIM_AutomaticOutput;  /*!< Specifies whether the TIM Automatic Output feature is enabled or not. 
                                      This parameter can be a value of @ref TIM_AOE_Bit_Set_Reset */
} TIM_BDTRInitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup TIM_Exported_constants 
  * @{
  */

/* All Timer */
/* ALL: TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8, TIM9, TIM10, TIM11, TIM12, TIM13 and TIM14 */   
#define IS_TIM_ALL_PERIPH(PERIPH)    (IS_TIM_INSTANCE(PERIPH))

/* All Timer except Basic */
/* LIST1: TIM1, TIM2, TIM3, TIM4, TIM5, TIM8, TIM9, TIM10, TIM11, TIM12, TIM13 and TIM14 */                                         
#define IS_TIM_LIST1_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8) || \
                                     ((PERIPH) == TIM9) || \
                                     ((PERIPH) == TIM10) || \
                                     ((PERIPH) == TIM11) || \
                                     ((PERIPH) == TIM12) || \
                                     ((PERIPH) == TIM13) || \
                                     ((PERIPH) == TIM14))
                                     
/* LIST2: TIM1, TIM2, TIM3, TIM4, TIM5, TIM8, TIM9 and TIM12 */
#define IS_TIM_LIST2_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8) || \
                                     ((PERIPH) == TIM9) || \
                                     ((PERIPH) == TIM12))
/* Can count up and down */
/* LIST3: TIM1, TIM2, TIM3, TIM4, TIM5 and TIM8 */
#define IS_TIM_LIST3_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8))
/* All Advance Timer */
/* LIST4: TIM1 and TIM8 */ 
#define IS_TIM_LIST4_PERIPH(PERIPH) (IS_TIM_ADVANCED_INSTANCE(PERIPH))

/* LIST5: TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7 and TIM8 */
#define IS_TIM_LIST5_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM6) || \
                                     ((PERIPH) == TIM7) || \
                                     ((PERIPH) == TIM8))
/* LIST6: TIM2, TIM5 and TIM11 */                               
#define IS_TIM_LIST6_PERIPH(TIMx) IS_TIM_REMAP_INSTANCE(TIMx)

/**
  * @}
  */ 

/** @defgroup TIM_Output_Compare_and_PWM_modes 
  * @{
  */

#define TIM_OCMode_Timing                  ((uint32_t)0x0000)
#define TIM_OCMode_Active                  ((uint32_t)TIM_CCMR1_OC1M_0)
#define TIM_OCMode_Inactive                ((uint32_t)TIM_CCMR1_OC1M_1)
#define TIM_OCMode_Toggle                  ((uint32_t)TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0)
#define TIM_OCMode_PWM1                    ((uint32_t)TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1)
#define TIM_OCMode_PWM2                    ((uint32_t)TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0)

#define TIM_OCMode_Retrigerrable_OPM1      ((uint32_t)TIM_CCMR1_OC1M_3)
#define TIM_OCMode_Retrigerrable_OPM2      ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_0)
#define TIM_OCMode_Combined_PWM1           ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_2)
#define TIM_OCMode_Combined_PWM2           ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_2)
#define TIM_OCMode_Asymmetric_PWM1         ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2)
#define TIM_OCMode_Asymmetric_PWM2         ((uint32_t)TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M)

#define IS_TIM_OC_MODE(MODE) (((MODE) == TIM_OCMode_Timing) || \
                              ((MODE) == TIM_OCMode_Active) || \
                              ((MODE) == TIM_OCMode_Inactive) || \
                              ((MODE) == TIM_OCMode_Toggle)|| \
                              ((MODE) == TIM_OCMode_PWM1) || \
                              ((MODE) == TIM_OCMode_PWM2) || \
                              ((MODE) == TIM_OCMode_Retrigerrable_OPM1) || \
                              ((MODE) == TIM_OCMode_Retrigerrable_OPM2) || \
                              ((MODE) == TIM_OCMode_Combined_PWM1) || \
                              ((MODE) == TIM_OCMode_Combined_PWM2) || \
                              ((MODE) == TIM_OCMode_Asymmetric_PWM1) || \
                              ((MODE) == TIM_OCMode_Asymmetric_PWM2))
                              
#define IS_TIM_OCM(MODE) (((MODE) == TIM_OCMode_Timing) || \
                          ((MODE) == TIM_OCMode_Active) || \
                          ((MODE) == TIM_OCMode_Inactive) || \
                          ((MODE) == TIM_OCMode_Toggle)|| \
                          ((MODE) == TIM_OCMode_PWM1) || \
                          ((MODE) == TIM_OCMode_PWM2) ||	\
                          ((MODE) == TIM_ForcedAction_Active) || \
                          ((MODE) == TIM_ForcedAction_InActive) || \
                          ((MODE) == TIM_OCMode_Retrigerrable_OPM1) || \
                          ((MODE) == TIM_OCMode_Retrigerrable_OPM2) || \
                          ((MODE) == TIM_OCMode_Combined_PWM1) || \
                          ((MODE) == TIM_OCMode_Combined_PWM2) || \
                          ((MODE) == TIM_OCMode_Asymmetric_PWM1) || \
                          ((MODE) == TIM_OCMode_Asymmetric_PWM2))
/**
  * @}
  */


/** @defgroup TIM_One_Pulse_Mode 
  * @{
  */

#define TIM_OPMode_Single                  (TIM_CR1_OPM)
#define TIM_OPMode_Repetitive              ((uint32_t)0x0000)
#define IS_TIM_OPM_MODE(MODE) (((MODE) == TIM_OPMode_Single) || \
                               ((MODE) == TIM_OPMode_Repetitive))
/**
  * @}
  */ 

/** @defgroup TIM_Channel 
  * @{
  */

#define TIM_Channel_1                      ((uint32_t)0x0000)
#define TIM_Channel_2                      ((uint32_t)0x0004)
#define TIM_Channel_3                      ((uint32_t)0x0008)
#define TIM_Channel_4                      ((uint32_t)0x000C)
#define TIM_Channel_5                      ((uint32_t)0x0010)
#define TIM_Channel_6                      ((uint32_t)0x0014)
                                 
#define IS_TIM_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                 ((CHANNEL) == TIM_Channel_2) || \
                                 ((CHANNEL) == TIM_Channel_3) || \
                                 ((CHANNEL) == TIM_Channel_4))
                                 
#define IS_TIM_PWMI_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                      ((CHANNEL) == TIM_Channel_2))
#define IS_TIM_COMPLEMENTARY_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                               ((CHANNEL) == TIM_Channel_2) || \
                                               ((CHANNEL) == TIM_Channel_3))
/**
  * @}
  */

/** @defgroup TIM_Clock_Division_CKD 
  * @{
  */

#define TIM_CKD_DIV1                       ((uint32_t)0x0000)
#define TIM_CKD_DIV2                       (TIM_CR1_CKD_0)
#define TIM_CKD_DIV4                       (TIM_CR1_CKD_1)
#define IS_TIM_CKD_DIV(DIV) (((DIV) == TIM_CKD_DIV1) || \
                             ((DIV) == TIM_CKD_DIV2) || \
                             ((DIV) == TIM_CKD_DIV4))
/**
  * @}
  */

/** @defgroup TIM_Counter_Mode 
  * @{
  */

#define TIM_CounterMode_Up                 ((uint32_t)0x0000)
#define TIM_CounterMode_Down               (TIM_CR1_DIR)
#define TIM_CounterMode_CenterAligned1     (TIM_CR1_CMS_0)
#define TIM_CounterMode_CenterAligned2     (TIM_CR1_CMS_1)
#define TIM_CounterMode_CenterAligned3     (TIM_CR1_CMS)
#define IS_TIM_COUNTER_MODE(MODE) (((MODE) == TIM_CounterMode_Up) ||  \
                                   ((MODE) == TIM_CounterMode_Down) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned1) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned2) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned3))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Polarity 
  * @{
  */

#define TIM_OCPolarity_High                ((uint32_t)0x0000)
#define TIM_OCPolarity_Low                 (TIM_CCER_CC1P)
#define IS_TIM_OC_POLARITY(POLARITY) (((POLARITY) == TIM_OCPolarity_High) || \
                                      ((POLARITY) == TIM_OCPolarity_Low))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Polarity 
  * @{
  */
  
#define TIM_OCNPolarity_High               ((uint32_t)0x0000)
#define TIM_OCNPolarity_Low                (TIM_CCER_CC1NP)
#define IS_TIM_OCN_POLARITY(POLARITY) (((POLARITY) == TIM_OCNPolarity_High) || \
                                       ((POLARITY) == TIM_OCNPolarity_Low))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_State 
  * @{
  */

#define TIM_OutputState_Disable            ((uint32_t)0x0000)
#define TIM_OutputState_Enable             (TIM_CCER_CC1E)
#define IS_TIM_OUTPUT_STATE(STATE) (((STATE) == TIM_OutputState_Disable) || \
                                    ((STATE) == TIM_OutputState_Enable))
/**
  * @}
  */ 


/** @defgroup TIM_Output_Compare_N_State
  * @{
  */

#define TIM_OutputNState_Disable           ((uint32_t)0x0000)
#define TIM_OutputNState_Enable            (TIM_CCER_CC1NE)
#define IS_TIM_OUTPUTN_STATE(STATE) (((STATE) == TIM_OutputNState_Disable) || \
                                     ((STATE) == TIM_OutputNState_Enable))
/**
  * @}
  */ 

/** @defgroup TIM_Capture_Compare_State
  * @{
  */

#define TIM_CCx_Enable                      ((uint32_t)0x0001)
#define TIM_CCx_Disable                     ((uint32_t)0x0000)
#define IS_TIM_CCX(CCX) (((CCX) == TIM_CCx_Enable) || \
                         ((CCX) == TIM_CCx_Disable))
/**
  * @}
  */ 

/** @defgroup TIM_Capture_Compare_N_State
  * @{
  */

#define TIM_CCxN_Enable                     ((uint32_t)0x0004)
#define TIM_CCxN_Disable                    ((uint32_t)0x0000)
#define IS_TIM_CCXN(CCXN) (((CCXN) == TIM_CCxN_Enable) || \
                           ((CCXN) == TIM_CCxN_Disable))
/**
  * @}
  */ 

/** @defgroup TIM_Break_Input_enable_disable 
  * @{
  */

#define TIM_Break_Enable                   (TIM_BDTR_BKE)
#define TIM_Break_Disable                  ((uint32_t)0x0000)
#define IS_TIM_BREAK_STATE(STATE) (((STATE) == TIM_Break_Enable) || \
                                   ((STATE) == TIM_Break_Disable))
/**
  * @}
  */ 

/** @defgroup TIM_Break1_Input_enable_disable 
  * @{
  */

#define TIM_Break1_Enable                   (TIM_BDTR_BKE)
#define TIM_Break1_Disable                  ((uint32_t)0x00000000)
#define IS_TIM_BREAK1_STATE(STATE) (((STATE) == TIM_Break1_Enable) || \
                                   ((STATE) == TIM_Break1_Disable))
/**
  * @}
  */

/** @defgroup TIM_Break2_Input_enable_disable 
  * @{
  */

#define TIM_Break2_Enable                   (TIM_BDTR_BK2E)
#define TIM_Break2_Disable                  ((uint32_t)0x00000000)
#define IS_TIM_BREAK2_STATE(STATE) (((STATE) == TIM_Break2_Enable) || \
                                   ((STATE) == TIM_Break2_Disable))
/**
  * @}
  */

/** @defgroup TIM_Break_Polarity 
  * @{
  */

#define TIM_BreakPolarity_Low              ((uint32_t)0x0000)
#define TIM_BreakPolarity_High             (TIM_BDTR_BKP)
#define IS_TIM_BREAK_POLARITY(POLARITY) (((POLARITY) == TIM_BreakPolarity_Low) || \
                                         ((POLARITY) == TIM_BreakPolarity_High))
/**
  * @}
  */ 

/** @defgroup TIM_Break1_Polarity 
  * @{
  */

#define TIM_Break1Polarity_Low              ((uint32_t)0x00000000)
#define TIM_Break1Polarity_High             (TIM_BDTR_BKP)
#define IS_TIM_BREAK1_POLARITY(POLARITY) (((POLARITY) == TIM_Break1Polarity_Low) || \
                                         ((POLARITY) == TIM_Break1Polarity_High))
/**
  * @}
  */ 

/** @defgroup TIM_Break2_Polarity 
  * @{
  */

#define TIM_Break2Polarity_Low              ((uint32_t)0x00000000)
#define TIM_Break2Polarity_High             (TIM_BDTR_BK2P)
#define IS_TIM_BREAK2_POLARITY(POLARITY) (((POLARITY) == TIM_Break2Polarity_Low) || \
                                         ((POLARITY) == TIM_Break2Polarity_High))
/**
  * @}
  */ 

/** @defgroup TIM_Break1_Filter 
  * @{
  */

#define IS_TIM_BREAK1_FILTER(FILTER) ((FILTER) <= 0xF)
/**
  * @}
  */ 

/** @defgroup TIM_Break2_Filter 
  * @{
  */

#define IS_TIM_BREAK2_FILTER(FILTER) ((FILTER) <= 0xF)
/**
  * @}
  */ 

/** @defgroup TIM_AOE_Bit_Set_Reset 
  * @{
  */

#define TIM_AutomaticOutput_Enable         (TIM_BDTR_AOE)
#define TIM_AutomaticOutput_Disable        ((uint32_t)0x0000)
#define IS_TIM_AUTOMATIC_OUTPUT_STATE(STATE) (((STATE) == TIM_AutomaticOutput_Enable) || \
                                              ((STATE) == TIM_AutomaticOutput_Disable))
/**
  * @}
  */ 

/** @defgroup TIM_Lock_level
  * @{
  */

#define TIM_LOCKLevel_OFF                  ((uint32_t)0x0000)
#define TIM_LOCKLevel_1                    (TIM_BDTR_LOCK_0)
#define TIM_LOCKLevel_2                    (TIM_BDTR_LOCK_1)
#define TIM_LOCKLevel_3                    (TIM_BDTR_LOCK)
#define IS_TIM_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM_LOCKLevel_OFF) || \
                                  ((LEVEL) == TIM_LOCKLevel_1) || \
                                  ((LEVEL) == TIM_LOCKLevel_2) || \
                                  ((LEVEL) == TIM_LOCKLevel_3))
/**
  * @}
  */ 

/** @defgroup TIM_OSSI_Off_State_Selection_for_Idle_mode_state 
  * @{
  */

#define TIM_OSSIState_Enable               (TIM_BDTR_OSSI)
#define TIM_OSSIState_Disable              ((uint32_t)0x0000)
#define IS_TIM_OSSI_STATE(STATE) (((STATE) == TIM_OSSIState_Enable) || \
                                  ((STATE) == TIM_OSSIState_Disable))
/**
  * @}
  */

/** @defgroup TIM_OSSR_Off_State_Selection_for_Run_mode_state
  * @{
  */

#define TIM_OSSRState_Enable               (TIM_BDTR_OSSR)
#define TIM_OSSRState_Disable              ((uint32_t)0x0000)
#define IS_TIM_OSSR_STATE(STATE) (((STATE) == TIM_OSSRState_Enable) || \
                                  ((STATE) == TIM_OSSRState_Disable))
/**
  * @}
  */ 

/** @defgroup TIM_Output_Compare_Idle_State 
  * @{
  */

#define TIM_OCIdleState_Set                (TIM_CR2_OIS1)
#define TIM_OCIdleState_Reset              ((uint32_t)0x0000)
#define IS_TIM_OCIDLE_STATE(STATE) (((STATE) == TIM_OCIdleState_Set) || \
                                    ((STATE) == TIM_OCIdleState_Reset))
/**
  * @}
  */ 

/** @defgroup TIM_Output_Compare_N_Idle_State 
  * @{
  */

#define TIM_OCNIdleState_Set               (TIM_CR2_OIS1N)
#define TIM_OCNIdleState_Reset             ((uint32_t)0x0000)
#define IS_TIM_OCNIDLE_STATE(STATE) (((STATE) == TIM_OCNIdleState_Set) || \
                                     ((STATE) == TIM_OCNIdleState_Reset))
/**
  * @}
  */ 
    
/** @defgroup TIM_Input_Capture_Polarity 
  * @{
  */

#define  TIM_ICPolarity_Rising             ((uint32_t)0x00000000)
#define  TIM_ICPolarity_Falling            (TIM_CCER_CC1P) 
#define  TIM_ICPolarity_BothEdge           (TIM_CCER_CC1P | TIM_CCER_CC1NP)
#define IS_TIM_IC_POLARITY(POLARITY) (((POLARITY) == TIM_ICPolarity_Rising) || \
                                      ((POLARITY) == TIM_ICPolarity_Falling)|| \
                                      ((POLARITY) == TIM_ICPolarity_BothEdge))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Selection 
  * @{
  */

#define TIM_ICSelection_DirectTI           (TIM_CCMR1_CC1S_0) /*!< TIM Input 1, 2, 3 or 4 is selected to be 
                                                                   connected to IC1, IC2, IC3 or IC4, respectively */
#define TIM_ICSelection_IndirectTI         (TIM_CCMR1_CC1S_1) /*!< TIM Input 1, 2, 3 or 4 is selected to be
                                                                   connected to IC2, IC1, IC4 or IC3, respectively. */
#define TIM_ICSelection_TRC                (TIM_CCMR1_CC1S)   /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to TRC. */
#define IS_TIM_IC_SELECTION(SELECTION) (((SELECTION) == TIM_ICSelection_DirectTI) || \
                                        ((SELECTION) == TIM_ICSelection_IndirectTI) || \
                                        ((SELECTION) == TIM_ICSelection_TRC))
/**
  * @}
  */ 

/** @defgroup TIM_Input_Capture_Prescaler 
  * @{
  */

#define TIM_ICPSC_DIV1                     ((uint32_t)0x0000) /*!< Capture performed each time an edge is detected on the capture input. */
#define TIM_ICPSC_DIV2                     (TIM_CCMR1_IC1PSC_0) /*!< Capture performed once every 2 events. */
#define TIM_ICPSC_DIV4                     (TIM_CCMR1_IC1PSC_1) /*!< Capture performed once every 4 events. */
#define TIM_ICPSC_DIV8                     (TIM_CCMR1_IC1PSC) /*!< Capture performed once every 8 events. */
#define IS_TIM_IC_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ICPSC_DIV1) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV2) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV4) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV8))
/**
  * @}
  */ 

/** @defgroup TIM_interrupt_sources 
  * @{
  */

#define TIM_IT_Update                      (TIM_DIER_UIE)
#define TIM_IT_CC1                         (TIM_DIER_CC1IE)
#define TIM_IT_CC2                         (TIM_DIER_CC2IE)
#define TIM_IT_CC3                         (TIM_DIER_CC3IE)
#define TIM_IT_CC4                         (TIM_DIER_CC4IE)
#define TIM_IT_COM                         (TIM_DIER_COMIE)
#define TIM_IT_Trigger                     (TIM_DIER_TIE)
#define TIM_IT_Break                       (TIM_DIER_BIE)
#define IS_TIM_IT(__IT__) ((((__IT__) & 0xFFFFFF00) == 0x00000000) && ((__IT__) != 0x00000000))

#define IS_TIM_GET_IT(IT) (((IT) == TIM_IT_Update) || \
                           ((IT) == TIM_IT_CC1) || \
                           ((IT) == TIM_IT_CC2) || \
                           ((IT) == TIM_IT_CC3) || \
                           ((IT) == TIM_IT_CC4) || \
                           ((IT) == TIM_IT_COM) || \
                           ((IT) == TIM_IT_Trigger) || \
                           ((IT) == TIM_IT_Break))
/**
  * @}
  */ 

/** @defgroup TIM_DMA_Base_address 
  * @{
  */

#define TIM_DMABase_CR1                    (0x00000000)
#define TIM_DMABase_CR2                    (TIM_DCR_DBA_0)
#define TIM_DMABase_SMCR                   (TIM_DCR_DBA_1)
#define TIM_DMABase_DIER                   (TIM_DCR_DBA_1 | TIM_DCR_DBA_0)
#define TIM_DMABase_SR                     (TIM_DCR_DBA_2)
#define TIM_DMABase_EGR                    (TIM_DCR_DBA_2 | TIM_DCR_DBA_0)
#define TIM_DMABase_CCMR1                  (TIM_DCR_DBA_2 | TIM_DCR_DBA_1)
#define TIM_DMABase_CCMR2                  (TIM_DCR_DBA_2 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)
#define TIM_DMABase_CCER                   (TIM_DCR_DBA_3)
#define TIM_DMABase_CNT                    (TIM_DCR_DBA_3 | TIM_DCR_DBA_0)
#define TIM_DMABase_PSC                    (TIM_DCR_DBA_3 | TIM_DCR_DBA_1)
#define TIM_DMABase_ARR                    (TIM_DCR_DBA_3 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)
#define TIM_DMABase_RCR                    (TIM_DCR_DBA_3 | TIM_DCR_DBA_2)
#define TIM_DMABase_CCR1                   (TIM_DCR_DBA_3 | TIM_DCR_DBA_2 | TIM_DCR_DBA_0)
#define TIM_DMABase_CCR2                   (TIM_DCR_DBA_3 | TIM_DCR_DBA_2 | TIM_DCR_DBA_1)
#define TIM_DMABase_CCR3                   (TIM_DCR_DBA_3 | TIM_DCR_DBA_2 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)
#define TIM_DMABase_CCR4                   (TIM_DCR_DBA_4)
#define TIM_DMABase_BDTR                   (TIM_DCR_DBA_4 | TIM_DCR_DBA_0)
#define TIM_DMABase_DCR                    (TIM_DCR_DBA_4 | TIM_DCR_DBA_1)
#define TIM_DMABase_OR                     (TIM_DCR_DBA_4 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)
#define TIM_DMABase_CCMR3                  (TIM_DCR_DBA_4 | TIM_DCR_DBA_2)
#define TIM_DMABase_CCR5                   (TIM_DCR_DBA_4 | TIM_DCR_DBA_2 | TIM_DCR_DBA_0)
#define TIM_DMABase_CCR6                   (TIM_DCR_DBA_4 | TIM_DCR_DBA_2 | TIM_DCR_DBA_1)
#define IS_TIM_DMA_BASE(BASE) (((BASE) == TIM_DMABase_CR1) || \
                               ((BASE) == TIM_DMABase_CR2) || \
                               ((BASE) == TIM_DMABase_SMCR) || \
                               ((BASE) == TIM_DMABase_DIER) || \
                               ((BASE) == TIM_DMABase_SR) || \
                               ((BASE) == TIM_DMABase_EGR) || \
                               ((BASE) == TIM_DMABase_CCMR1) || \
                               ((BASE) == TIM_DMABase_CCMR2) || \
                               ((BASE) == TIM_DMABase_CCER) || \
                               ((BASE) == TIM_DMABase_CNT) || \
                               ((BASE) == TIM_DMABase_PSC) || \
                               ((BASE) == TIM_DMABase_ARR) || \
                               ((BASE) == TIM_DMABase_RCR) || \
                               ((BASE) == TIM_DMABase_CCR1) || \
                               ((BASE) == TIM_DMABase_CCR2) || \
                               ((BASE) == TIM_DMABase_CCR3) || \
                               ((BASE) == TIM_DMABase_CCR4) || \
                               ((BASE) == TIM_DMABase_BDTR) || \
                               ((BASE) == TIM_DMABase_DCR) || \
                               ((BASE) == TIM_DMABase_OR) || \
                               ((BASE) == TIM_DMABase_CCMR3) || \
                               ((BASE) == TIM_DMABase_CCR5) || \
                               ((BASE) == TIM_DMABase_CCR6))                     
/**
  * @}
  */ 

/** @defgroup TIM_DMA_Burst_Length 
  * @{
  */

#define TIM_DMABurstLength_1Transfer           (0x00000000)
#define TIM_DMABurstLength_2Transfers          (TIM_DCR_DBL_0)
#define TIM_DMABurstLength_3Transfers          (TIM_DCR_DBL_1)
#define TIM_DMABurstLength_4Transfers          (TIM_DCR_DBL_1 | TIM_DCR_DBL_0)
#define TIM_DMABurstLength_5Transfers          (TIM_DCR_DBL_2)
#define TIM_DMABurstLength_6Transfers          (TIM_DCR_DBL_2 | TIM_DCR_DBL_0)
#define TIM_DMABurstLength_7Transfers          (TIM_DCR_DBL_2 | TIM_DCR_DBL_1)
#define TIM_DMABurstLength_8Transfers          (TIM_DCR_DBL_2 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0)
#define TIM_DMABurstLength_9Transfers          (TIM_DCR_DBL_3)
#define TIM_DMABurstLength_10Transfers         (TIM_DCR_DBL_3 | TIM_DCR_DBL_0)
#define TIM_DMABurstLength_11Transfers         (TIM_DCR_DBL_3 | TIM_DCR_DBL_1)
#define TIM_DMABurstLength_12Transfers         (TIM_DCR_DBL_3 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0)
#define TIM_DMABurstLength_13Transfers         (TIM_DCR_DBL_3 | TIM_DCR_DBL_2)
#define TIM_DMABurstLength_14Transfers         (TIM_DCR_DBL_3 | TIM_DCR_DBL_2 | TIM_DCR_DBL_0)
#define TIM_DMABurstLength_15Transfers         (TIM_DCR_DBL_3 | TIM_DCR_DBL_2 | TIM_DCR_DBL_1)
#define TIM_DMABurstLength_16Transfers         (TIM_DCR_DBL_3 | TIM_DCR_DBL_2 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0)
#define TIM_DMABurstLength_17Transfers         (TIM_DCR_DBL_4)
#define TIM_DMABurstLength_18Transfers         (TIM_DCR_DBL_4 | TIM_DCR_DBL_0)
#define IS_TIM_DMA_LENGTH(LENGTH) (((LENGTH) == TIM_DMABurstLength_1Transfer) || \
                                   ((LENGTH) == TIM_DMABurstLength_2Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_3Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_4Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_5Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_6Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_7Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_8Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_9Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_10Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_11Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_12Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_13Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_14Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_15Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_16Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_17Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_18Transfers))
/**
  * @}
  */ 

/** @defgroup TIM_DMA_sources 
  * @{
  */

#define TIM_DMA_Update                     (TIM_DIER_UDE)
#define TIM_DMA_CC1                        (TIM_DIER_CC1DE)
#define TIM_DMA_CC2                        (TIM_DIER_CC2DE)
#define TIM_DMA_CC3                        (TIM_DIER_CC3DE)
#define TIM_DMA_CC4                        (TIM_DIER_CC4DE)
#define TIM_DMA_COM                        (TIM_DIER_COMDE)
#define TIM_DMA_Trigger                    (TIM_DIER_TDE)
#define IS_TIM_DMA_SOURCE(SOURCE) ((((SOURCE) & 0xFFFF80FF) == 0x00000000) && ((SOURCE) != 0x00000000))
/**
  * @}
  */ 

/** @defgroup TIM_External_Trigger_Prescaler 
  * @{
  */

#define TIM_ExtTRGPSC_OFF                  ((uint32_t)0x0000)
#define TIM_ExtTRGPSC_DIV2                 (TIM_SMCR_ETPS_0)
#define TIM_ExtTRGPSC_DIV4                 (TIM_SMCR_ETPS_1)
#define TIM_ExtTRGPSC_DIV8                 (TIM_SMCR_ETPS)
#define IS_TIM_EXT_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ExtTRGPSC_OFF) || \
                                         ((PRESCALER) == TIM_ExtTRGPSC_DIV2) || \
                                         ((PRESCALER) == TIM_ExtTRGPSC_DIV4) || \
                                         ((PRESCALER) == TIM_ExtTRGPSC_DIV8))
/**
  * @}
  */ 

/** @defgroup TIM_Internal_Trigger_Selection 
  * @{
  */

#define TIM_TS_ITR0                        ((uint32_t)0x0000)
#define TIM_TS_ITR1                        (TIM_SMCR_TS_0)
#define TIM_TS_ITR2                        (TIM_SMCR_TS_1)
#define TIM_TS_ITR3                        (TIM_SMCR_TS_1 | TIM_SMCR_TS_0)
#define TIM_TS_TI1F_ED                     (TIM_SMCR_TS_2)
#define TIM_TS_TI1FP1                      (TIM_SMCR_TS_2 | TIM_SMCR_TS_0)
#define TIM_TS_TI2FP2                      (TIM_SMCR_TS_2 | TIM_SMCR_TS_1)
#define TIM_TS_ETRF                        (TIM_SMCR_TS_2 | TIM_SMCR_TS_1 | TIM_SMCR_TS_0)
#define IS_TIM_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                             ((SELECTION) == TIM_TS_ITR1) || \
                                             ((SELECTION) == TIM_TS_ITR2) || \
                                             ((SELECTION) == TIM_TS_ITR3) || \
                                             ((SELECTION) == TIM_TS_TI1F_ED) || \
                                             ((SELECTION) == TIM_TS_TI1FP1) || \
                                             ((SELECTION) == TIM_TS_TI2FP2) || \
                                             ((SELECTION) == TIM_TS_ETRF))
#define IS_TIM_INTERNAL_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                                      ((SELECTION) == TIM_TS_ITR1) || \
                                                      ((SELECTION) == TIM_TS_ITR2) || \
                                                      ((SELECTION) == TIM_TS_ITR3))
/**
  * @}
  */ 

/** @defgroup TIM_TIx_External_Clock_Source 
  * @{
  */

#define TIM_TIxExternalCLK1Source_TI1      (TIM_SMCR_TS_2 | TIM_SMCR_TS_0)
#define TIM_TIxExternalCLK1Source_TI2      (TIM_SMCR_TS_2 | TIM_SMCR_TS_1)
#define TIM_TIxExternalCLK1Source_TI1ED    (TIM_SMCR_TS_2)

/**
  * @}
  */ 

/** @defgroup TIM_External_Trigger_Polarity 
  * @{
  */ 
#define TIM_ExtTRGPolarity_Inverted        (TIM_SMCR_ETP)
#define TIM_ExtTRGPolarity_NonInverted     ((uint32_t)0x0000)
#define IS_TIM_EXT_POLARITY(POLARITY) (((POLARITY) == TIM_ExtTRGPolarity_Inverted) || \
                                       ((POLARITY) == TIM_ExtTRGPolarity_NonInverted))
/**
  * @}
  */










/** @defgroup TIM_Prescaler_Reload_Mode 
  * @{
  */

#define TIM_PSCReloadMode_Update           ((uint32_t)0x0000)
#define TIM_PSCReloadMode_Immediate        (TIM_EGR_UG)
#define IS_TIM_PRESCALER_RELOAD(RELOAD) (((RELOAD) == TIM_PSCReloadMode_Update) || \
                                         ((RELOAD) == TIM_PSCReloadMode_Immediate))
/**
  * @}
  */ 

/** @defgroup TIM_Forced_Action 
  * @{
  */

#define TIM_ForcedAction_Active            ((uint32_t)TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0)
#define TIM_ForcedAction_InActive          ((uint32_t)TIM_CCMR1_OC1M_2)
#define IS_TIM_FORCED_ACTION(ACTION) (((ACTION) == TIM_ForcedAction_Active) || \
                                      ((ACTION) == TIM_ForcedAction_InActive))
/**
  * @}
  */ 

/** @defgroup TIM_Encoder_Mode 
  * @{
  */

#define TIM_EncoderMode_TI1                (TIM_SMCR_SMS_0)
#define TIM_EncoderMode_TI2                (TIM_SMCR_SMS_1)
#define TIM_EncoderMode_TI12               (TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)
#define IS_TIM_ENCODER_MODE(MODE) (((MODE) == TIM_EncoderMode_TI1) || \
                                   ((MODE) == TIM_EncoderMode_TI2) || \
                                   ((MODE) == TIM_EncoderMode_TI12))
/**
  * @}
  */ 

/** @defgroup TIM_Event_Source 
  * @{
  */

#define TIM_EventSource_Update             (TIM_EGR_UG)
#define TIM_EventSource_CC1                (TIM_EGR_CC1G)
#define TIM_EventSource_CC2                (TIM_EGR_CC2G)
#define TIM_EventSource_CC3                (TIM_EGR_CC3G)
#define TIM_EventSource_CC4                (TIM_EGR_CC4G)
#define TIM_EventSource_COM                (TIM_EGR_COMG)
#define TIM_EventSource_Trigger            (TIM_EGR_TG)
#define TIM_EventSource_Break              (TIM_EGR_BG)
#define TIM_EventSource_Break2             (TIM_EGR_B2G)
#define IS_TIM_EVENT_SOURCE(SOURCE) ((((SOURCE) & 0xFFFFFE00) == 0x00000000) && ((SOURCE) != 0x00000000))
  
/**
  * @}
  */ 

/** @defgroup TIM_Update_Source 
  * @{
  */

#define TIM_UpdateSource_Global            ((uint16_t)0x0000) /*!< Source of update is the counter overflow/underflow
                                                                   or the setting of UG bit, or an update generation
                                                                   through the slave mode controller. */
#define TIM_UpdateSource_Regular           ((uint16_t)0x0001) /*!< Source of update is counter overflow/underflow. */
#define IS_TIM_UPDATE_SOURCE(SOURCE) (((SOURCE) == TIM_UpdateSource_Global) || \
                                      ((SOURCE) == TIM_UpdateSource_Regular))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Preload_State 
  * @{
  */

#define TIM_OCPreload_Enable               ((uint16_t)0x0008)
#define TIM_OCPreload_Disable              ((uint16_t)0x0000)
#define IS_TIM_OCPRELOAD_STATE(STATE) (((STATE) == TIM_OCPreload_Enable) || \
                                       ((STATE) == TIM_OCPreload_Disable))
/**
  * @}
  */ 

/** @defgroup TIM_Output_Compare_Fast_State 
  * @{
  */

#define TIM_OCFast_Enable                  ((uint16_t)0x0004)
#define TIM_OCFast_Disable                 ((uint16_t)0x0000)
#define IS_TIM_OCFAST_STATE(STATE) (((STATE) == TIM_OCFast_Enable) || \
                                    ((STATE) == TIM_OCFast_Disable))
                                     
/**
  * @}
  */ 

/** @defgroup TIM_Output_Compare_Clear_State 
  * @{
  */

#define TIM_OCClear_Enable                 ((uint16_t)0x0080)
#define TIM_OCClear_Disable                ((uint16_t)0x0000)
#define IS_TIM_OCCLEAR_STATE(STATE) (((STATE) == TIM_OCClear_Enable) || \
                                     ((STATE) == TIM_OCClear_Disable))
/**
  * @}
  */ 

/** @defgroup TIM_Trigger_Output_Source 
  * @{
  */

#define TIM_TRGOSource_Reset               ((uint32_t)0x0000)
#define TIM_TRGOSource_Enable              (TIM_CR2_MMS_0)
#define TIM_TRGOSource_Update              (TIM_CR2_MMS_1)
#define TIM_TRGOSource_OC1                 ((TIM_CR2_MMS_1 | TIM_CR2_MMS_0)) 
#define TIM_TRGOSource_OC1Ref              (TIM_CR2_MMS_2)
#define TIM_TRGOSource_OC2Ref              ((TIM_CR2_MMS_2 | TIM_CR2_MMS_0))
#define TIM_TRGOSource_OC3Ref              ((TIM_CR2_MMS_2 | TIM_CR2_MMS_1))
#define TIM_TRGOSource_OC4Ref              ((TIM_CR2_MMS_2 | TIM_CR2_MMS_1 | TIM_CR2_MMS_0))
#define IS_TIM_TRGO_SOURCE(SOURCE) (((SOURCE) == TIM_TRGOSource_Reset) || \
                                    ((SOURCE) == TIM_TRGOSource_Enable) || \
                                    ((SOURCE) == TIM_TRGOSource_Update) || \
                                    ((SOURCE) == TIM_TRGOSource_OC1) || \
                                    ((SOURCE) == TIM_TRGOSource_OC1Ref) || \
                                    ((SOURCE) == TIM_TRGOSource_OC2Ref) || \
                                    ((SOURCE) == TIM_TRGOSource_OC3Ref) || \
                                    ((SOURCE) == TIM_TRGOSource_OC4Ref))


#define TIM_TRGO2Source_Reset                             ((uint32_t)0x00000000)
#define TIM_TRGO2Source_Enable                            ((uint32_t)(TIM_CR2_MMS2_0))
#define TIM_TRGO2Source_Update                            ((uint32_t)(TIM_CR2_MMS2_1))
#define TIM_TRGO2Source_OC1                               ((uint32_t)(TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
#define TIM_TRGO2Source_OC1Ref                            ((uint32_t)(TIM_CR2_MMS2_2))
#define TIM_TRGO2Source_OC2Ref                            ((uint32_t)(TIM_CR2_MMS2_2 | TIM_CR2_MMS2_0))
#define TIM_TRGO2Source_OC3Ref                            ((uint32_t)(TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1))
#define TIM_TRGO2Source_OC4Ref                            ((uint32_t)(TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
#define TIM_TRGO2Source_OC5Ref                            ((uint32_t)(TIM_CR2_MMS2_3))
#define TIM_TRGO2Source_OC6Ref                            ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_0))
#define TIM_TRGO2Source_OC4Ref_RisingFalling              ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_1))
#define TIM_TRGO2Source_OC6Ref_RisingFalling              ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
#define TIM_TRGO2Source_OC4RefRising_OC6RefRising         ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2))
#define TIM_TRGO2Source_OC4RefRising_OC6RefFalling        ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 | TIM_CR2_MMS2_0))
#define TIM_TRGO2Source_OC5RefRising_OC6RefRising         ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 |TIM_CR2_MMS2_1))
#define TIM_TRGO2Source_OC5RefRising_OC6RefFalling        ((uint32_t)(TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0))
#define IS_TIM_TRGO2_SOURCE(SOURCE) (((SOURCE) == TIM_TRGO2Source_Reset) || \
                                     ((SOURCE) == TIM_TRGO2Source_Enable) || \
                                     ((SOURCE) == TIM_TRGO2Source_Update) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC1) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC1Ref) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC2Ref) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC3Ref) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC4Ref) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC5Ref) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC6Ref) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC4Ref_RisingFalling) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC6Ref_RisingFalling) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC4RefRising_OC6RefRising) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC4RefRising_OC6RefFalling) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC5RefRising_OC6RefRising) || \
                                     ((SOURCE) == TIM_TRGO2Source_OC5RefRising_OC6RefFalling))
/**
  * @}
  */ 

/** @defgroup TIM_Slave_Mode 
  * @{
  */
#define TIM_SlaveMode_Disable                     ((uint32_t)0x0000)
#define TIM_SlaveMode_Reset                       (TIM_SMCR_SMS_2)
#define TIM_SlaveMode_Gated                       (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_0)
#define TIM_SlaveMode_Trigger                     (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1)
#define TIM_SlaveMode_External1                   (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)
#define TIM_SlaveMode_Combined_ResetTrigger       (TIM_SMCR_SMS_3)
#define IS_TIM_SLAVE_MODE(MODE) (((MODE) == TIM_SlaveMode_Reset) || \
                                 ((MODE) == TIM_SlaveMode_Gated) || \
                                 ((MODE) == TIM_SlaveMode_Trigger) || \
                                 ((MODE) == TIM_SlaveMode_External1) || \
                                 ((MODE) == TIM_SlaveMode_Combined_ResetTrigger))
/**
  * @}
  */ 

/** @defgroup TIM_Master_Slave_Mode 
  * @{
  */

#define TIM_MasterSlaveMode_Enable         (TIM_SMCR_MSM)
#define TIM_MasterSlaveMode_Disable        ((uint32_t)0x0000)
#define IS_TIM_MSM_STATE(STATE) (((STATE) == TIM_MasterSlaveMode_Enable) || \
                                 ((STATE) == TIM_MasterSlaveMode_Disable))
/**
  * @}
  */ 

/** @defgroup TIMEx_Remap  TIMEx Remap
  * @{
  */
#define TIM_TIM2_TIM8_TRGO                     (0x00000000)
#define TIM_TIM2_ETH_PTP                       (0x00000400)
#define TIM_TIM2_USBFS_SOF                     (0x00000800)
#define TIM_TIM2_USBHS_SOF                     (0x00000C00)
#define TIM_TIM5_GPIO                          (0x00000000)
#define TIM_TIM5_LSI                           (0x00000040)
#define TIM_TIM5_LSE                           (0x00000080)
#define TIM_TIM5_RTC                           (0x000000C0)
#define TIM_TIM11_GPIO                         (0x00000000)
#define TIM_TIM11_SPDIFRX                      (0x00000001)
#define TIM_TIM11_HSE                          (0x00000002)
#define TIM_TIM11_MCO1                         (0x00000003)

#define IS_TIM_REMAP(__TIM_REMAP__)	 (((__TIM_REMAP__) == TIM_TIM2_TIM8_TRGO)||\
                                      ((__TIM_REMAP__) == TIM_TIM2_ETH_PTP)||\
                                      ((__TIM_REMAP__) == TIM_TIM2_USBFS_SOF)||\
                                      ((__TIM_REMAP__) == TIM_TIM2_USBHS_SOF)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_GPIO)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_LSI)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_LSE)||\
                                      ((__TIM_REMAP__) == TIM_TIM5_RTC)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_GPIO)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_SPDIFRX)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_HSE)||\
                                      ((__TIM_REMAP__) == TIM_TIM11_MCO1)) 
/**
  * @}
  */

/** @defgroup TIM_Flags 
  * @{
  */

#define TIM_FLAG_Update                    (TIM_SR_UIF)
#define TIM_FLAG_CC1                       (TIM_SR_CC1IF)
#define TIM_FLAG_CC2                       (TIM_SR_CC2IF)
#define TIM_FLAG_CC3                       (TIM_SR_CC3IF)
#define TIM_FLAG_CC4                       (TIM_SR_CC4IF)
#define TIM_FLAG_COM                       (TIM_SR_COMIF)
#define TIM_FLAG_Trigger                   (TIM_SR_TIF)
#define TIM_FLAG_Break                     (TIM_SR_BIF)
#define TIM_FLAG_Break2                    (TIM_SR_B2IF)
#define TIM_FLAG_CC1OF                     (TIM_SR_CC1OF)
#define TIM_FLAG_CC2OF                     (TIM_SR_CC2OF)
#define TIM_FLAG_CC3OF                     (TIM_SR_CC3OF)
#define TIM_FLAG_CC4OF                     (TIM_SR_CC4OF)
//#define TIM_FLAG_CC5                       (TIM_SR_CC5IF)
//#define TIM_FLAG_CC6                       (TIM_SR_CC6IF)
#define IS_TIM_GET_FLAG(FLAG) (((FLAG) == TIM_FLAG_Update) || \
                               ((FLAG) == TIM_FLAG_CC1) || \
                               ((FLAG) == TIM_FLAG_CC2) || \
                               ((FLAG) == TIM_FLAG_CC3) || \
                               ((FLAG) == TIM_FLAG_CC4) || \
                               ((FLAG) == TIM_FLAG_COM) || \
                               ((FLAG) == TIM_FLAG_Trigger) || \
                               ((FLAG) == TIM_FLAG_Break) || \
                               ((FLAG) == TIM_FLAG_Break2) || \
                               ((FLAG) == TIM_FLAG_CC1OF) || \
                               ((FLAG) == TIM_FLAG_CC2OF) || \
                               ((FLAG) == TIM_FLAG_CC3OF) || \
                               ((FLAG) == TIM_FLAG_CC4OF)) // ||\
//                               ((FLAG) == TIM_FLAG_CC5) ||\
//                               ((FLAG) == TIM_FLAG_CC6))

#define IS_TIM_CLEAR_FLAG(TIM_FLAG) ((((TIM_FLAG) & (uint32_t)0xE000) == 0x0000) && ((TIM_FLAG) != 0x0000))
/**
  * @}
  */ 

/** @defgroup TIM_OCReferenceClear 
  * @{
  */
#define TIM_OCReferenceClear_ETRF          ((uint32_t)TIM_SMCR_OCCS)
#define TIM_OCReferenceClear_OCREFCLR      ((uint32_t)0x0000)
#define TIM_OCREFERENCECECLEAR_SOURCE(SOURCE) (((SOURCE) == TIM_OCReferenceClear_ETRF) || \
                                              ((SOURCE) == TIM_OCReferenceClear_OCREFCLR))
/**
  * @}
  */ 

/** @defgroup TIM_Input_Capture_Filer_Value 
  * @{
  */

#define IS_TIM_IC_FILTER(ICFILTER) ((ICFILTER) <= 0xF) 
/**
  * @}
  */ 

/** @defgroup TIM_External_Trigger_Filter 
  * @{
  */

#define IS_TIM_EXT_FILTER(EXTFILTER) ((EXTFILTER) <= 0xF)
/**
  * @}
  */ 



/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 

/* TimeBase management ********************************************************/
void TIM_DeInit(TIM_TypeDef* TIMx);
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint32_t Prescaler, uint32_t TIM_PSCReloadMode);
void TIM_CounterModeConfig(TIM_TypeDef* TIMx, uint32_t TIM_CounterMode);
void TIM_SetCounter(TIM_TypeDef* TIMx, uint32_t Counter);
void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint32_t Autoreload);
uint32_t TIM_GetCounter(TIM_TypeDef* TIMx);
uint32_t TIM_GetPrescaler(TIM_TypeDef* TIMx);
void TIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_UpdateRequestConfig(TIM_TypeDef* TIMx, uint32_t TIM_UpdateSource);
void TIM_UIFRemap(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SelectOnePulseMode(TIM_TypeDef* TIMx, uint32_t TIM_OPMode);
void TIM_SetClockDivision(TIM_TypeDef* TIMx, uint32_t TIM_CKD);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);

/* Output Compare management **************************************************/
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC5Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC6Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_SelectGC5C1(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SelectGC5C2(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SelectGC5C3(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_SelectOCxM(TIM_TypeDef* TIMx, uint32_t TIM_Channel, uint32_t TIM_OCMode);
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint32_t Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint32_t Compare2);
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint32_t Compare3);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint32_t Compare4);
void TIM_SetCompare5(TIM_TypeDef* TIMx, uint32_t Compare5);
void TIM_SetCompare6(TIM_TypeDef* TIMx, uint32_t Compare6);
void TIM_ForcedOC1Config(TIM_TypeDef* TIMx, uint32_t TIM_ForcedAction);
void TIM_ForcedOC2Config(TIM_TypeDef* TIMx, uint32_t TIM_ForcedAction);
void TIM_ForcedOC3Config(TIM_TypeDef* TIMx, uint32_t TIM_ForcedAction);
void TIM_ForcedOC4Config(TIM_TypeDef* TIMx, uint32_t TIM_ForcedAction);
void TIM_ForcedOC5Config(TIM_TypeDef* TIMx, uint32_t TIM_ForcedAction);
void TIM_ForcedOC6Config(TIM_TypeDef* TIMx, uint32_t TIM_ForcedAction);
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPreload);
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPreload);
void TIM_OC5PreloadConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPreload);
void TIM_OC6PreloadConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPreload);
void TIM_OC1FastConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCFast);
void TIM_OC2FastConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCFast);
void TIM_OC3FastConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCFast);
void TIM_OC4FastConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCFast);
void TIM_ClearOC1Ref(TIM_TypeDef* TIMx, uint32_t TIM_OCClear);
void TIM_ClearOC2Ref(TIM_TypeDef* TIMx, uint32_t TIM_OCClear);
void TIM_ClearOC3Ref(TIM_TypeDef* TIMx, uint32_t TIM_OCClear);
void TIM_ClearOC4Ref(TIM_TypeDef* TIMx, uint32_t TIM_OCClear);
void TIM_ClearOC5Ref(TIM_TypeDef* TIMx, uint32_t TIM_OCClear);
void TIM_ClearOC6Ref(TIM_TypeDef* TIMx, uint32_t TIM_OCClear);
void TIM_SelectOCREFClear(TIM_TypeDef* TIMx, uint32_t TIM_OCReferenceClear);  
void TIM_OC1PolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPolarity);
void TIM_OC1NPolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCNPolarity);
void TIM_OC2PolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPolarity);
void TIM_OC2NPolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCNPolarity);
void TIM_OC3PolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPolarity);
void TIM_OC3NPolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCNPolarity);
void TIM_OC4PolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPolarity);
void TIM_OC5PolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPolarity);
void TIM_OC6PolarityConfig(TIM_TypeDef* TIMx, uint32_t TIM_OCPolarity);
void TIM_CCxCmd(TIM_TypeDef* TIMx, uint32_t TIM_Channel, uint32_t TIM_CCx);
void TIM_CCxNCmd(TIM_TypeDef* TIMx, uint32_t TIM_Channel, uint32_t TIM_CCxN);

/* Input Capture management ***************************************************/
void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_ICStructInit(TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);
uint32_t TIM_GetCapture1(TIM_TypeDef* TIMx);
uint32_t TIM_GetCapture2(TIM_TypeDef* TIMx);
uint32_t TIM_GetCapture3(TIM_TypeDef* TIMx);
uint32_t TIM_GetCapture4(TIM_TypeDef* TIMx);
void TIM_SetIC1Prescaler(TIM_TypeDef* TIMx, uint32_t TIM_ICPSC);
void TIM_SetIC2Prescaler(TIM_TypeDef* TIMx, uint32_t TIM_ICPSC);
void TIM_SetIC3Prescaler(TIM_TypeDef* TIMx, uint32_t TIM_ICPSC);
void TIM_SetIC4Prescaler(TIM_TypeDef* TIMx, uint32_t TIM_ICPSC);

/* Advanced-control timers (TIM1 and TIM8) specific features ******************/
void TIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void TIM_Break1Config(TIM_TypeDef* TIMx, uint32_t TIM_Break1Polarity, uint8_t TIM_Break1Filter);
void TIM_Break2Config(TIM_TypeDef* TIMx, uint32_t TIM_Break2Polarity, uint8_t TIM_Break2Filter);
void TIM_Break1Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_Break2Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef* TIM_BDTRInitStruct);
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SelectCOM(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_CCPreloadControl(TIM_TypeDef* TIMx, FunctionalState NewState);

/* Interrupts, DMA and flags management ***************************************/
void TIM_ITConfig(TIM_TypeDef* TIMx, uint32_t TIM_IT, FunctionalState NewState);
void TIM_GenerateEvent(TIM_TypeDef* TIMx, uint32_t TIM_EventSource);
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint32_t TIM_FLAG);
void TIM_ClearFlag(TIM_TypeDef* TIMx, uint32_t TIM_FLAG);
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint32_t TIM_IT);
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint32_t TIM_IT);
void TIM_DMAConfig(TIM_TypeDef* TIMx, uint32_t TIM_DMABase, uint32_t TIM_DMABurstLength);
void TIM_DMACmd(TIM_TypeDef* TIMx, uint32_t TIM_DMASource, FunctionalState NewState);
void TIM_SelectCCDMA(TIM_TypeDef* TIMx, FunctionalState NewState);

/* Clocks management **********************************************************/
void TIM_InternalClockConfig(TIM_TypeDef* TIMx);
void TIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint32_t TIM_InputTriggerSource);
void TIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint32_t TIM_TIxExternalCLKSource,
                                uint32_t TIM_ICPolarity, uint32_t ICFilter);
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint32_t TIM_ExtTRGPrescaler, uint32_t TIM_ExtTRGPolarity,
                             uint32_t ExtTRGFilter);
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint32_t TIM_ExtTRGPrescaler, 
                             uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);

/* Synchronization management *************************************************/
void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint32_t TIM_InputTriggerSource);
void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint32_t TIM_TRGOSource);
void TIM_SelectOutputTrigger2(TIM_TypeDef* TIMx, uint32_t TIM_TRGO2Source);
void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint32_t TIM_SlaveMode);
void TIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, uint32_t TIM_MasterSlaveMode);
void TIM_ETRConfig(TIM_TypeDef* TIMx, uint32_t TIM_ExtTRGPrescaler, uint32_t TIM_ExtTRGPolarity,
                   uint32_t ExtTRGFilter);

/* Specific interface management **********************************************/   
void TIM_EncoderInterfaceConfig(TIM_TypeDef* TIMx, uint32_t TIM_EncoderMode,
                                uint32_t TIM_IC1Polarity, uint32_t TIM_IC2Polarity);
void TIM_SelectHallSensor(TIM_TypeDef* TIMx, FunctionalState NewState);

/* Specific remapping management **********************************************/
void TIM_RemapConfig(TIM_TypeDef* TIMx, uint32_t TIM_Remap);

#ifdef __cplusplus
}
#endif

#endif /*__STM32F7xx_TIM_H */

/**
  * @}
  */ 

/**
  * @}
  */
