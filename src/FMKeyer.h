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

#if defined(MODE_FM)

#if !defined(FMKeyer_H)
#define  FMKeyer_H

class CFMKeyer {
public:
  CFMKeyer();

  uint8_t setParams(const char* text, uint8_t speed, uint16_t frequency, uint8_t highLevel, uint8_t lowLevel);

  q15_t getHighAudio();
  q15_t getLowAudio();

  void start();
  void stop();

  bool isRunning() const;

  bool isWanted() const;

private:
  bool     m_wanted;
  uint8_t  m_poBuffer[1000U];
  uint16_t m_poLen;
  uint16_t m_poPos;
  uint16_t m_dotLen;
  uint16_t m_dotPos;
  bool*    m_audio;
  uint16_t m_audioLen;
  uint16_t m_audioPos;
  q15_t    m_highLevel;
  q15_t    m_lowLevel;
};

#endif

#endif

