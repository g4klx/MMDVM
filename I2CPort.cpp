
/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX
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

#if defined(STM32F4XX)
#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#include "stm32f7xx_i2c.h"
#endif

#include "Globals.h"

const uint32_t I2C_CLK_FREQ = 100000U;	// XXX FIXME
const uint16_t I2C_ADDR     = 0U;		// XXX FIXME


extern "C" {
#if defined(I2C_REPEATER)
  void I2C1_EV_IRQHandler(void)
  {
    i2C1.eventHandler();
  }

  void I2C1_ER_IRQHandler(void)
  {
    if (I2C_GetITStatus(I2C1, I2C_IT_AF))
      I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
  }
#endif

#ifdef notdef
  void I2C2_EV_IRQHandler(void)
  {
    i2C2.eventHandler();
  }

  void I2C2_ER_IRQHandler(void)
  {
    if (I2C_GetITStatus(I2C2, I2C_IT_AF))
      I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
  }
#endif

#if defined(MODE_OLED)
  void I2C3_EV_IRQHandler(void)
  {
    i2C3.eventHandler();
  }

  void I2C3_ER_IRQHandler(void)
  {
    if (I2C_GetITStatus(I2C3, I2C_IT_AF))
      I2C_ClearITPendingBit(I2C3, I2C_IT_AF);
  }
#endif
}


CI2CPort::CI2CPort(uint8_t n) :
m_n(n),
m_ok(false),
m_fifo(),
m_fifoHead(0U),
m_fifoTail(0U)
{
}

bool CI2CPort::init()
{
  I2C_TypeDef* i2CPort = NULL;
  uint32_t i2CClock = 0U;
  uint8_t i2CIrq = 0U;

  switch (m_n) {
    case 1U:
      i2CPort  = I2C1;
      i2CIrq   = I2C1_EV_IRQn;
      i2CClock = RCC_APB1Periph_I2C1;
      break;
    case 2U:
      i2CPort  = I2C2;
      i2CIrq   = I2C2_EV_IRQn;
      i2CClock = RCC_APB1Periph_I2C2;
      break;
    case 3U:
      i2CPort  = I2C3;
      i2CIrq   = I2C3_EV_IRQn;
      i2CClock = RCC_APB1Periph_I2C3;
      break;
    default:
      return false;
  }

  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;

  RCC_AHB1PeriphClockCmd(i2CClock,             ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_AFIO,  ENABLE);

  // Configure I2C GPIOs
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // Configure the I2C event interrupt
  NVIC_InitStructure.NVIC_IRQChannel                   = i2CIrq;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 15;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Configure the I2C error interrupt
  NVIC_InitStructure.NVIC_IRQChannel = i2CIrq;
  NVIC_Init(&NVIC_InitStructure);

  // I2C configuration
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1         = I2C_ADDR << 1;
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed          = I2C_CLK_FREQ;

  // Enable I2C
  I2C_Cmd(i2CPort, ENABLE);
  // Apply I2C configuration
  I2C_Init(i2CPort, &I2C_InitStructure);

  I2C_ITConfig(i2CPort, I2C_IT_EVT, ENABLE);
  I2C_ITConfig(i2CPort, I2C_IT_BUF, ENABLE);
  I2C_ITConfig(i2CPort, I2C_IT_ERR, ENABLE);

  m_fifoHead = 0U;
  m_fifoTail = 0U;

  m_ok = true;

  return true;
}

uint8_t CI2CPort::write(const uint8_t* data, uint8_t length)
{
  if (!m_ok)
    return 6U;

  for (uint16_t i = 0U; i < length; i++)
    fifoPut(data[i]);

  return 0U;
}

void CI2CPort::eventHandler()
{
  I2C_TypeDef* i2CPort = NULL;

  switch (m_n) {
    case 1U:
      i2CPort = I2C1;
      break;
    case 2U:
      i2CPort = I2C2;
      break;
    case 3U:
      i2CPort = I2C3;
      break;
    default:
      return;
  }

  uint32_t event = I2C_GetLastEvent(i2CPort);

  switch (event) {
    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:
    case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:
      if (fifoLevel() > 0U) {
        I2C_SendData(i2CPort, m_fifo[m_fifoTail]);
        m_fifoTail++;
        if (m_fifoTail >= I2C_TX_FIFO_SIZE)
          m_fifoTail = 0U;
      } else
        I2C_SendData(i2CPort, 0U);
      break;

    case I2C_EVENT_SLAVE_STOP_DETECTED:
      clearFlag();
      break;
  }
}

void CI2CPort::clearFlag()
{
  I2C_TypeDef* i2CPort = NULL;

  switch (m_n) {
    case 1U:
      i2CPort = I2C1;
      break;
    case 2U:
      i2CPort = I2C2;
      break;
    case 3U:
      i2CPort = I2C3;
      break;
    default:
      return;
  }

  // Clear ADDR flag
  while((i2CPort->SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) {
    i2CPort->SR1;
    i2CPort->SR2;
  }

  // Clear STOPF flag
  while((i2CPort->SR1 & I2C_SR1_STOPF) == I2C_SR1_STOPF) {
    i2CPort->SR1;
    i2CPort->CR1 |= 0x1;
  }
}

uint16_t CI2CPort::fifoLevel()
{
  uint32_t tail = m_fifoTail;
  uint32_t head = m_fifoHead;

  if (tail > head)
    return I2C_TX_FIFO_SIZE + head - tail;
  else
    return head - tail;
}

bool CI2CPort::fifoPut(uint8_t next)
{
  if (fifoLevel() >= I2C_TX_FIFO_SIZE)
    return false;

  m_fifo[m_fifoHead] = next;

  m_fifoHead++;
  if (m_fifoHead >= I2C_TX_FIFO_SIZE)
    m_fifoHead = 0U;

  return true;
}

#endif

