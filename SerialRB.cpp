/*
Serial RB control - Copyright (C) KI6ZUM 2015
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

#include "SerialRB.h"

CSerialRB::CSerialRB(uint16_t length) :
m_length(length),
m_buffer(NULL),
m_head(0U),
m_tail(0U),
m_full(false)
{
  m_buffer = new uint8_t[length];
}

void CSerialRB::reset()
{
  m_head = 0U;
  m_tail = 0U;
  m_full = false;
}

uint16_t CSerialRB::getSpace() const
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

uint16_t CSerialRB::getData() const
{
  if (m_tail == m_head)
    return m_full ? m_length : 0U;
  else if (m_tail < m_head)
    return m_head - m_tail;
  else
    return m_length - m_tail + m_head;
}

bool CSerialRB::put(uint8_t c)
{
  if (m_full)
    return false;

  m_buffer[m_head] = c;

  m_head++;
  if (m_head >= m_length)
    m_head = 0U;

  if (m_head == m_tail)
    m_full = true;

  return true;
}

uint8_t CSerialRB::peek() const
{
  return m_buffer[m_tail];
}

uint8_t CSerialRB::get()
{
  uint8_t value = m_buffer[m_tail];

  m_full = false;

  m_tail++;
  if (m_tail >= m_length)
    m_tail = 0U;

  return value;
}

