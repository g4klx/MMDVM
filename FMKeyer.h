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

#if !defined(FMKeyer_H)
#define  FMKeyer_H

#include "Config.h"

class CFMKeyer {
public:
  CFMKeyer();

  void setParams(const char* text, uint8_t speed, uint16_t frequency, uint8_t level);

  void getAudio(q15_t* samples, uint8_t length);

  void start();
  void stop();

  bool isRunning() const;

private:
  q15_t m_level;
  bool  m_wanted;
  bool  m_running;
};

#endif
