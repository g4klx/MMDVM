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

#if !defined(I2COLED_H)
#define  I2COLED_H

#include "I2C3.h"

class CI2COLED {
public:
  CI2COLED();

  void init();

  void setMode(int state);

private:
  CI2C3    m_i2c;
  uint8_t* m_oledBuffer;

  void write(const char* text);
  void write(uint8_t x, uint8_t y, char c, uint8_t size);
  void drawPixel(uint8_t x, uint8_t y);
  void fillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
  void clear();
  void display();

  void sendCommand(uint8_t c);
  void sendCommand(uint8_t c0, uint8_t c1);
  void sendData(const uint8_t* c, uint16_t length);
};

#endif

#endif

