/*
 *   Copyright (C) 2009-2015,2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
 *   Copyright (C) 2020 by Phil Taylor M0VSE
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

#if !defined(CALFM_H)
#define  CALFM_H

class CCalFM {
public:
  CCalFM();

  void process();
  void fm10kcal();
  void fm12k5cal();
  void fm15kcal();
  void fm20kcal();
  void fm25kcal();
  void fm30kcal();

  uint8_t write(const uint8_t* data, uint16_t length);

private:
  uint16_t  m_frequency;
  uint16_t  m_length;
  q15_t*    m_tone;
  q15_t     m_level;
  bool      m_transmit;
  uint8_t   m_audioSeq;
  uint8_t   m_lastState;
};

#endif

#endif

