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

#if defined(MODE_OLED) || defined(I2C_REPEATER)

#include "I2CPort.h"

#include "Globals.h"

const uint16_t MAX_NBYTES_SIZE = 255U;

CI2CPort::CI2CPort(uint8_t n) :
m_port(NULL),
m_clock(0x00U),
m_ok(true),
m_addr(0x00U)
{
  switch (n) {
    case 1U:
      m_port         = I2C1;
      m_clock        = RCC_APB1Periph_I2C1;
      m_busSCL       = RCC_AHB1Periph_GPIOB;
      m_busSDA       = RCC_AHB1Periph_GPIOB;
      m_af           = GPIO_AF4_I2C1;
      m_gpioSCL      = GPIOB;
      m_gpioSDA      = GPIOB;
      m_pinSCL       = GPIO_Pin_8;
      m_pinSDA       = GPIO_Pin_9;
      m_pinSourceSCL = GPIO_PinSource8;
      m_pinSourceSDA = GPIO_PinSource9;
      break;
    case 3U:
      m_port         = I2C3;
      m_clock        = RCC_APB1Periph_I2C3;
      m_busSCL       = RCC_AHB1Periph_GPIOA;
      m_busSDA       = RCC_AHB1Periph_GPIOC;
      m_af           = GPIO_AF4_I2C3;
      m_gpioSCL      = GPIOA;
      m_gpioSDA      = GPIOC;
      m_pinSCL       = GPIO_Pin_8;
      m_pinSDA       = GPIO_Pin_9;
      m_pinSourceSCL = GPIO_PinSource8;
      m_pinSourceSDA = GPIO_PinSource9;
      break;
    default:
      m_ok = false;
      break;
  }
}

bool CI2CPort::init()
{
  if (!m_ok)
    return false;

  // Enable I2C
  RCC_APB1PeriphClockCmd(m_clock, ENABLE);

  // Reset the Peripheral
  RCC_APB1PeriphResetCmd(m_clock, ENABLE);
  RCC_APB1PeriphResetCmd(m_clock, DISABLE);
  
  // Enable the GPIOs for the SCL/SDA Pins
  RCC_AHB1PeriphClockCmd(m_busSCL | m_busSDA, ENABLE);
  
  // Configure and initialize the GPIOs
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = m_pinSCL;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(m_gpioSCL, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = m_pinSDA;
  GPIO_Init(m_gpioSDA, &GPIO_InitStructure);
  
  // Connect GPIO pins to peripheral
  GPIO_PinAFConfig(m_gpioSCL, m_pinSourceSCL, m_af);
  GPIO_PinAFConfig(m_gpioSDA, m_pinSourceSDA, m_af);
  
  // Configure and Initialize the I2C
  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_Timing              = 0x0010061AU;	// 400kHz (Fast Mode)
  I2C_InitStructure.I2C_AnalogFilter        = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter       = 0U;		// No digital filter
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1         = 0x00U;		// We are the master. We don't need this
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  
  // Initialize the Peripheral
  I2C_Init(m_port, &I2C_InitStructure);

  // I2C Peripheral Enable
  I2C_Cmd(m_port, ENABLE);

  m_ok = true;

  return true;
}

uint8_t CI2CPort::write(uint8_t addr, const uint8_t* data, uint16_t length)
{
  if (!m_ok)
    return 6U;

  // Wait for the I2C transmitter to become free
  if (waitISRFlagsSet(I2C_ISR_BUSY))
    return 6U;

  // Configure the data transfer
  uint16_t size;
  if (length > MAX_NBYTES_SIZE)
    size = MAX_NBYTES_SIZE;
  else
    size = length;
  configureDataTransfer(size);

  // Start Writing Data
  while (length > 0U) {
    // Wait for the TXIS flag to be set
    if (waitISRFlagsSet(I2C_ISR_TXIS))
      return 6U;

    // Write the byte to the TXDR
    m_port->TXDR = *data++;
    length--;
    size--;

    // Configure the data transfer
    if (size == 0U && length > 0U) {
      if (length > MAX_NBYTES_SIZE)
        size = MAX_NBYTES_SIZE;
      else
        size = length;
      configureDataTransfer(size);
    }
  }

  if (waitISRFlagsSet(I2C_ISR_STOPF))
    return 6U;

  m_port->ISR &= ~I2C_ISR_STOPF;
  
  m_port->CR2 &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));

  return 0U;
}

bool CI2CPort::waitISRFlagsSet(uint32_t flags)
{
  // Wait till the specified ISR Bits are set
  // More than 1 Flag can be "or"ed.
  uint32_t timeOut = HSI_VALUE;
  
  while ((m_port->ISR & flags) != flags) {
    if (!(timeOut--))
      return false;
  } 

  return true;
}

void CI2CPort::configureDataTransfer(uint8_t size)
{
  m_port->CR2 &= ~(I2C_CR2_SADD    |
                   I2C_CR2_NBYTES  |
                   I2C_CR2_RELOAD  |
                   I2C_CR2_AUTOEND |
                   (I2C_CR2_RD_WRN & (uint32_t)(I2C_Generate_Start_Write >> (31U - I2C_CR2_RD_WRN_Pos))) |
                   I2C_CR2_START   |
                   I2C_CR2_STOP);
  m_port->CR2 |= (uint32_t)(((uint32_t)m_addr & I2C_CR2_SADD) |
                 (((uint32_t)size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) |
                 (uint32_t)I2C_CR2_RELOAD |
                 (uint32_t)I2C_Generate_Start_Write);
}

#endif

