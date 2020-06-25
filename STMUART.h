/*
 *   Copyright (c) 2020 by Jonathan Naylor G4KLX
 *   Copyright (c) 2020 by Geoffrey Merck F4FXL - KC3FRA   
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

#if defined(STM32F4XX) || defined(STM32F7XX)
#if !defined(STMUART_H)
#define STMUART_H

#if defined(STM32F4XX)
#include "stm32f4xx.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#endif

const uint16_t BUFFER_SIZE = 2048U; //needs to be a power of 2 !
const uint16_t BUFFER_MASK = BUFFER_SIZE - 1;

class CSTMUARTFIFO {
public:
  CSTMUARTFIFO() :
  m_head(0U),
  m_tail(0U)
  {
  }

  void put(uint8_t data)
  {
    m_buffer[BUFFER_MASK & (m_head++)] = data;
  }

  uint8_t get()
  {
    return m_buffer[BUFFER_MASK & (m_tail++)];
  }

  void reset()
  {
    m_tail = 0U;
    m_head = 0U;
  }

  bool isEmpty()
  {
    return m_tail == m_head;
  }

  bool isFull()
  {
    return ((m_head + 1U) & BUFFER_MASK) == (m_tail & BUFFER_MASK);
  }

private:
  volatile uint8_t  m_buffer[BUFFER_SIZE];
  volatile uint16_t m_head;
  volatile uint16_t m_tail;
};

class CSTMUART {
public:
  CSTMUART();
  void init(USART_TypeDef* usart);
  void write(const uint8_t * data, uint16_t length);
  uint8_t read();
  void handleIRQ();
  void flush();
  uint16_t available();
  uint16_t availableForWrite();

private:
  USART_TypeDef * m_usart;
  CSTMUARTFIFO    m_rxFifo;
  CSTMUARTFIFO    m_txFifo;
};

#endif
#endif