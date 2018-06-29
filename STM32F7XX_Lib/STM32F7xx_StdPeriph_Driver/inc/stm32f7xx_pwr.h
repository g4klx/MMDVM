/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_PWR_H
#define __STM32F7xx_PWR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"

/** @addtogroup STM32F7xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup PWR
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup PWR_Exported_Constants
  * @{
  */ 

/** @defgroup PWR_PVD_detection_level 
  * @{
  */ 
#define PWR_PVDLevel_0                  PWR_CR1_PLS_LEV0
#define PWR_PVDLevel_1                  PWR_CR1_PLS_LEV1
#define PWR_PVDLevel_2                  PWR_CR1_PLS_LEV2
#define PWR_PVDLevel_3                  PWR_CR1_PLS_LEV3
#define PWR_PVDLevel_4                  PWR_CR1_PLS_LEV4
#define PWR_PVDLevel_5                  PWR_CR1_PLS_LEV5
#define PWR_PVDLevel_6                  PWR_CR1_PLS_LEV6
#define PWR_PVDLevel_7                  PWR_CR1_PLS_LEV7

#define IS_PWR_PVD_LEVEL(LEVEL) (((LEVEL) == PWR_PVDLevel_0) || ((LEVEL) == PWR_PVDLevel_1)|| \
                                 ((LEVEL) == PWR_PVDLevel_2) || ((LEVEL) == PWR_PVDLevel_3)|| \
                                 ((LEVEL) == PWR_PVDLevel_4) || ((LEVEL) == PWR_PVDLevel_5)|| \
                                 ((LEVEL) == PWR_PVDLevel_6) || ((LEVEL) == PWR_PVDLevel_7))
/**
  * @}
  */
    
/** @defgroup PWR_Regulator_state_in_STOP_mode 
  * @{
  */
#define PWR_MainRegulator_ON                        ((uint32_t)0x00000000)
#define PWR_LowPowerRegulator_ON                    PWR_CR1_LPDS

/* --- PWR_Legacy ---*/
#define PWR_Regulator_ON                            PWR_MainRegulator_ON
#define PWR_Regulator_LowPower                      PWR_LowPowerRegulator_ON

#define IS_PWR_REGULATOR(REGULATOR) (((REGULATOR) == PWR_MainRegulator_ON) || \
                                     ((REGULATOR) == PWR_LowPowerRegulator_ON))

/**
  * @}
  */

/** @defgroup PWR_Regulator_state_in_UnderDrive_mode 
  * @{
  */
#define PWR_MainRegulator_UnderDrive_ON               PWR_CR1_MRUDS
#define PWR_LowPowerRegulator_UnderDrive_ON           ((uint32_t)(PWR_CR1_LPDS | PWR_CR1_LPUDS))

#define IS_PWR_REGULATOR_UNDERDRIVE(REGULATOR) (((REGULATOR) == PWR_MainRegulator_UnderDrive_ON) || \
                                                ((REGULATOR) == PWR_LowPowerRegulator_UnderDrive_ON))

/**
  * @}
  */

/** @defgroup PWR_Wake_Up_Pin
  * @{
  */
#define PWR_WakeUp_Pin1                PWR_CSR2_EWUP1           /*!< Wakeup Pin PA0            */
#define PWR_WakeUp_Pin2                PWR_CSR2_EWUP2           /*!< Wakeup Pin PA2            */
#define PWR_WakeUp_Pin3                PWR_CSR2_EWUP3           /*!< Wakeup Pin PC1            */
#define PWR_WakeUp_Pin4                PWR_CSR2_EWUP4           /*!< Wakeup Pin PC13           */
#define PWR_WakeUp_Pin5                PWR_CSR2_EWUP5           /*!< Wakeup Pin PI8            */
#define PWR_WakeUp_Pin6                PWR_CSR2_EWUP6           /*!< Wakeup Pin PI11           */
#define PWR_WakeUp_Pin1_RisingEdge     PWR_CSR2_EWUP1           /*!< Wakeup Pin PA0 with risig edge */
#define PWR_WakeUp_Pin2_RisingEdge     PWR_CSR2_EWUP2           /*!< Wakeup Pin PA2 with risig edge */
#define PWR_WakeUp_Pin3_RisingEdge     PWR_CSR2_EWUP3           /*!< Wakeup Pin PC1 with risig edge */
#define PWR_WakeUp_Pin4_RisingEdge     PWR_CSR2_EWUP4           /*!< Wakeup Pin PC13 with risig edge */
#define PWR_WakeUp_Pin5_RisingEdge     PWR_CSR2_EWUP5           /*!< Wakeup Pin PI8 with risig edge */
#define PWR_WakeUp_Pin6_RisingEdge     PWR_CSR2_EWUP6           /*!< Wakeup Pin PI11 with risig edge */
#define PWR_WakeUp_Pin1_FallingEdge    (uint32_t)((PWR_CR2_WUPP1<<6) | PWR_CSR2_EWUP1)  /*!< Wakeup Pin PA0 with falling edge */
#define PWR_WakeUp_Pin2_FallingEdge    (uint32_t)((PWR_CR2_WUPP2<<6) | PWR_CSR2_EWUP2)  /*!< Wakeup Pin PA2 with falling edge */
#define PWR_WakeUp_Pin3_FallingEdge    (uint32_t)((PWR_CR2_WUPP3<<6) | PWR_CSR2_EWUP3)  /*!< Wakeup Pin PC1 with falling edge */
#define PWR_WakeUp_Pin4_FallingEdge    (uint32_t)((PWR_CR2_WUPP4<<6) | PWR_CSR2_EWUP4)  /*!< Wakeup Pin PC13 with falling edge */
#define PWR_WakeUp_Pin5_FallingEdge    (uint32_t)((PWR_CR2_WUPP5<<6) | PWR_CSR2_EWUP5)  /*!< Wakeup Pin PI8 with falling edge */
#define PWR_WakeUp_Pin6_FallingEdge    (uint32_t)((PWR_CR2_WUPP6<<6) | PWR_CSR2_EWUP6)  /*!< Wakeup Pin PI11 with falling edge */

#define IS_PWR_WAKEUP_PIN(__PIN__)         (((__PIN__) == PWR_WakeUp_Pin1)       || \
                                            ((__PIN__) == PWR_WakeUp_Pin2)       || \
                                            ((__PIN__) == PWR_WakeUp_Pin3)       || \
                                            ((__PIN__) == PWR_WakeUp_Pin4)       || \
                                            ((__PIN__) == PWR_WakeUp_Pin5)       || \
                                            ((__PIN__) == PWR_WakeUp_Pin6)  		 || \
                                            ((__PIN__) == PWR_WakeUp_Pin1_RisingEdge)  || \
                                            ((__PIN__) == PWR_WakeUp_Pin2_RisingEdge)  || \
                                            ((__PIN__) == PWR_WakeUp_Pin3_RisingEdge)  || \
                                            ((__PIN__) == PWR_WakeUp_Pin4_RisingEdge)  || \
                                            ((__PIN__) == PWR_WakeUp_Pin5_RisingEdge)  || \
                                            ((__PIN__) == PWR_WakeUp_Pin6_RisingEdge)  || \
                                            ((__PIN__) == PWR_WakeUp_Pin1_FallingEdge)   || \
                                            ((__PIN__) == PWR_WakeUp_Pin2_FallingEdge)   || \
                                            ((__PIN__) == PWR_WakeUp_Pin3_FallingEdge)   || \
                                            ((__PIN__) == PWR_WakeUp_Pin4_FallingEdge)   || \
                                            ((__PIN__) == PWR_WakeUp_Pin5_FallingEdge)	 || \
                                            ((__PIN__) == PWR_WakeUp_Pin6_FallingEdge))

/**
  * @}
  */    


/** @defgroup PWR_STOP_mode_entry 
  * @{
  */
#define PWR_STOPEntry_WFI               ((uint8_t)0x01)
#define PWR_STOPEntry_WFE               ((uint8_t)0x02)
#define IS_PWR_STOP_ENTRY(ENTRY) (((ENTRY) == PWR_STOPEntry_WFI) || ((ENTRY) == PWR_STOPEntry_WFE))
/**
  * @}
  */

/** @defgroup PWR_Regulator_Voltage_Scale 
  * @{
  */
#define PWR_Regulator_Voltage_Scale1    ((uint32_t)PWR_CR1_VOS)
#define PWR_Regulator_Voltage_Scale2    ((uint32_t)PWR_CR1_VOS_1)
#define PWR_Regulator_Voltage_Scale3    ((uint32_t)PWR_CR1_VOS_0)
#define IS_PWR_REGULATOR_VOLTAGE(VOLTAGE) (((VOLTAGE) == PWR_Regulator_Voltage_Scale1) || \
                                           ((VOLTAGE) == PWR_Regulator_Voltage_Scale2) || \
                                           ((VOLTAGE) == PWR_Regulator_Voltage_Scale3))
/**
  * @}
  */

/** @defgroup PWR_Flag 
  * @{
  */
#define PWR_FLAG_WU1                    (PWR_CSR2_WUPF1 | 0x80000000)
#define PWR_FLAG_WU2                    (PWR_CSR2_WUPF2 | 0x80000000)
#define PWR_FLAG_WU3                    (PWR_CSR2_WUPF3 | 0x80000000)
#define PWR_FLAG_WU4                    (PWR_CSR2_WUPF4 | 0x80000000)
#define PWR_FLAG_WU5                    (PWR_CSR2_WUPF5 | 0x80000000)
#define PWR_FLAG_WU6                    (PWR_CSR2_WUPF6 | 0x80000000)

#define PWR_FLAG_SB                     PWR_CSR1_SBF
#define PWR_FLAG_PVDO                   PWR_CSR1_PVDO
#define PWR_FLAG_BRR                    PWR_CSR1_BRR
#define PWR_FLAG_VOSRDY                 PWR_CSR1_VOSRDY
#define PWR_FLAG_ODRDY                  PWR_CSR1_ODRDY
#define PWR_FLAG_ODSWRDY                PWR_CSR1_ODSWRDY
#define PWR_FLAG_UDRDY                  PWR_CSR1_UDSWRDY

/* --- FLAG Legacy ---*/
#define PWR_FLAG_REGRDY                  PWR_FLAG_VOSRDY               

#define IS_PWR_GET_FLAG(FLAG) (((FLAG) == PWR_FLAG_WU1) || \
                                ((FLAG) == PWR_FLAG_WU2) || \
                                ((FLAG) == PWR_FLAG_WU3) || \
                                ((FLAG) == PWR_FLAG_WU4) || \
                                ((FLAG) == PWR_FLAG_WU5) || \
                                ((FLAG) == PWR_FLAG_WU6) || \
                                ((FLAG) == PWR_FLAG_SB) || \
                                ((FLAG) == PWR_FLAG_PVDO) || ((FLAG) == PWR_FLAG_BRR) || \
                                ((FLAG) == PWR_FLAG_VOSRDY) || ((FLAG) == PWR_FLAG_ODRDY) || \
                                ((FLAG) == PWR_FLAG_ODSWRDY) || ((FLAG) == PWR_FLAG_UDRDY))


#define IS_PWR_CLEAR_FLAG(FLAG) (((FLAG) == PWR_FLAG_WU1) || \
                                 ((FLAG) == PWR_FLAG_WU2) || \
                                 ((FLAG) == PWR_FLAG_WU3) || \
                                 ((FLAG) == PWR_FLAG_WU4) || \
                                 ((FLAG) == PWR_FLAG_WU5) || \
                                 ((FLAG) == PWR_FLAG_WU6) || \
                                 ((FLAG) == PWR_FLAG_SB) || \
                                 ((FLAG) == PWR_FLAG_UDRDY))
                                   

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 

/* Function used to set the PWR configuration to the default reset state ******/ 
void PWR_DeInit(void);

/* Backup Domain Access function **********************************************/ 
void PWR_BackupAccessCmd(FunctionalState NewState);

/* PVD configuration functions ************************************************/ 
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void PWR_PVDCmd(FunctionalState NewState);

/* WakeUp pins configuration functions ****************************************/
void PWR_WakeUpPinCmd(uint32_t PWR_WakeUpPinx, FunctionalState NewState);

/* Main and Backup Regulators configuration functions *************************/ 
void PWR_BackupRegulatorCmd(FunctionalState NewState);
void PWR_MainRegulatorModeConfig(uint32_t PWR_Regulator_Voltage);
void PWR_OverDriveCmd(FunctionalState NewState);
void PWR_OverDriveSWCmd(FunctionalState NewState);
void PWR_UnderDriveCmd(FunctionalState NewState);

void PWR_MainRegulatorUnderDriveCmd(FunctionalState NewState);
void PWR_LowRegulatorUnderDriveCmd(FunctionalState NewState);

/* FLASH Power Down configuration functions ***********************************/ 
void PWR_FlashPowerDownCmd(FunctionalState NewState);

/* Low Power modes configuration functions ************************************/ 
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void PWR_EnterUnderDriveSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void PWR_EnterSTANDBYMode(void);

/* Flags management functions *************************************************/ 
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
void PWR_ClearFlag(uint32_t PWR_FLAG);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_PWR_H */

/**
  * @}
  */

/**
  * @}
  */