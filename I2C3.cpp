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

CI2C3::CI2C3()
{
}

void CI2C3::init()
{
  // Enable I2C3
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);

  // Reset the Peripheral
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C3, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C3, DISABLE);
  
  // Enable the GPIOs for the SCL/SDA Pins
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
  
  // Configure and initialize the GPIOs
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
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
}

void CI2C3::write(uint8_t addr, const uint8_t* data, uint16_t length)
{
  DEBUG2("OLED Data", addr);
  DEBUG_DUMP(data, length);

  // Wait for the I2C transmitter to become free
  waitISRFlagsSet(I2C_ISR_BUSY);

  // Configure the data transfer
  configureDataTransfer(length, addr);

  // Start Writing Data
  for (uint16_t i = 0U; i < length; i++) {
    // Wait for the TXIS flag to be set
    waitISRFlagsSet(I2C_ISR_TXIS);

    // Write the byte to the TXDR
    I2C3->TXDR = data[i];
  }

  waitISRFlagsSet(I2C_ISR_STOPF);

  I2C3->ISR &= ~I2C_ISR_STOPF;
  
  I2C3->CR2 &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
}

void CI2C3::waitISRFlagsSet(uint32_t flags)
{
  // Wait till the specified ISR Bits are set
  // More than 1 Flag can be "or"ed.
  while ((I2C3->ISR & flags) != flags)
  	;
}

void CI2C3::configureDataTransfer(uint8_t size, uint8_t addr)
{
  I2C3->CR2 &= ~(I2C_CR2_SADD    |
                   I2C_CR2_NBYTES  |
                   I2C_CR2_RELOAD  |
                   I2C_CR2_AUTOEND |
                   (I2C_CR2_RD_WRN & (uint32_t)(I2C_Generate_Start_Write >> (31U - I2C_CR2_RD_WRN_Pos))) |
                   I2C_CR2_START   |
                   I2C_CR2_STOP);
  I2C3->CR2 |= (uint32_t)(((uint32_t)addr & I2C_CR2_SADD) |
                 (((uint32_t)size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) |
                 (uint32_t)I2C_CR2_RELOAD |
                 (uint32_t)I2C_Generate_Start_Write);
}

#endif

