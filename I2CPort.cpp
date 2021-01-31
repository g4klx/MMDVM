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
  I2C_InitStructure.I2C_Timing              = 50000U;  //400kHz (Fast Mode)
  I2C_InitStructure.I2C_AnalogFilter        = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter       = 7U;
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1         = 0x00U;		//We are the master. We don't need this
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

  // Generate a start condition. (As soon as the line becomes idle, a Start condition will be generated)
  m_port->CR2 |= I2C_CR2_START;
  
  // Set I2C device address if needed
  if (addr != m_addr) {
    bool ret = setAddr(addr, I2C_Direction_Transmitter);
    if (!ret)
      return 7U;

    m_addr = addr;
  }

  // Unstretch the clock by just reading ISR (Physically the clock is continued to be strectehed because we have not written anything to the TXDR yet.)
  (void)m_port->ISR; 
  
  // Start Writing Data
  while (length--) {
    bool ret = write(*data++);
    if (!ret)
      return 7U;
  }
  
  // Wait for the data on the shift register to be transmitted completely
  bool ret = waitISRFlagsSet(I2C_ISR_TC);
  if (!ret)
    return 7U;

  // Here TXE=BTF=1. Therefore the clock stretches again.
  
  // Order a stop condition at the end of the current tranmission (or if the clock is being streched, generate stop immediatelly)
  m_port->CR2 |= I2C_CR2_STOP;

  // Stop condition resets the TXE and BTF automatically.
  
  // Wait to be sure that line is iddle
  ret = waitLineIdle();  
  if (!ret)
    return 7U;

  return 0U;
}

bool CI2CPort::write(uint8_t c)
{
  // Write the byte to the TXDR
  m_port->TXDR = c;
  
  // Wait till the content of TXDR is transferred to the shift Register.
  return waitISRFlagsSet(I2C_ISR_TXE);
}

bool CI2CPort::setAddr(uint8_t addr, uint8_t dir)
{
  // Write address to the TXDR (to the bus)
  m_port->TXDR = (addr << 1) | dir;
  
  // Wait till ADDR is set (ADDR is set when the slave sends ACK to the address).
  // Clock streches till ADDR is Reset. To reset the hardware i)Read the SR1 ii)Wait till ADDR is Set iii)Read SR2
  // Note1:Spec_p602 recommends the waiting operation
  return waitISRFlagsSet(I2C_ISR_ADDR); 
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

bool CI2CPort::waitLineIdle()
{
  // Wait till the Line becomes idle.
  uint32_t timeOut = HSI_VALUE;

  // Check to see if the Line is busy
  // This bit is set automatically when a start condition is broadcast on the line (even from another master)
  // and is reset when stop condition is detected.
  while (m_port->ISR & I2C_ISR_BUSY) {
    if (!(timeOut--))
      return false;
  }
  
  return true;
}

#endif

