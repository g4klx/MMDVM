/*
 *   Copyright (C) 2020,2021 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Config.h"

#if defined(MODE_OLED)

#include "I2C3.h"

#include "Globals.h"


#define	I2C_WRITE_REG(REG, VAL)			((REG) = (VAL))
#define	I2C_READ_REG(REG)				((REG))

#define	I2C_CLEAR_FLAG(__FLAG__)			(((__FLAG__) == I2C_FLAG_TXE) ? (I2C3->ISR |= (__FLAG__)) : (I2C3->ICR = (__FLAG__)))
#define	I2C_GET_FLAG(__FLAG__)				((((I2C3->ISR) & (__FLAG__)) == (__FLAG__)) ? SET : RESET)

#define	I2C_RESET_CR2()				(I2C3->CR2 &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)))

#define	I2C_MODIFY_REG(REG, CLEARMASK, SETMASK)	I2C_WRITE_REG((REG), (((I2C_READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))


CI2C3::CI2C3()
{
}

void CI2C3::init()
{
  // Enable I2C3
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
  // LL_RCC_SetI2CClockSource(LL_RCC_I2C3_CLKSOURCE_PCLK1);
/*
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
  PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
*/

  // Enable the GPIOs for the SCL/SDA Pins
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
  // LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  // LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  // LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  // LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

/*
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
*/

  // Configure and initialize the GPIOs
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

/*
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
*/

  // LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);

  // Connect GPIO pins to I2C3
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF4_I2C3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF4_I2C3);
  
  // Configure and Initialize I2C3
  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_Timing              = 0x0010061AU;	// 400kHz (Fast Mode)
  I2C_InitStructure.I2C_AnalogFilter        = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter       = 0U;		// No digital filter
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1         = 0x00U;		// We are the master. We don't need this
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  
  // Initialize the Peripheral
  I2C_Init(I2C3, &I2C_InitStructure);

  // I2C Peripheral Enable
  I2C_Cmd(I2C3, ENABLE);

/*
  LL_I2C_EnableAutoEndMode(I2C3);
  LL_I2C_SetOwnAddress2(I2C3, 0, LL_I2C_OWNADDRESS2_NOMASK);
  LL_I2C_DisableOwnAddress2(I2C3);
  LL_I2C_DisableGeneralCall(I2C3);
  LL_I2C_EnableClockStretching(I2C3);

  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x0010061A;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C3, &I2C_InitStruct);
*/

/*
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x0010061A;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  // Configure Analogue filter
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  // Configure Digital filter
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
*/
}

void CI2C3::write(uint8_t addr, const uint8_t* data, uint8_t length)
{
  DEBUG2("OLED Data", addr);
  DEBUG_DUMP(data, length);

  // Wait for the I2C transmitter to become free
  while (I2C_GET_FLAG(I2C_FLAG_BUSY) == SET)
    ;

  // Configure the data transfer
  transferConfig(addr, length, I2C_CR2_AUTOEND, I2C_Generate_Start_Write);

  // Start Writing Data
  for (uint16_t i = 0U; i < length; i++) {
    // Wait for the TXIS flag to be set
    while (I2C_GET_FLAG(I2C_FLAG_TXIS) == RESET)
	  ;

    // Write the byte to the TXDR
    I2C3->TXDR = data[i];
  }

  // No need to Check TC flag, with AUTOEND mode the stop is automatically generated Wait until STOPF flag is set
  while (I2C_GET_FLAG(I2C_FLAG_STOPF) == RESET)
    ;

  // Clear STOP flag
  I2C_CLEAR_FLAG(I2C_FLAG_STOPF);

  // Clear Configuration Register 2
  I2C_RESET_CR2();
}

void CI2C3::transferConfig(uint16_t addr, uint8_t length, uint32_t mode, uint32_t request)
{
  // Update CR2 register
  I2C_MODIFY_REG(I2C3->CR2, ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | \
                (I2C_CR2_RD_WRN & (uint32_t)(request >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)), \
                (uint32_t)(((uint32_t)addr & I2C_CR2_SADD) |
                        (((uint32_t)length << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)mode | (uint32_t)request));
}

#endif

