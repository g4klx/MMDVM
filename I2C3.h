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

#if !defined(I2C3_H)
#define  I2C3_H

#if defined(STM32F7XX)
#include "stm32f7xx.h"
#include "stm32f7xx_i2c.h"
#endif


class CI2C3 {
public:
  CI2C3();

  void init();

  void write(uint8_t addr, const uint8_t* data, uint16_t length);

private:
  void waitISRFlagsSet(uint32_t flags);
  void configureDataTransfer(uint8_t size, uint8_t addr);
};

#endif

#endif

