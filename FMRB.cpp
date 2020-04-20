/*
TX fifo control - Copyright (C) KI6ZUM 2015
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

#include "FMRB.h"

CFMRB::CFMRB(uint16_t length) :
m_length(length),
m_head(0U),
m_tail(0U),
m_full(false),
m_overflow(false)
{
  m_samples = new q15_t[length];
}

uint16_t CFMRB::getSpace() const
{
  uint16_t n = 0U;

  if (m_tail == m_head)
    n = m_full ? 0U : m_length;
  else if (m_tail < m_head)
    n = m_length - m_head + m_tail;
  else
    n = m_tail - m_head;

  if (n > m_length)
    n = 0U;

  return n;
}

uint16_t CFMRB::getData() const
{
  if (m_tail == m_head)
    return m_full ? m_length : 0U;
  else if (m_tail < m_head)
    return m_head - m_tail;
  else
    return m_length - m_tail + m_head;
}

bool CFMRB::put(q15_t sample)
{
  if (m_full) {
    m_overflow = true;
    return false;
  }

  m_samples[m_head] = sample;

  m_head++;
  if (m_head >= m_length)
    m_head = 0U;

  if (m_head == m_tail)
    m_full = true;

  return true;
}

bool CFMRB::get(q15_t& sample)
{
  if (m_head == m_tail && !m_full)
    return false;

  sample = m_samples[m_tail];

  m_full = false;

  m_tail++;
  if (m_tail >= m_length)
    m_tail = 0U;

  return true;
}

bool CFMRB::hasOverflowed()
{
  bool overflow = m_overflow;

  m_overflow = false;

  return overflow;
}
