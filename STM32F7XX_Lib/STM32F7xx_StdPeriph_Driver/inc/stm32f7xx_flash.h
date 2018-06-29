
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_FLASH_H
#define __STM32F7xx_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"

/** @addtogroup STM32F7xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup FLASH
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** 
  * @brief FLASH Status  
  */ 
typedef enum
{ 
  FLASH_BUSY = 1,
  FLASH_ERROR_ERS,
  FLASH_ERROR_PGP,
  FLASH_ERROR_PGA,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_ERROR_OPERATION,
  FLASH_COMPLETE
}FLASH_Status;

/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants
  * @{
  */  

/** @defgroup Flash_Latency 
  * @{
  */ 
#define FLASH_Latency_0                FLASH_ACR_LATENCY_0WS   /*!< FLASH Zero Latency cycle      */
#define FLASH_Latency_1                FLASH_ACR_LATENCY_1WS   /*!< FLASH One Latency cycle       */
#define FLASH_Latency_2                FLASH_ACR_LATENCY_2WS   /*!< FLASH Two Latency cycles      */
#define FLASH_Latency_3                FLASH_ACR_LATENCY_3WS   /*!< FLASH Three Latency cycles    */
#define FLASH_Latency_4                FLASH_ACR_LATENCY_4WS   /*!< FLASH Four Latency cycles     */
#define FLASH_Latency_5                FLASH_ACR_LATENCY_5WS   /*!< FLASH Five Latency cycles     */
#define FLASH_Latency_6                FLASH_ACR_LATENCY_6WS   /*!< FLASH Six Latency cycles      */
#define FLASH_Latency_7                FLASH_ACR_LATENCY_7WS   /*!< FLASH Seven Latency cycles    */
#define FLASH_Latency_8                FLASH_ACR_LATENCY_8WS   /*!< FLASH Eight Latency cycles    */
#define FLASH_Latency_9                FLASH_ACR_LATENCY_9WS   /*!< FLASH Nine Latency cycles     */
#define FLASH_Latency_10               FLASH_ACR_LATENCY_10WS  /*!< FLASH Ten Latency cycles      */
#define FLASH_Latency_11               FLASH_ACR_LATENCY_11WS  /*!< FLASH Eleven Latency cycles   */
#define FLASH_Latency_12               FLASH_ACR_LATENCY_12WS  /*!< FLASH Twelve Latency cycles   */
#define FLASH_Latency_13               FLASH_ACR_LATENCY_13WS  /*!< FLASH Thirteen Latency cycles */
#define FLASH_Latency_14               FLASH_ACR_LATENCY_14WS  /*!< FLASH Fourteen Latency cycles */
#define FLASH_Latency_15               FLASH_ACR_LATENCY_15WS  /*!< FLASH Fifteen Latency cycles  */

#define IS_FLASH_LATENCY(LATENCY) (((LATENCY) == FLASH_Latency_0)  || \
                                   ((LATENCY) == FLASH_Latency_1)  || \
                                   ((LATENCY) == FLASH_Latency_2)  || \
                                   ((LATENCY) == FLASH_Latency_3)  || \
                                   ((LATENCY) == FLASH_Latency_4)  || \
                                   ((LATENCY) == FLASH_Latency_5)  || \
                                   ((LATENCY) == FLASH_Latency_6)  || \
                                   ((LATENCY) == FLASH_Latency_7)  || \
                                   ((LATENCY) == FLASH_Latency_8)  || \
                                   ((LATENCY) == FLASH_Latency_9)  || \
                                   ((LATENCY) == FLASH_Latency_10) || \
                                   ((LATENCY) == FLASH_Latency_11) || \
                                   ((LATENCY) == FLASH_Latency_12) || \
                                   ((LATENCY) == FLASH_Latency_13) || \
                                   ((LATENCY) == FLASH_Latency_14) || \
                                   ((LATENCY) == FLASH_Latency_15))
/**
  * @}
  */ 

/** @defgroup FLASH_Voltage_Range 
  * @{
  */ 
#define VoltageRange_1        ((uint8_t)0x00)  /*!< Device operating range: 1.8V to 2.1V */
#define VoltageRange_2        ((uint8_t)0x01)  /*!<Device operating range: 2.1V to 2.7V */
#define VoltageRange_3        ((uint8_t)0x02)  /*!<Device operating range: 2.7V to 3.6V */
#define VoltageRange_4        ((uint8_t)0x03)  /*!<Device operating range: 2.7V to 3.6V + External Vpp */

#define IS_VOLTAGERANGE(RANGE)(((RANGE) == VoltageRange_1) || \
                               ((RANGE) == VoltageRange_2) || \
                               ((RANGE) == VoltageRange_3) || \
                               ((RANGE) == VoltageRange_4))
/**
  * @}
  */ 

/** @defgroup FLASHEx_Sectors FLASH Sectors
  * @{
  */
#define FLASH_Sector_0     ((uint32_t)0) /*!< Sector Number 0   */
#define FLASH_Sector_1     ((uint32_t)1) /*!< Sector Number 1   */
#define FLASH_Sector_2     ((uint32_t)2) /*!< Sector Number 2   */
#define FLASH_Sector_3     ((uint32_t)3) /*!< Sector Number 3   */
#define FLASH_Sector_4     ((uint32_t)4) /*!< Sector Number 4   */
#define FLASH_Sector_5     ((uint32_t)5) /*!< Sector Number 5   */
#define FLASH_Sector_6     ((uint32_t)6) /*!< Sector Number 6   */
#define FLASH_Sector_7     ((uint32_t)7) /*!< Sector Number 7   */

#define IS_FLASH_SECTOR(SECTOR) (((SECTOR) == FLASH_Sector_0)   || ((SECTOR) == FLASH_Sector_1)   ||\
                                 ((SECTOR) == FLASH_Sector_2)   || ((SECTOR) == FLASH_Sector_3)   ||\
                                 ((SECTOR) == FLASH_Sector_4)   || ((SECTOR) == FLASH_Sector_5)   ||\
                                 ((SECTOR) == FLASH_Sector_6)   || ((SECTOR) == FLASH_Sector_7))

#define IS_FLASH_ADDRESS(ADDRESS) (((ADDRESS) >= FLASH_BASE) && ((ADDRESS) <= FLASH_END))

/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_Write_Protection FLASH Option Bytes Write Protection
  * @{
  */
#define OB_WRP_Sector_0       ((uint32_t)0x00010000) /*!< Write protection of Sector0     */
#define OB_WRP_Sector_1       ((uint32_t)0x00020000) /*!< Write protection of Sector1     */
#define OB_WRP_Sector_2       ((uint32_t)0x00040000) /*!< Write protection of Sector2     */
#define OB_WRP_Sector_3       ((uint32_t)0x00080000) /*!< Write protection of Sector3     */
#define OB_WRP_Sector_4       ((uint32_t)0x00100000) /*!< Write protection of Sector4     */
#define OB_WRP_Sector_5       ((uint32_t)0x00200000) /*!< Write protection of Sector5     */
#define OB_WRP_Sector_6       ((uint32_t)0x00400000) /*!< Write protection of Sector6     */
#define OB_WRP_Sector_7       ((uint32_t)0x00800000) /*!< Write protection of Sector7     */
#define OB_WRP_Sector_All     ((uint32_t)0x00FF0000) /*!< Write protection of all Sectors */

#define IS_OB_WRP(SECTOR)  ((((SECTOR) & (uint32_t)0xFF00FFFF) == 0x00000000) && ((SECTOR) != 0x00000000))

/**
  * @}
  */

  /** @defgroup FLASH_Option_Bytes_Read_Protection 
  * @{
  */
#define OB_RDP_Level_0   ((uint8_t)0xAA)
#define OB_RDP_Level_1   ((uint8_t)0x55)
#define OB_RDP_Level_2   ((uint8_t)0xCC) /*!< Warning: When enabling read protection level 2 
                                              it's no more possible to go back to level 1 or 0 */
#define IS_OB_RDP(LEVEL) (((LEVEL) == OB_RDP_Level_0)||\
                          ((LEVEL) == OB_RDP_Level_1))/*||\
                          ((LEVEL) == OB_RDP_Level_2))*/
/**
  * @}
  */ 

/** @defgroup FLASHEx_Option_Bytes_WWatchdog FLASH Option Bytes WWatchdog
  * @{
  */ 
#define OB_WWDG_SW                      ((uint32_t)0x10)  /*!< Software WWDG selected */
#define OB_WWDG_HW                      ((uint32_t)0x00)  /*!< Hardware WWDG selected */
#define IS_OB_WWDG_SOURCE(SOURCE) (((SOURCE) == OB_WWDG_SW) || ((SOURCE) == OB_WWDG_HW))
/**
  * @}
  */ 

/** @defgroup FLASH_Option_Bytes_IWatchdog 
  * @{
  */ 
#define OB_IWDG_SW                     ((uint8_t)0x20)  /*!< Software IWDG selected */
#define OB_IWDG_HW                     ((uint8_t)0x00)  /*!< Hardware IWDG selected */
#define IS_OB_IWDG_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_SW) || ((SOURCE) == OB_IWDG_HW))
/**
  * @}
  */ 

/** @defgroup FLASH_Option_Bytes_nRST_STOP 
  * @{
  */ 
#define OB_STOP_NoRST                  ((uint8_t)0x40) /*!< No reset generated when entering in STOP */
#define OB_STOP_RST                    ((uint8_t)0x00) /*!< Reset generated when entering in STOP */
#define IS_OB_STOP_SOURCE(SOURCE) (((SOURCE) == OB_STOP_NoRST) || ((SOURCE) == OB_STOP_RST))
/**
  * @}
  */ 


/** @defgroup FLASH_Option_Bytes_nRST_STDBY 
  * @{
  */ 
#define OB_STDBY_NoRST                 ((uint8_t)0x80) /*!< No reset generated when entering in STANDBY */
#define OB_STDBY_RST                   ((uint8_t)0x00) /*!< Reset generated when entering in STANDBY */
#define IS_OB_STDBY_SOURCE(SOURCE) (((SOURCE) == OB_STDBY_NoRST) || ((SOURCE) == OB_STDBY_RST))
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_IWDG_FREEZE_STOP FLASH IWDG Counter Freeze in STOP
  * @{
  */
#define OB_IWDG_STOP_FREEZE      ((uint32_t)0x00000000) /*!< Freeze IWDG counter in STOP mode */
#define OB_IWDG_STOP_ACTIVE      ((uint32_t)0x40000000) /*!< IWDG counter active in STOP mode */
#define IS_OB_IWDG_STOP_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_STOP_FREEZE) || ((SOURCE) == OB_IWDG_STOP_ACTIVE))
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_IWDG_FREEZE_SANDBY FLASH IWDG Counter Freeze in STANDBY
  * @{
  */
#define OB_IWDG_STDBY_FREEZE      ((uint32_t)0x00000000) /*!< Freeze IWDG counter in STANDBY mode */
#define OB_IWDG_STDBY_ACTIVE      ((uint32_t)0x40000000) /*!< IWDG counter active in STANDBY mode */
#define IS_OB_IWDG_STDBY_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_STDBY_FREEZE) || ((SOURCE) == OB_IWDG_STDBY_ACTIVE))
/**
  * @}
  */

/** @defgroup FLASHEx_BOR_Reset_Level FLASH BOR Reset Level
  * @{
  */
#define OB_BOR_LEVEL3          ((uint32_t)0x00)  /*!< Supply voltage ranges from 2.70 to 3.60 V */
#define OB_BOR_LEVEL2          ((uint32_t)0x04)  /*!< Supply voltage ranges from 2.40 to 2.70 V */
#define OB_BOR_LEVEL1          ((uint32_t)0x08)  /*!< Supply voltage ranges from 2.10 to 2.40 V */
#define OB_BOR_OFF             ((uint32_t)0x0C)  /*!< Supply voltage ranges from 1.62 to 2.10 V */
#define IS_OB_BOR(LEVEL) (((LEVEL) == OB_BOR_LEVEL1) || ((LEVEL) == OB_BOR_LEVEL2) ||\
                          ((LEVEL) == OB_BOR_LEVEL3) || ((LEVEL) == OB_BOR_OFF))
/**
  * @}
  */

/** @defgroup FLASHEx_Boot_Address FLASH Boot Address
  * @{
  */
#define OPTIONBYTE_BOOTADDR_0  ((uint32_t)0x10)  /*!< Boot 0 Address configuration   */
#define OPTIONBYTE_BOOTADDR_1  ((uint32_t)0x20)  /*!< Boot 1 Address configuration   */
#define IS_OB_BOOT_OPTION(BOOT)       (((BOOT) == OPTIONBYTE_BOOTADDR_0)   || \
                                     ((BOOT) == OPTIONBYTE_BOOTADDR_1))
/**
  * @}
  */

/** @defgroup FLASHEx_Boot_Address FLASH Boot Address
  * @{
  */
#define OB_BOOTADDR_ITCM_RAM         ((uint32_t)0x0000)  /*!< Boot from ITCM RAM (0x00000000)                 */
#define OB_BOOTADDR_SYSTEM           ((uint32_t)0x0040)  /*!< Boot from System memory bootloader (0x00100000) */
#define OB_BOOTADDR_ITCM_FLASH       ((uint32_t)0x0080)  /*!< Boot from Flash on ITCM interface (0x00200000)  */
#define OB_BOOTADDR_AXIM_FLASH       ((uint32_t)0x2000)  /*!< Boot from Flash on AXIM interface (0x08000000)  */
#define OB_BOOTADDR_DTCM_RAM         ((uint32_t)0x8000)  /*!< Boot from DTCM RAM (0x20000000)                 */
#define OB_BOOTADDR_SRAM1            ((uint32_t)0x8004)  /*!< Boot from SRAM1 (0x20010000)                    */
#define OB_BOOTADDR_SRAM2            ((uint32_t)0x8013)  /*!< Boot from SRAM2 (0x2004C000)                    */
#define IS_OB_BOOT_ADDRESS(ADDRESS) (((ADDRESS) == OB_BOOTADDR_ITCM_RAM)   || \
                                     ((ADDRESS) == OB_BOOTADDR_SYSTEM)     || \
                                     ((ADDRESS) == OB_BOOTADDR_ITCM_FLASH) || \
                                     ((ADDRESS) == OB_BOOTADDR_AXIM_FLASH) || \
                                     ((ADDRESS) == OB_BOOTADDR_DTCM_RAM)   || \
                                     ((ADDRESS) == OB_BOOTADDR_SRAM1)      || \
                                     ((ADDRESS) == OB_BOOTADDR_SRAM2))
/**
  * @}
  */

/** @defgroup FLASH_Interrupts 
  * @{
  */ 
#define FLASH_IT_EOP                   ((uint32_t)FLASH_CR_EOPIE)  /*!< End of FLASH Operation Interrupt source */
#define FLASH_IT_ERR                   ((uint32_t)FLASH_CR_ERRIE)  /*!< Error Interrupt source */
#define IS_FLASH_IT(IT) ((((IT) & (uint32_t)0xFCFFFFFF) == 0x00000000) && ((IT) != 0x00000000))
/**
  * @}
  */ 

/** @defgroup FLASH_Flags 
  * @{
  */ 
#define FLASH_FLAG_EOP                 ((uint32_t)FLASH_SR_EOP)  /*!< FLASH End of Operation flag               */
#define FLASH_FLAG_OPERR               ((uint32_t)FLASH_SR_OPERR)  /*!< FLASH operation Error flag                */
#define FLASH_FLAG_WRPERR              ((uint32_t)FLASH_SR_WRPERR)  /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_PGAERR              ((uint32_t)FLASH_SR_PGAERR)  /*!< FLASH Programming Alignment error flag    */
#define FLASH_FLAG_PGPERR              ((uint32_t)FLASH_SR_PGPERR)  /*!< FLASH Programming Parallelism error flag  */
#define FLASH_FLAG_ERSERR              ((uint32_t)FLASH_SR_ERSERR)  /*!< FLASH Erasing Sequence error flag         */
#define FLASH_FLAG_BSY                 ((uint32_t)FLASH_SR_BSY)  /*!< FLASH Busy flag                           */ 
#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFE0C) == 0x00000000) && ((FLAG) != 0x00000000))
#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_EOP)    || ((FLAG) == FLASH_FLAG_OPERR)  || \
                                  ((FLAG) == FLASH_FLAG_WRPERR) || ((FLAG) == FLASH_FLAG_PGAERR) || \
                                  ((FLAG) == FLASH_FLAG_PGPERR) || ((FLAG) == FLASH_FLAG_ERSERR) || \
                                  ((FLAG) == FLASH_FLAG_BSY))
/**
  * @}
  */

/** @defgroup FLASH_Program_Parallelism FLASH Program Parallelism
  * @{
  */
#define FLASH_PSIZE_BYTE           ((uint32_t)0x00000000)
#define FLASH_PSIZE_HALF_WORD      ((uint32_t)FLASH_CR_PSIZE_0)
#define FLASH_PSIZE_WORD           ((uint32_t)FLASH_CR_PSIZE_1)
#define FLASH_PSIZE_DOUBLE_WORD    ((uint32_t)FLASH_CR_PSIZE)
#define CR_PSIZE_MASK              ((uint32_t)0xFFFFFCFF)
/**
  * @}
  */

/** @defgroup FLASH_Keys FLASH Keys
  * @{
  */ 
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
#define FLASH_OPT_KEY1           ((uint32_t)0x08192A3B)
#define FLASH_OPT_KEY2           ((uint32_t)0x4C5D6E7F)
/**
  * @}
  */


/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 
 
/* FLASH Interface configuration functions ************************************/
void FLASH_SetLatency(uint32_t FLASH_Latency);
void FLASH_PrefetchBufferCmd(FunctionalState NewState);
void FLASH_AdaptiveRealTimeCmd(FunctionalState NewState);
void FLASH_AdaptiveRealTimeReset(void);

/* FLASH Memory Programming functions *****************************************/   
void         FLASH_Unlock(void);
void         FLASH_Lock(void);
FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector, uint8_t VoltageRange);
FLASH_Status FLASH_EraseAllSectors(uint8_t VoltageRange);
FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data);

/* Option Bytes Programming functions *****************************************/ 
void         FLASH_OB_Unlock(void);
void         FLASH_OB_Lock(void);
void         FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState);
void         FLASH_OB_RDPConfig(uint8_t OB_RDP);
void         FLASH_OB_UserConfig(uint32_t Wwdg, uint32_t Iwdg, uint32_t Stop, uint32_t Stdby, uint32_t Iwdgstop, uint32_t Iwdgstdby );
void         FLASH_OB_BORConfig(uint8_t OB_BOR);
FLASH_Status FLASH_OB_BootAddressConfig(uint32_t BootOption, uint32_t Address);
FLASH_Status FLASH_OB_Launch(void);
uint32_t     FLASH_OB_GetUser(void);
uint32_t     FLASH_OB_GetWRP(void);
uint8_t      FLASH_OB_GetRDP(void);
uint32_t     FLASH_OB_GetBOR(void);

/* Interrupts and flags management functions **********************************/
void         FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus   FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void         FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_FLASH_H */

/**
  * @}
  */ 

/**
  * @}
  */ 