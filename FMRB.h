/*
Serial fifo control - Copyright (C) KI6ZUM 2015
Copyright (C) 2015,2016,2020 by Jonathan Naylor G4KLX

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

#if !defined(FMRB_H)
#define  FMRB_H

#if defined(STM32F4XX)
#include "stm32f4xx.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#elif defined(STM32F105xC)
#include "stm32f1xx.h"
#include <cstddef>
#else
#include <Arduino.h>
#endif

#if defined(__SAM3X8E__) || defined(STM32F105xC)
#define  ARM_MATH_CM3
#elif defined(STM32F7XX)
#define  ARM_MATH_CM7
#elif defined(STM32F4XX) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define  ARM_MATH_CM4
#else
#error "Unknown processor type"
#endif

#include <arm_math.h>

class CFMRB {
public:
  CFMRB(uint16_t length);
  
  uint16_t getSpace() const;
  
  uint16_t getData() const;

  bool put(q15_t sample);

  bool get(q15_t& sample);

  bool hasOverflowed();

  void reset();

private:
  uint16_t          m_length;
  volatile q15_t*   m_samples;
  volatile uint16_t m_head;
  volatile uint16_t m_tail;
  volatile bool     m_full;
  bool              m_overflow;
};

#endif
