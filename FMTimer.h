/*
 *   Copyright (C) 2009,2010,2015,2020 by Jonathan Naylor G4KLX
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

#if !defined(FMTimer_H)
#define  FMTimer_H

class CFMTimer {
public:
  CFMTimer();

  void setTimeout(uint16_t secs, uint32_t msecs);

  uint32_t getTimeout() const;

  void start();
  
  void stop();

  void clock(uint8_t length);

  bool isRunning() const;
  
  bool hasExpired() const;

private:
  uint32_t m_timeout;
  uint32_t m_timer;
};

#endif

#endif

