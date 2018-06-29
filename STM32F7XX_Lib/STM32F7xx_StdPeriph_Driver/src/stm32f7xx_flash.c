/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_flash.h"

/** @addtogroup STM32F7xx_StdPeriph_Driver
  * @{
  */

/** @defgroup FLASH 
  * @brief FLASH driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
#define SECTOR_MASK               ((uint32_t)0xFFFFFF07)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FLASH_Private_Functions
  * @{
  */ 

/** @defgroup FLASH_Group1 FLASH Interface configuration functions
  *  @brief   FLASH Interface configuration functions 
 *

@verbatim   
 ===============================================================================
              ##### FLASH Interface configuration functions #####
 ===============================================================================
    [..]
      This group includes the following functions:
      (+) void FLASH_SetLatency(uint32_t FLASH_Latency)
          To correctly read data from FLASH memory, the number of wait states (LATENCY) 
          must be correctly programmed according to the frequency of the CPU clock 
          (HCLK) and the supply voltage of the device.
    [..]      
      For STM32F405xx/07xx and STM32F415xx/17xx devices
 +-------------------------------------------------------------------------------------+
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
 |---------------|----------------|----------------|-----------------|-----------------|
 |5WS(6CPU cycle)|150< HCLK <= 168|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120|
 |---------------|----------------|----------------|-----------------|-----------------|
 |6WS(7CPU cycle)|      NA        |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140|
 |---------------|----------------|----------------|-----------------|-----------------|
 |7WS(8CPU cycle)|      NA        |      NA        |154 < HCLK <= 168|140 < HCLK <= 160|
 +---------------|----------------|----------------|-----------------|-----------------+
 
 [..]
 +-------------------------------------------------------------------------------------------------------------------+
 |               | voltage range  | voltage range  | voltage range   | voltage range   | voltage range 2.7 V - 3.6 V |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   | with External Vpp = 9V      |
 |---------------|----------------|----------------|-----------------|-----------------|-----------------------------|
 |Max Parallelism|      x32       |               x16                |       x8        |          x64                |
 |---------------|----------------|----------------|-----------------|-----------------|-----------------------------|
 |PSIZE[1:0]     |      10        |               01                 |       00        |           11                |
 +-------------------------------------------------------------------------------------------------------------------+

      -@- On STM32F405xx/407xx and STM32F415xx/417xx devices: 
           (++) when VOS = '0' Scale 2 mode, the maximum value of fHCLK = 144MHz. 
           (++) when VOS = '1' Scale 1 mode, the maximum value of fHCLK = 168MHz. 
          [..] 
          On STM32F42xxx/43xxx devices:
           (++) when VOS[1:0] = '0x01' Scale 3 mode, the maximum value of fHCLK is 120MHz.
           (++) when VOS[1:0] = '0x10' Scale 2 mode, the maximum value of fHCLK is 144MHz if OverDrive OFF and 168MHz if OverDrive ON.
           (++) when VOS[1:0] = '0x11' Scale 1 mode, the maximum value of fHCLK is 168MHz if OverDrive OFF and 180MHz if OverDrive ON. 
          [..]
          On STM32F401x devices:
           (++) when VOS[1:0] = '0x01' Scale 3 mode, the maximum value of fHCLK is 60MHz.
           (++) when VOS[1:0] = '0x10' Scale 2 mode, the maximum value of fHCLK is 84MHz.
          [..]  
          On STM32F411xE devices:
           (++) when VOS[1:0] = '0x01' Scale 3 mode, the maximum value of fHCLK is 64MHz.
           (++) when VOS[1:0] = '0x10' Scale 2 mode, the maximum value of fHCLK is 84MHz.
           (++) when VOS[1:0] = '0x11' Scale 1 mode, the maximum value of fHCLK is 100MHz.

        For more details please refer product DataSheet 
           You can use PWR_MainRegulatorModeConfig() function to control VOS bits.

      (+) void FLASH_PrefetchBufferCmd(FunctionalState NewState)
      (+) void FLASH_InstructionCacheCmd(FunctionalState NewState)
      (+) void FLASH_DataCacheCmd(FunctionalState NewState)
      (+) void FLASH_InstructionCacheReset(void)
      (+) void FLASH_DataCacheReset(void)
      
    [..]   
      The unlock sequence is not needed for these functions.
 
@endverbatim
  * @{
  */
 
/**
  * @brief  Sets the code latency value.  
  * @param  FLASH_Latency: specifies the FLASH Latency value.
  *          This parameter can be one of the following values:
  *            @arg FLASH_Latency_0: FLASH Zero Latency cycle
  *            @arg FLASH_Latency_1: FLASH One Latency cycle
  *            @arg FLASH_Latency_2: FLASH Two Latency cycles
  *            @arg FLASH_Latency_3: FLASH Three Latency cycles
  *            @arg FLASH_Latency_4: FLASH Four Latency cycles 
  *            @arg FLASH_Latency_5: FLASH Five Latency cycles 
  *            @arg FLASH_Latency_6: FLASH Six Latency cycles
  *            @arg FLASH_Latency_7: FLASH Seven Latency cycles 
  *            @arg FLASH_Latency_8: FLASH Eight Latency cycles
  *            @arg FLASH_Latency_9: FLASH Nine Latency cycles
  *            @arg FLASH_Latency_10: FLASH Teen Latency cycles 
  *            @arg FLASH_Latency_11: FLASH Eleven Latency cycles 
  *            @arg FLASH_Latency_12: FLASH Twelve Latency cycles
  *            @arg FLASH_Latency_13: FLASH Thirteen Latency cycles
  *            @arg FLASH_Latency_14: FLASH Fourteen Latency cycles
  *            @arg FLASH_Latency_15: FLASH Fifteen Latency cycles 
  *              
  * @note For STM32F405xx/407xx, STM32F415xx/417xx and STM32F401xx/411xE devices this parameter
  *       can be a value between FLASH_Latency_0 and FLASH_Latency_7.
  *
  * @note For STM32F42xxx/43xxx devices this parameter can be a value between 
  *       FLASH_Latency_0 and FLASH_Latency_15. 
  *         
  * @retval None
  */
void FLASH_SetLatency(uint32_t FLASH_Latency)
{
  /* Check the parameters */
  assert_param(IS_FLASH_LATENCY(FLASH_Latency));
  
  /* Perform Byte access to FLASH_ACR[8:0] to set the Latency value */
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, (uint32_t)FLASH_Latency);
}


/**
  * @brief  Enables or disables the Prefetch Buffer.
  * @param  NewState: new state of the Prefetch Buffer.
  *          This parameter  can be: ENABLE or DISABLE.
  * @retval None
  */
void FLASH_PrefetchBufferCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* Enable or disable the Prefetch Buffer */
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_PRFTEN;
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_PRFTEN);
  }
}

/**
  * @brief  Enable the FLASH Adaptive Real-Time memory accelerator.
  * @note   The ART accelerator is available only for flash access on ITCM interface.
  * @retval none
  */ 
void FLASH_AdaptiveRealTimeCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_ARTEN;
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_ARTEN);
  }
}

/**
  * @brief  Disable the FLASH Adaptive Real-Time memory accelerator.
  * @retval none
  */ 
void FLASH_AdaptiveRealTimeReset(void)
{
  FLASH->ACR |= FLASH_ACR_ARTRST;
}

/**
  * @}
  */


/** @defgroup FLASH_Group2 FLASH Memory Programming functions
 *  @brief   FLASH Memory Programming functions
 *
@verbatim   
 ===============================================================================
                ##### FLASH Memory Programming functions #####
 ===============================================================================   
    [..]
      This group includes the following functions:
      (+) void FLASH_Unlock(void)
      (+) void FLASH_Lock(void)
      (+) FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector, uint8_t VoltageRange)
      (+) FLASH_Status FLASH_EraseAllSectors(uint8_t VoltageRange)       
      (+) FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
      (+) FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
      (+) FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
      (+) FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data)
          The following functions can be used only for STM32F42xxx/43xxx devices. 
      (+) FLASH_Status FLASH_EraseAllBank1Sectors(uint8_t VoltageRange)
      (+) FLASH_Status FLASH_EraseAllBank2Sectors(uint8_t VoltageRange)    
    [..]   
      Any operation of erase or program should follow these steps:
      (#) Call the FLASH_Unlock() function to enable the FLASH control register access

      (#) Call the desired function to erase sector(s) or program data

      (#) Call the FLASH_Lock() function to disable the FLASH control register access
          (recommended to protect the FLASH memory against possible unwanted operation)
    
@endverbatim
  * @{
  */

/**
  * @brief  Unlocks the FLASH control register access
  * @param  None
  * @retval None
  */
void FLASH_Unlock(void)
{
  if((FLASH->CR & FLASH_CR_LOCK) != RESET)
  {
    /* Authorize the FLASH Registers access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }  
}

/**
  * @brief  Locks the FLASH control register access
  * @param  None
  * @retval None
  */
void FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH Registers access */
  FLASH->CR |= FLASH_CR_LOCK;
}

/**
  * @brief  Erase the specified FLASH memory sector
  * @param  Sector: FLASH sector to erase
  *         The value of this parameter depend on device used within the same series      
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.  
  *          This parameter can be one of the following values:
  *            @arg FLASH_VOLTAGE_RANGE_1: when the device voltage range is 1.8V to 2.1V, 
  *                                  the operation will be done by byte (8-bit) 
  *            @arg FLASH_VOLTAGE_RANGE_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg FLASH_VOLTAGE_RANGE_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg FLASH_VOLTAGE_RANGE_4: when the device voltage range is 2.7V to 3.6V + External Vpp, 
  *                                  the operation will be done by double word (64-bit)
  * 
  * @retval None
  */
FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector, uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0;
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_FLASH_SECTOR(FLASH_Sector));
  assert_param(IS_VOLTAGERANGE(VoltageRange));
  
  if(VoltageRange == VoltageRange_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;
  }
  else if(VoltageRange == VoltageRange_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD;
  }
  else if(VoltageRange == VoltageRange_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD;
  }
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to erase the sector */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= tmp_psize;
    FLASH->CR &= SECTOR_MASK;
    FLASH->CR |= FLASH_CR_SER | (FLASH_Sector << POSITION_VAL(FLASH_CR_SNB));
    FLASH->CR |= FLASH_CR_STRT;
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation();
    
    /* if the erase operation is completed, disable the SER Bit */
    FLASH->CR &= (~FLASH_CR_SER);
    FLASH->CR &= SECTOR_MASK; 
    
    /* Data synchronous Barrier (DSB) Just after the write operation
       This will force the CPU to respect the sequence of instruction (no optimization).*/
    __DSB();
  }
  /* Return the Erase Status */
  return status;
}

/**
  * @brief  Erases all FLASH Sectors.
  *
  * @note   If an erase and a program operations are requested simultaneously,    
  *         the erase operation is performed before the program one.
  *  
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.  
  *          This parameter can be one of the following values:
  *            @arg VoltageRange_1: when the device voltage range is 1.8V to 2.1V, 
  *                                  the operation will be done by byte (8-bit) 
  *            @arg VoltageRange_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg VoltageRange_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg VoltageRange_4: when the device voltage range is 2.7V to 3.6V + External Vpp, 
  *                                  the operation will be done by double word (64-bit)
  *       
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_EraseAllSectors(uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0x0;
  FLASH_Status status = FLASH_COMPLETE;
  
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();
  assert_param(IS_VOLTAGERANGE(VoltageRange));
  
  if(VoltageRange == VoltageRange_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;
  }
  else if(VoltageRange == VoltageRange_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD;
  }
  else if(VoltageRange == VoltageRange_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD;
  }  
  if(status == FLASH_COMPLETE)
  {
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= tmp_psize;
    FLASH->CR |= FLASH_CR_MER;
    FLASH->CR |= FLASH_CR_STRT;
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation();

    /* if the erase operation is completed, disable the MER Bit */
    FLASH->CR &= (~FLASH_CR_MER);
  }   
  /* Return the Erase Status */
  return status;
}

/**
  * @brief  Programs a double word (64-bit) at a specified address.
  * @note   This function must be used when the device voltage range is from
  *         2.7V to 3.6V and an External Vpp is present.
  *
  * @note   If an erase and a program operations are requested simultaneously,    
  *         the erase operation is performed before the program one.
  *  
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint64_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation();

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Programs a word (32-bit) at a specified address.
  *
  * @note   This function must be used when the device voltage range is from 2.7V to 3.6V. 
  *
  * @note   If an erase and a program operations are requested simultaneously,    
  *         the erase operation is performed before the program one.
  *  
  * @param  Address: specifies the address to be programmed.
  *         This parameter can be any address in Program memory zone or in OTP zone.  
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint32_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation();

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Programs a half word (16-bit) at a specified address. 
  * @note   This function must be used when the device voltage range is from 2.1V to 3.6V. 
  *
  * @note   If an erase and a program operations are requested simultaneously,    
  *         the erase operation is performed before the program one.
  * 
  * @param  Address: specifies the address to be programmed.
  *         This parameter can be any address in Program memory zone or in OTP zone.  
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_HALF_WORD;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint16_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation();

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Programs a byte (8-bit) at a specified address.
  * @note   This function can be used within all the device supply voltage ranges.  
  *
  * @note   If an erase and a program operations are requested simultaneously,    
  *         the erase operation is performed before the program one.
  * 
  * @param  Address: specifies the address to be programmed.
  *         This parameter can be any address in Program memory zone or in OTP zone.  
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_BYTE;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint8_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation();

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 

  /* Return the Program Status */
  return status;
}

/**
  * @}
  */


/** @defgroup FLASH_Group3 Option Bytes Programming functions
 *  @brief   Option Bytes Programming functions 
 *
@verbatim   
 ===============================================================================
                ##### Option Bytes Programming functions #####
 ===============================================================================  
    [..]
      This group includes the following functions:
      (+) void FLASH_OB_Unlock(void)
      (+) void FLASH_OB_Lock(void)
      (+) void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState)
      (+) void FLASH_OB_WRP1Config(uint32_t OB_WRP, FunctionalState NewState)  
      (+) void FLASH_OB_PCROPSelectionConfig(uint8_t OB_PCROPSelect)
      (+) void FLASH_OB_PCROPConfig(uint32_t OB_PCROP, FunctionalState NewState)
      (+) void FLASH_OB_PCROP1Config(uint32_t OB_PCROP, FunctionalState NewState) 
      (+) void FLASH_OB_RDPConfig(uint8_t OB_RDP)
      (+) void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY)
      (+) void FLASH_OB_BORConfig(uint8_t OB_BOR)
      (+) FLASH_Status FLASH_ProgramOTP(uint32_t Address, uint32_t Data)
      (+) FLASH_Status FLASH_OB_Launch(void)
      (+) uint32_t FLASH_OB_GetUser(void)
      (+) uint8_t FLASH_OB_GetWRP(void)
      (+) uint8_t FLASH_OB_GetWRP1(void)
      (+) uint8_t FLASH_OB_GetPCROP(void)
      (+) uint8_t FLASH_OB_GetPCROP1(void)
      (+) uint8_t FLASH_OB_GetRDP(void)
      (+) uint8_t FLASH_OB_GetBOR(void)
    [..]  
      The following function can be used only for STM32F42xxx/43xxx devices. 
      (+) void FLASH_OB_BootConfig(uint8_t OB_BOOT)
    [..]   
     Any operation of erase or program should follow these steps:
      (#) Call the FLASH_OB_Unlock() function to enable the FLASH option control 
          register access

      (#) Call one or several functions to program the desired Option Bytes:
        (++) void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState) 
             => to Enable/Disable the desired sector write protection
        (++) void FLASH_OB_RDPConfig(uint8_t OB_RDP) => to set the desired read 
             Protection Level
        (++) void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY) 
             => to configure the user Option Bytes.
        (++) void FLASH_OB_BORConfig(uint8_t OB_BOR) => to set the BOR Level 			 

      (#) Once all needed Option Bytes to be programmed are correctly written, 
          call the FLASH_OB_Launch() function to launch the Option Bytes 
          programming process.
     
      -@- When changing the IWDG mode from HW to SW or from SW to HW, a system 
          reset is needed to make the change effective.  

      (#) Call the FLASH_OB_Lock() function to disable the FLASH option control 
          register access (recommended to protect the Option Bytes against 
          possible unwanted operations)
    
@endverbatim
  * @{
  */

/**
  * @brief  Unlocks the FLASH Option Control Registers access.
  * @param  None
  * @retval None
  */
void FLASH_OB_Unlock(void)
{
  if((FLASH->OPTCR & FLASH_OPTCR_OPTLOCK) != RESET)
  {
    /* Authorizes the Option Byte register programming */
    FLASH->OPTKEYR = FLASH_OPT_KEY1;
    FLASH->OPTKEYR = FLASH_OPT_KEY2;
  }  
}

/**
  * @brief  Locks the FLASH Option Control Registers access.
  * @param  None
  * @retval None
  */
void FLASH_OB_Lock(void)
{
  /* Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access */
  FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
}

/**
  * @brief  Enables or disables the write protection of the desired sectors, for the first
  *         1 Mb of the Flash  
  *
  * @note   When the memory read protection level is selected (RDP level = 1), 
  *         it is not possible to program or erase the flash sector i if CortexM4  
  *         debug features are connected or boot code is executed in RAM, even if nWRPi = 1 
  * @note   Active value of nWRPi bits is inverted when PCROP mode is active (SPRMOD =1).   
  * 
  * @param  OB_WRP: specifies the sector(s) to be write protected or unprotected.
  *          This parameter can be one of the following values:
  *            @arg OB_WRP: A value between OB_WRP_Sector0 and OB_WRP_Sector11                      
  *            @arg OB_WRP_Sector_All
  * @param  Newstate: new state of the Write Protection.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None  
  */
void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState)
{ 
  FLASH_Status status = FLASH_COMPLETE;
  
  /* Check the parameters */
  assert_param(IS_OB_WRP(OB_WRP));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
    
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  { 
    if(NewState != DISABLE)
    {
      /*Write protection enabled on sectors */
      FLASH->OPTCR &= (~OB_WRP);
    }
    else
    {
      /* Write protection disabled on sectors */
      FLASH->OPTCR |= (OB_WRP); 
    }
  }
}

/**
  * @brief  Sets the read protection level.
  * @param  OB_RDP: specifies the read protection level.
  *          This parameter can be one of the following values:
  *            @arg OB_RDP_Level_0: No protection
  *            @arg OB_RDP_Level_1: Read protection of the memory
  *            @arg OB_RDP_Level_2: Full chip protection
  *   
  * /!\ Warning /!\ When enabling OB_RDP level 2 it's no more possible to go back to level 1 or 0
  *    
  * @retval None
  */
void FLASH_OB_RDPConfig(uint8_t OB_RDP)
{
  FLASH_Status status = FLASH_COMPLETE;
  uint32_t tmpreg;
  
  /* Check the parameters */
  assert_param(IS_OB_RDP(OB_RDP));

  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  {
    tmpreg = FLASH->OPTCR;
    tmpreg &= ~FLASH_OPTCR_RDP;
    tmpreg |= (OB_RDP << 8);
    FLASH->OPTCR = tmpreg;
  }
}

/**
  * @brief  Program the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY.    
  * @param  Wwdg: Selects the IWDG mode
  *          This parameter can be one of the following values:
  *            @arg OB_WWDG_SW: Software WWDG selected
  *            @arg OB_WWDG_HW: Hardware WWDG selected
  * @param  Iwdg: Selects the WWDG mode
  *          This parameter can be one of the following values:
  *            @arg OB_IWDG_SW: Software IWDG selected
  *            @arg OB_IWDG_HW: Hardware IWDG selected
  * @param  Stop: Reset event when entering STOP mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STOP_NO_RST: No reset generated when entering in STOP
  *            @arg OB_STOP_RST: Reset generated when entering in STOP
  * @param  Stdby: Reset event when entering Standby mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STDBY_NO_RST: No reset generated when entering in STANDBY
  *            @arg OB_STDBY_RST: Reset generated when entering in STANDBY
  * @param  Iwdgstop: Independent watchdog counter freeze in Stop mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_IWDG_STOP_FREEZE: Freeze IWDG counter in STOP
  *            @arg OB_IWDG_STOP_ACTIVE: IWDG counter active in STOP
  * @param  Iwdgstdby: Independent watchdog counter freeze in standby mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_IWDG_STDBY_FREEZE: Freeze IWDG counter in STANDBY
  *            @arg OB_IWDG_STDBY_ACTIVE: IWDG counter active in STANDBY           
  * @retval HAL Status
  */
void FLASH_OB_UserConfig(uint32_t Wwdg, uint32_t Iwdg, uint32_t Stop, uint32_t Stdby, uint32_t Iwdgstop, uint32_t Iwdgstdby )
{
  uint32_t useroptionmask = 0x00;
  uint32_t useroptionvalue = 0x00;

  FLASH_Status status = FLASH_COMPLETE; 

  /* Check the parameters */
  assert_param(IS_OB_WWDG_SOURCE(Wwdg));
  assert_param(IS_OB_IWDG_SOURCE(Iwdg));
  assert_param(IS_OB_STOP_SOURCE(Stop));
  assert_param(IS_OB_STDBY_SOURCE(Stdby));
  assert_param(IS_OB_IWDG_STOP_SOURCE(Iwdgstop));
  assert_param(IS_OB_IWDG_STDBY_SOURCE(Iwdgstdby));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    useroptionmask = (FLASH_OPTCR_WWDG_SW | FLASH_OPTCR_IWDG_SW | FLASH_OPTCR_nRST_STOP | \
                      FLASH_OPTCR_nRST_STDBY | FLASH_OPTCR_IWDG_STOP | FLASH_OPTCR_IWDG_STDBY);
                      
    useroptionvalue = (Iwdg | Wwdg | Stop | Stdby | Iwdgstop | Iwdgstdby);
        
    /* Update User Option Byte */               
    MODIFY_REG(FLASH->OPTCR, useroptionmask, useroptionvalue);
  }
}

/**
  * @brief  Sets the BOR Level. 
  * @param  OB_BOR: specifies the Option Bytes BOR Reset Level.
  *          This parameter can be one of the following values:
  *            @arg OB_BOR_LEVEL3: Supply voltage ranges from 2.7 to 3.6 V
  *            @arg OB_BOR_LEVEL2: Supply voltage ranges from 2.4 to 2.7 V
  *            @arg OB_BOR_LEVEL1: Supply voltage ranges from 2.1 to 2.4 V
  *            @arg OB_BOR_OFF: Supply voltage ranges from 1.62 to 2.1 V
  * @retval None
  */
void FLASH_OB_BORConfig(uint8_t OB_BOR)
{
  /* Check the parameters */
  assert_param(IS_OB_BOR(OB_BOR));
  
  /* Set the BOR Level */
  MODIFY_REG(FLASH->OPTCR, FLASH_OPTCR_BOR_LEV, OB_BOR);
}

/**
  * @brief  Configure Boot base address.
  * 
  * @param   BootOption : specifies Boot base address depending from Boot pin = 0 or pin = 1
  *          This parameter can be one of the following values:
  *            @arg OPTIONBYTE_BOOTADDR_0 : Boot address based when Boot pin = 0                 
  *            @arg OPTIONBYTE_BOOTADDR_1 : Boot address based when Boot pin = 1  
  * @param   Address: specifies Boot base address
  *          This parameter can be one of the following values:
  *            @arg OB_BOOTADDR_ITCM_RAM : Boot from ITCM RAM (0x00000000)                 
  *            @arg OB_BOOTADDR_SYSTEM : Boot from System memory bootloader (0x00100000) 
  *            @arg OB_BOOTADDR_ITCM_FLASH : Boot from Flash on ITCM interface (0x00200000)  
  *            @arg OB_BOOTADDR_AXIM_FLASH : Boot from Flash on AXIM interface (0x08000000)  
  *            @arg OB_BOOTADDR_DTCM_RAM : Boot from DTCM RAM (0x20000000)                 
  *            @arg OB_BOOTADDR_SRAM1 : Boot from SRAM1 (0x20010000)                    
  *            @arg OB_BOOTADDR_SRAM2 : Boot from SRAM2 (0x2004C000)              
  *    
  * @retval HAL Status
  */
FLASH_Status FLASH_OB_BootAddressConfig(uint32_t BootOption, uint32_t Address)
{
  FLASH_Status status = FLASH_COMPLETE;
  
  /* Check the parameters */
  assert_param(IS_OB_BOOT_ADDRESS(Address));
  assert_param(IS_OB_BOOT_OPTION(BootOption));
    
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  {
    if(BootOption == OPTIONBYTE_BOOTADDR_0)
    {
      MODIFY_REG(FLASH->OPTCR1, FLASH_OPTCR1_BOOT_ADD0, Address);
    }
    else
    {
      MODIFY_REG(FLASH->OPTCR1, FLASH_OPTCR1_BOOT_ADD1, (Address << 16));
    }
  }
  
  return status;
}

/**
  * @brief  Launch the option byte loading.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_OB_Launch(void)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Set the OPTSTRT bit in OPTCR register */
  SET_BIT( FLASH->OPTCR, FLASH_OPTCR_OPTSTRT );
  
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation();

  return status;
}

/**
  * @brief  Return the FLASH User Option Byte value.
  * @retval uint32_t FLASH User Option Bytes values: IWDG_SW(Bit0), RST_STOP(Bit1)
  *         and RST_STDBY(Bit2).
  */
uint32_t FLASH_OB_GetUser(void)
{
  /* Return the User Option Byte */
  return ((uint32_t)(FLASH->OPTCR & 0xC00000F0));
}

/**
  * @brief  Return the FLASH Write Protection Option Bytes value.
  * @retval uint32_t FLASH Write Protection Option Bytes value
  */
uint32_t FLASH_OB_GetWRP(void)
{
  /* Return the FLASH write protection Register value */
  return ((uint32_t)(FLASH->OPTCR & 0x00FF0000));
}

/**
  * @brief  Returns the FLASH Read Protection level.
  * @retval FlagStatus FLASH ReadOut Protection Status:
  *         This parameter can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Read protection of the memory
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  */
uint8_t FLASH_OB_GetRDP(void)
{
  uint8_t readstatus = OB_RDP_Level_0;
  
  if (((FLASH->OPTCR & FLASH_OPTCR_RDP) >> 8) == OB_RDP_Level_0)
  {
    readstatus = OB_RDP_Level_0;
  }
  else if (((FLASH->OPTCR & FLASH_OPTCR_RDP) >> 8) == OB_RDP_Level_2)
  {
    readstatus = OB_RDP_Level_2;
  }
  else 
  {
    readstatus = OB_RDP_Level_1;
  }

  return readstatus;
}

/**
  * @brief  Returns the FLASH BOR level.
  * @retval uint32_t The FLASH BOR level:
  *           - OB_BOR_LEVEL3: Supply voltage ranges from 2.7 to 3.6 V
  *           - OB_BOR_LEVEL2: Supply voltage ranges from 2.4 to 2.7 V
  *           - OB_BOR_LEVEL1: Supply voltage ranges from 2.1 to 2.4 V
  *           - OB_BOR_OFF   : Supply voltage ranges from 1.62 to 2.1 V  
  */
uint32_t FLASH_OB_GetBOR(void)
{
  /* Return the FLASH BOR level */
  return ((uint32_t)(FLASH->OPTCR & 0x0C));
}

/** @defgroup FLASH_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
              ##### Interrupts and flags management functions #####
 ===============================================================================  
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified FLASH interrupts.
  * @param  FLASH_IT: specifies the FLASH interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg FLASH_IT_ERR: FLASH Error Interrupt
  *            @arg FLASH_IT_EOP: FLASH end of operation Interrupt
  * @retval None 
  */
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FLASH_IT(FLASH_IT)); 
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState != DISABLE)
  {
    /* Enable the interrupt sources */
    FLASH->CR |= FLASH_IT;
  }
  else
  {
    /* Disable the interrupt sources */
    FLASH->CR &= ~(uint32_t)FLASH_IT;
  }
}

/**
  * @brief  Checks whether the specified FLASH flag is set or not.
  * @param  FLASH_FLAG: specifies the FLASH flag to check.
  *          This parameter can be one of the following values:
  *            @arg FLASH_FLAG_EOP: FLASH End of Operation flag 
  *            @arg FLASH_FLAG_OPERR: FLASH operation Error flag 
  *            @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag 
  *            @arg FLASH_FLAG_PGAERR: FLASH Programming Alignment error flag 
  *            @arg FLASH_FLAG_PGPERR: FLASH Programming Parallelism error flag
  *            @arg FLASH_FLAG_ERSERR: FLASH Erasing Sequence error flag
  *            @arg FLASH_FLAG_BSY: FLASH Busy flag
  * @retval The new state of FLASH_FLAG (SET or RESET).
  */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_FLASH_GET_FLAG(FLASH_FLAG));

  if((FLASH->SR & FLASH_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the new state of FLASH_FLAG (SET or RESET) */
  return bitstatus; 
}

/**
  * @brief  Clears the FLASH's pending flags.
  * @param  FLASH_FLAG: specifies the FLASH flags to clear.
  *          This parameter can be any combination of the following values:
  *            @arg FLASH_FLAG_EOP: FLASH End of Operation flag 
  *            @arg FLASH_FLAG_OPERR: FLASH operation Error flag 
  *            @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag 
  *            @arg FLASH_FLAG_PGAERR: FLASH Programming Alignment error flag 
  *            @arg FLASH_FLAG_PGPERR: FLASH Programming Parallelism error flag
  *            @arg FLASH_FLAG_ERSERR: FLASH Erasing Sequence error flag
  * @retval None
  */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
  /* Check the parameters */
  assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG));
  
  /* Clear the flags */
  FLASH->SR = FLASH_FLAG;
}

/**
  * @brief  Returns the FLASH Status.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_RD, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_GetStatus(void)
{
  FLASH_Status flashstatus = FLASH_COMPLETE;
  
  if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
  {
    flashstatus = FLASH_BUSY;
  }
  else 
  {  
    if((FLASH->SR & FLASH_FLAG_WRPERR) != (uint32_t)0x00)
    { 
      flashstatus = FLASH_ERROR_WRP;
    }
    else
    {
      if((FLASH->SR & FLASH_FLAG_ERSERR) != (uint32_t)0x00)
      { 
        flashstatus = FLASH_ERROR_ERS;
      }
      else 
      {
        if((FLASH->SR & (uint32_t)0xE0) != (uint32_t)0x00)
        {
          flashstatus = FLASH_ERROR_PROGRAM; 
        }
        else
        {
          if((FLASH->SR & FLASH_FLAG_OPERR) != (uint32_t)0x00)
          {
            flashstatus = FLASH_ERROR_OPERATION;
          }
          else
          {
            flashstatus = FLASH_COMPLETE;
          }
        }
      }
    }
  }
  /* Return the FLASH Status */
  return flashstatus;
}

/**
  * @brief  Waits for a FLASH operation to complete.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
FLASH_Status FLASH_WaitForLastOperation(void)
{ 
  __IO FLASH_Status status = FLASH_COMPLETE;
   
  /* Check for the FLASH Status */
  status = FLASH_GetStatus();

  /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
     Even if the FLASH operation fails, the BUSY flag will be reset and an error
     flag will be set */
  while(status == FLASH_BUSY)
  {
    status = FLASH_GetStatus();
  }
  /* Return the operation status */
  return status;
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

/**
  * @}
  */