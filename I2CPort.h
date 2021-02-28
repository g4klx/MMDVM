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

#if !defined(I2CPORT_H)
#define  I2CPORT_H

#if defined(STM32F7XX)
#include "stm32f7xx.h"
#include "stm32f7xx_i2c.h"
#endif


class CI2CPort {
public:
  CI2CPort(uint8_t n);

  bool init();

  uint8_t write(uint8_t addr, const uint8_t* data, uint16_t length);

private:
  I2C_TypeDef*  m_port;
  uint32_t      m_clock;
  uint32_t      m_busSCL;
  uint32_t      m_busSDA;
  uint8_t       m_af;
  GPIO_TypeDef* m_gpioSCL;
  GPIO_TypeDef* m_gpioSDA;
  uint32_t      m_pinSCL;
  uint32_t      m_pinSDA;
  uint16_t      m_pinSourceSCL;
  uint16_t      m_pinSourceSDA;
  bool          m_ok;
  uint8_t       m_addr;
  
  bool waitISRFlagsSet(uint32_t flags);
  void configureDataTransfer(uint8_t size);
};

#endif

#endif

