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
#include "Globals.h"
#include "FMCTCSSRX.h"

const struct CTCSS_TABLE {
  uint8_t frequency;
  q31_t   coeffDivTwo;
} CTCSS_TABLE_DATA[] = {
  { 67U, 2147475280},
  { 69U, 2147474696},
  { 71U, 2147474012},
  { 74U, 2147473330},
  { 77U, 2147472596},
  { 79U, 2147471807},
  { 82U, 2147470961},
  { 85U, 2147470053},
  { 88U, 2147469048},
  { 91U, 2147468042},
  { 94U, 2147466895},
  { 97U, 2147465964},
  {100U, 2147465007},
  {103U, 2147463679},
  {107U, 2147462226},
  {110U, 2147460722},
  {114U, 2147459081},
  {118U, 2147457339},
  {123U, 2147455446},
  {127U, 2147453440},
  {131U, 2147451266},
  {136U, 2147448916},
  {141U, 2147446430},
  {146U, 2147443804},
  {151U, 2147440919},
  {156U, 2147437875},
  {159U, 2147436046},
  {162U, 2147434605},
  {165U, 2147432590},
  {167U, 2147431098},
  {171U, 2147428948},
  {173U, 2147427340},
  {177U, 2147425049},
  {179U, 2147423318},
  {183U, 2147420879},
  {186U, 2147419018},
  {189U, 2147416424},
  {192U, 2147414356},
  {196U, 2147411597},
  {199U, 2147409456},
  {203U, 2147406451},
  {206U, 2147404158},
  {210U, 2147400892},
  {218U, 2147394977},
  {225U, 2147388689},
  {229U, 2147385807},
  {233U, 2147381925},
  {241U, 2147374659},
  {250U, 2147366861},
  {254U, 2147363288}};

const uint8_t CTCSS_TABLE_DATA_LEN = 50U;

CFMCTCSSRX::CFMCTCSSRX() :
m_coeffDivTwo(0),
m_threshold(0U)
{
}

uint8_t CFMCTCSSRX::setParams(uint8_t frequency, uint8_t threshold)
{
  for (uint8_t i = 0U; i < CTCSS_TABLE_DATA_LEN; i++) {
    if (CTCSS_TABLE_DATA[i].frequency == frequency) {
      m_coeffDivTwo = CTCSS_TABLE_DATA[i].coeffDivTwo;
      break;
    }
  }

  if (m_coeffDivTwo == 0)
    return 4U;

  m_threshold = threshold;

  return 0U;
}

bool CFMCTCSSRX::process(const q15_t* samples, uint8_t length)
{
  return false;
}
