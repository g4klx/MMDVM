/*
 *   Copyright (C) 2017 by Wojciech Krutnik N0CALL
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
#include "Globals.h"
#include "RingBuff.h"

#include "SerialPort.h"

/*
Pin definitions (configuration in IOSTM_CMSIS.c):

- Host communication:
USART2 - TXD PA2  - RXD PA3

*/

#if defined(STM32F1)

// BaudRate calculator macro
// source: STM32 HAL Library (stm32f1xx_hal_usart.h)
#define USART_DIV(__PCLK__, __BAUD__)     (((__PCLK__)*25)/(4*(__BAUD__)))
#define USART_DIVMANT(__PCLK__, __BAUD__) (USART_DIV((__PCLK__), (__BAUD__))/100)
#define USART_DIVFRAQ(__PCLK__, __BAUD__) (((USART_DIV((__PCLK__), (__BAUD__)) - (USART_DIVMANT((__PCLK__), (__BAUD__)) * 100)) * 16 + 50) / 100)
#define USART_BRR(__PCLK__, __BAUD__)     ((USART_DIVMANT((__PCLK__), (__BAUD__)) << 4)|(USART_DIVFRAQ((__PCLK__), (__BAUD__)) & 0x0F))

#define USART_BUFFER_SIZE 256U
DECLARE_RINGBUFFER_TYPE(USARTBuffer, USART_BUFFER_SIZE);

/* ************* USART2 ***************** */
static volatile USARTBuffer_t txBuffer2={.size=USART_BUFFER_SIZE};
static volatile USARTBuffer_t rxBuffer2={.size=USART_BUFFER_SIZE};


extern "C" {
  bitband_t txe = (bitband_t)BITBAND_PERIPH(&USART2->SR, USART_SR_TXE_Pos);
  bitband_t rxne = (bitband_t)BITBAND_PERIPH(&USART2->SR, USART_SR_RXNE_Pos);
  bitband_t txeie = (bitband_t)BITBAND_PERIPH(&USART2->CR1, USART_CR1_TXEIE_Pos);
  
  RAMFUNC void USART2_IRQHandler()
  {
    /* Transmitting data */
    if(*txe){
      if(!(RINGBUFF_EMPTY(txBuffer2))){
        USART2->DR = RINGBUFF_READ(txBuffer2);
      }else{  /* Buffer empty */
        *txeie = 0;  /* Don't send further data */
      }
    }

    /* Receiving data */
    if(*rxne){
      RINGBUFF_WRITE(rxBuffer2, USART2->DR);
    }
  }
}


void USART2Init(int speed)
{
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  
  USART2->BRR = USART_BRR(36000000UL, speed);
  
  USART2->CR1 = USART_CR1_UE | USART_CR1_TE |
                USART_CR1_RE | USART_CR1_RXNEIE;  // Enable USART and RX interrupt
  NVIC_EnableIRQ(USART2_IRQn);
}

RAMFUNC void USART2TxData(const uint8_t* data, uint16_t length)
{
  NVIC_DisableIRQ(USART2_IRQn);
  
  /* Check remaining space in buffer */
  if(RINGBUFF_COUNT(txBuffer2) + length > RINGBUFF_SIZE(txBuffer2)){
    NVIC_EnableIRQ(USART2_IRQn);
    return;
  }
  
  /* Append data to buffer  */
  while(length--){
    RINGBUFF_WRITE(txBuffer2, *(data++));
  }

  /* Start sending data */
  USART2->CR1 |= USART_CR1_TXEIE;

  NVIC_EnableIRQ(USART2_IRQn);
}


/////////////////////////////////////////////////////////////////

void CSerialPort::beginInt(uint8_t n, int speed)
{
  switch (n) {
    case 1U:
      USART2Init(speed);
      break;
  default:
    break;
  }
}

int CSerialPort::availableInt(uint8_t n)
{
  switch (n) {
    case 1U:
      return !RINGBUFF_EMPTY(rxBuffer2);
    default:
      return false;
  }
}

uint8_t CSerialPort::readInt(uint8_t n)
{
  switch (n) {
    case 1U:
      return RINGBUFF_READ(rxBuffer2);
    default:
      return 0U;
  }
}

void CSerialPort::writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush)
{
  bitband_t tc = (bitband_t)BITBAND_PERIPH(&USART2->SR, USART_SR_TC_Pos);
  
  switch (n) {
    case 1U:
      USART2TxData(data, length);
      *tc = 0;
      
      if (flush) {
        while (!RINGBUFF_EMPTY(txBuffer2) || !tc)
          ;
      }
      break;
    default:
      break;
  }
}

#endif
