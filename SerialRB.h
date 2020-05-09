/*
Serial fifo control - Copyright (C) KI6ZUM 2015
Copyright (C) 2015,2016 by Jonathan Naylor G4KLX

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
Boston, MA  02110-1301, USA.
*/

#if !defined(SERIALRB_H)
#define  SERIALRB_H

#include "RingBuffer.h"

const uint16_t SERIAL_RINGBUFFER_SIZE = 370U;

class CSerialRB : public CRingBuffer<uint8_t>{
public:
  CSerialRB(uint16_t length = SERIAL_RINGBUFFER_SIZE);

  uint8_t get();
};

#endif

