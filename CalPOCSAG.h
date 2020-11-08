/*
 *   Copyright (C) 2019 by Florian Wolters DF2ET
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

#if defined(MODE_POCSAG)

#if !defined(CALPOCSAG_H)
#define  CALPOCSAG_H

enum POCSAGCAL {
  POCSAGCAL_IDLE,
  POCSAGCAL_TX
};

class CCalPOCSAG {
public:
  CCalPOCSAG();

  void process();

  uint8_t write(const uint8_t* data, uint16_t length);

private:
  POCSAGCAL m_state;
};

#endif

#endif

