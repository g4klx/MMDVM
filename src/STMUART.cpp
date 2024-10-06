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

#include "STMUART.h"

CSTMUART::CSTMUART() :
m_usart(NULL)
{

}

void CSTMUART::init(USART_TypeDef* usart)
{
  m_usart = usart;
}

void CSTMUART::write(const uint8_t * data, uint16_t length)
{
  if(length == 0U || m_usart == NULL)
    return;


  m_txFifo.put(data[0]);
  USART_ITConfig(m_usart, USART_IT_TXE, ENABLE);//switch TX IRQ is on

  for(uint16_t i = 1U; i < length; i++) {
    m_txFifo.put(data[i]);
  }

  USART_ITConfig(m_usart, USART_IT_TXE, ENABLE);//make sure TX IRQ is on
}

uint8_t CSTMUART::read()
{
  return m_rxFifo.get();
}

void CSTMUART::handleIRQ()
{
  if(m_usart == NULL)
    return;

  if (USART_GetITStatus(m_usart, USART_IT_RXNE)) {
    if(!m_rxFifo.isFull())
      m_rxFifo.put((uint8_t) USART_ReceiveData(m_usart));
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }

  if (USART_GetITStatus(m_usart, USART_IT_TXE)) {
    if(!m_txFifo.isEmpty())
      USART_SendData(m_usart, m_txFifo.get());

    USART_ClearITPendingBit(m_usart, USART_IT_TXE);

    if (m_txFifo.isEmpty()) // if there's no more data to transmit then turn off TX interrupts
      USART_ITConfig(m_usart, USART_IT_TXE, DISABLE);
  }
}

  // Flushes the transmit shift register
// warning: this call is blocking
void CSTMUART::flush()
{
  if(m_usart == NULL)
    return;

   // wait until the TXE shows the shift register is empty
   while (USART_GetITStatus(m_usart, USART_FLAG_TXE))
      ;
}

uint16_t CSTMUART::available()
{
  return m_rxFifo.isEmpty() ? 0U : 1U;
}

uint16_t CSTMUART::availableForWrite()
{
  return m_txFifo.isFull() ? 0U : 1U;
}

#endif