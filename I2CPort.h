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

#if !defined(I2CPORT_H)
#define  I2CPORT_H

#include <cstdint>

const uint16_t I2C_TX_FIFO_SIZE = 512U;


class CI2CPort {
public:
  CI2CPort(uint8_t n);

  bool init();

  uint8_t write(const uint8_t* data, uint8_t length);

  void eventHandler();

private:
  uint8_t m_n;
  bool    m_ok;
  volatile uint8_t  m_fifo[I2C_TX_FIFO_SIZE];
  volatile uint16_t m_fifoHead;
  volatile uint16_t m_fifoTail;

  uint16_t fifoLevel();
  bool     fifoPut(uint8_t next);
  void     clearFlag();
};

#endif

#endif

