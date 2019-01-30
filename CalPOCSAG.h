/*
 *   Copyright (C) 2019 by Florian Wolters DF2ET
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

#if !defined(CALPOCSAG_H)
#define  CALPOCSAG_H

#include "Config.h"

enum POCSAGCAL {
  POCSAGCAL_IDLE,
  POCSAGCAL_TX
};

class CCalPOCSAG {
public:
  CCalPOCSAG();

  void process();

  uint8_t write(const uint8_t* data, uint8_t length);

private:
  POCSAGCAL m_state;
};

#endif
