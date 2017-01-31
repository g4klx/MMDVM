/*
 *   Copyright (C) 2009-2017 by Jonathan Naylor G4KLX
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

#define  WANT_DEBUG

#define  DUMP_SAMPLES

#include "Config.h"
#include "Globals.h"
#include "YSFRX.h"
#include "Utils.h"

const q15_t SCALING_FACTOR = 18750;      // Q15(0.55)

const uint8_t MAX_SYNC_SYMBOLS_ERRS = 2U;
const uint8_t MAX_SYNC_BYTES_ERRS   = 3U;

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])

const uint16_t NOENDPTR = 9999U;

const unsigned int MAX_SYNC_FRAMES = 4U + 1U;

CYSFRX::CYSFRX() :
m_state(YSFRXS_NONE),
m_bitBuffer(),
m_buffer(),
m_bitPtr(0U),
m_dataPtr(0U),
m_endPtr(NOENDPTR),
m_syncPtr(NOENDPTR),
m_minSyncPtr(NOENDPTR),
m_maxSyncPtr(NOENDPTR),
m_maxCorr(0),
m_lostCount(0U),
m_countdown(0U),
m_centre(),
m_centreVal(0),
m_centreBest(0),
m_threshold(),
m_thresholdVal(0),
m_thresholdBest(0),
m_averagePtr(0u),
m_rssiAccum(0U),
m_rssiCount(0U)
{
}

void CYSFRX::reset()
{
  m_state        = YSFRXS_NONE;
  m_dataPtr      = 0U;
  m_bitPtr       = 0U;
  m_maxCorr      = 0;
  m_averagePtr   = 0U;
  m_endPtr       = NOENDPTR;
  m_syncPtr      = NOENDPTR;
  m_minSyncPtr   = NOENDPTR;
  m_maxSyncPtr   = NOENDPTR;
  m_centreVal    = 0;
  m_thresholdVal = 0;
  m_lostCount    = 0U;
  m_countdown    = 0U;
  m_rssiAccum    = 0U;
  m_rssiCount    = 0U;
}

void CYSFRX::samples(const q15_t* samples, uint16_t* rssi, uint8_t length)
{
  for (uint8_t i = 0U; i < length; i++) {
    q15_t sample = samples[i];

    m_rssiAccum += rssi[i];
    m_rssiCount++;

    m_bitBuffer[m_bitPtr] <<= 1;
    if (sample < 0)
      m_bitBuffer[m_bitPtr] |= 0x01U;

    m_buffer[m_dataPtr] = sample;

    switch (m_state) {
    case YSFRXS_DATA:
      processData(sample);
      break;
    default:
      processNone(sample);
      break;
    }

    m_dataPtr++;
    if (m_dataPtr >= YSF_FRAME_LENGTH_SAMPLES)
      m_dataPtr = 0U;

    m_bitPtr++;
    if (m_bitPtr >= YSF_RADIO_SYMBOL_LENGTH)
      m_bitPtr = 0U;
  }
}

void CYSFRX::processNone(q15_t sample)
{
  bool ret = correlateSync();
  if (ret) {
    // On the first sync, start the countdown to the state change
    if (m_countdown == 0U) {
      m_rssiAccum = 0U;
      m_rssiCount = 0U;

      io.setDecode(true);
      io.setADCDetection(true);

      m_countdown = 5U;
    }
  }

  if (m_countdown > 0U)
    m_countdown--;

  if (m_countdown == 1U) {
    for (uint8_t i = 0U; i < 16U; i++) {
      m_centre[i]    = m_centreBest;
      m_threshold[i] = m_thresholdBest;
    }

    m_centreVal    = m_centreBest;
    m_thresholdVal = m_thresholdBest;
    m_averagePtr   = 0U;

    m_minSyncPtr = m_syncPtr + YSF_FRAME_LENGTH_SAMPLES - 1U;
    if (m_minSyncPtr >= YSF_FRAME_LENGTH_SAMPLES)
      m_minSyncPtr -= YSF_FRAME_LENGTH_SAMPLES;

    m_maxSyncPtr = m_syncPtr + 1U;
    if (m_maxSyncPtr >= YSF_FRAME_LENGTH_SAMPLES)
      m_maxSyncPtr -= YSF_FRAME_LENGTH_SAMPLES;

    m_state      = YSFRXS_DATA;
    m_countdown  = 0U;
  }
}

void CYSFRX::processData(q15_t sample)
{
  if (m_minSyncPtr < m_maxSyncPtr) {
    if (m_dataPtr >= m_minSyncPtr && m_dataPtr <= m_maxSyncPtr)
      correlateSync();
  } else {
    if (m_dataPtr >= m_minSyncPtr || m_dataPtr <= m_maxSyncPtr)
      correlateSync();
  }

  if (m_dataPtr == m_endPtr) {
    uint16_t ptr = m_endPtr + YSF_RADIO_SYMBOL_LENGTH + 1U;
    if (ptr >= YSF_FRAME_LENGTH_SAMPLES)
      ptr -= YSF_FRAME_LENGTH_SAMPLES;

    // Only update the centre and threshold if they are from a good sync
    if (m_lostCount == MAX_SYNC_FRAMES) {
      m_threshold[m_averagePtr] = m_thresholdBest;
      m_centre[m_averagePtr]    = m_centreBest;

      m_averagePtr++;
      if (m_averagePtr >= 16U)
        m_averagePtr = 0U;

      // Find the average centre and threshold values
      m_centreVal    = 0;
      m_thresholdVal = 0;
      for (uint8_t i = 0U; i < 16U; i++) {
        m_centreVal    += m_centre[i];
        m_thresholdVal += m_threshold[i];
      }
      m_centreVal    >>= 4;
      m_thresholdVal >>= 4;

      DEBUG4("YSFRX: sync found (best) pos/centre/threshold", m_syncPtr, m_centreBest, m_thresholdBest);
      DEBUG4("YSFRX: sync found pos/centre/threshold", m_syncPtr, m_centreVal, m_thresholdVal);

      m_minSyncPtr = m_syncPtr + YSF_FRAME_LENGTH_SAMPLES - 1U;
      if (m_minSyncPtr >= YSF_FRAME_LENGTH_SAMPLES)
        m_minSyncPtr -= YSF_FRAME_LENGTH_SAMPLES;

      m_maxSyncPtr = m_syncPtr + 1U;
      if (m_maxSyncPtr >= YSF_FRAME_LENGTH_SAMPLES)
        m_maxSyncPtr -= YSF_FRAME_LENGTH_SAMPLES;
    }

    uint8_t frame[YSF_FRAME_LENGTH_BYTES + 3U];
    samplesToBits(ptr, YSF_FRAME_LENGTH_SYMBOLS, frame, 8U, m_centreVal, m_thresholdVal);
#if defined(DUMP_SAMPLES)
    writeSync(ptr);
#endif

    // We've not seen a data sync for too long, signal RXLOST and change to RX_NONE
    m_lostCount--;
    if (m_lostCount == 0U) {
      DEBUG1("YSFRX: sync timed out, lost lock");

      io.setDecode(false);
      io.setADCDetection(false);

      serial.writeYSFLost();

      m_state     = YSFRXS_NONE;
      m_endPtr    = NOENDPTR;
      m_countdown = 0U;
      m_maxCorr   = 0;
    } else {
      frame[0U] = m_lostCount == (MAX_SYNC_FRAMES - 1U) ? 0x01U : 0x00U;

      writeRSSIData(frame);

      m_maxCorr = 0;
    }
  }
}

bool CYSFRX::correlateSync()
{
  if (countBits32((m_bitBuffer[m_bitPtr] & YSF_SYNC_SYMBOLS_MASK) ^ YSF_SYNC_SYMBOLS) <= MAX_SYNC_SYMBOLS_ERRS) {
    uint16_t ptr = m_dataPtr + YSF_FRAME_LENGTH_SAMPLES - YSF_SYNC_LENGTH_SAMPLES + YSF_RADIO_SYMBOL_LENGTH;
    if (ptr >= YSF_FRAME_LENGTH_SAMPLES)
      ptr -= YSF_FRAME_LENGTH_SAMPLES;

    q31_t corr = 0;
    q15_t max = -16000;
    q15_t min = 16000;

    uint32_t mask = 0x00080000U;
    for (uint8_t i = 0U; i < YSF_SYNC_LENGTH_SYMBOLS; i++, mask >>= 1) {
      bool b = (YSF_SYNC_SYMBOLS & mask) == mask;

      if (m_buffer[ptr] > max)
        max = m_buffer[ptr];
      if (m_buffer[ptr] < min)
        min = m_buffer[ptr];

      corr += b ? -m_buffer[ptr] : m_buffer[ptr];

      ptr += YSF_RADIO_SYMBOL_LENGTH;
      if (ptr >= YSF_FRAME_LENGTH_SAMPLES)
        ptr -= YSF_FRAME_LENGTH_SAMPLES;
    }

    if (corr > m_maxCorr) {
      q15_t centre = (max + min) >> 1;

      q31_t v1 = (max - centre) * SCALING_FACTOR;
      q15_t threshold = q15_t(v1 >> 15);

      uint8_t sync[YSF_SYNC_BYTES_LENGTH];

      uint16_t ptr = m_dataPtr + YSF_FRAME_LENGTH_SAMPLES - YSF_SYNC_LENGTH_SAMPLES + YSF_RADIO_SYMBOL_LENGTH;
      if (ptr >= YSF_FRAME_LENGTH_SAMPLES)
        ptr -= YSF_FRAME_LENGTH_SAMPLES;

      if (m_state == YSFRXS_NONE)
        samplesToBits(ptr, YSF_SYNC_LENGTH_SYMBOLS, sync, 0U, centre, threshold);
      else
        samplesToBits(ptr, YSF_SYNC_LENGTH_SYMBOLS, sync, 0U, m_centreVal, m_thresholdVal);

      uint8_t errs = 0U;
      for (uint8_t i = 0U; i < YSF_SYNC_BYTES_LENGTH; i++)
        errs += countBits8(sync[i] ^ YSF_SYNC_BYTES[i]);

      if (errs <= MAX_SYNC_BYTES_ERRS) {
        m_maxCorr       = corr;
        m_thresholdBest = threshold;
        m_centreBest    = centre;
        m_lostCount     = MAX_SYNC_FRAMES;
        m_syncPtr       = m_dataPtr;

        m_endPtr = m_dataPtr + YSF_FRAME_LENGTH_SAMPLES - YSF_SYNC_LENGTH_SAMPLES - 1U;
        if (m_endPtr >= YSF_FRAME_LENGTH_SAMPLES)
          m_endPtr -= YSF_FRAME_LENGTH_SAMPLES;

        return true;
      }
    }
  }

  return false;
}

void CYSFRX::samplesToBits(uint16_t start, uint16_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold)
{
  for (uint16_t i = 0U; i < count; i++) {
    q15_t sample = m_buffer[start] - centre;

    if (sample < -threshold) {
      WRITE_BIT1(buffer, offset, false);
      offset++;
      WRITE_BIT1(buffer, offset, true);
      offset++;
    } else if (sample < 0) {
      WRITE_BIT1(buffer, offset, false);
      offset++;
      WRITE_BIT1(buffer, offset, false);
      offset++;
    } else if (sample < threshold) {
      WRITE_BIT1(buffer, offset, true);
      offset++;
      WRITE_BIT1(buffer, offset, false);
      offset++;
    } else {
      WRITE_BIT1(buffer, offset, true);
      offset++;
      WRITE_BIT1(buffer, offset, true);
      offset++;
    }

    start += YSF_RADIO_SYMBOL_LENGTH;
    if (start >= YSF_FRAME_LENGTH_SAMPLES)
      start -= YSF_FRAME_LENGTH_SAMPLES;
  }
}

void CYSFRX::writeRSSIData(uint8_t* data)
{
#if defined(SEND_RSSI_DATA)
  if (m_rssiCount > 0U) {
    uint16_t rssi = m_rssiAccum / m_rssiCount;

    data[120U] = (rssi >> 8) & 0xFFU;
    data[121U] = (rssi >> 0) & 0xFFU;

    serial.writeYSFData(data, YSF_FRAME_LENGTH_BYTES + 3U);
  } else {
    serial.writeYSFData(data, YSF_FRAME_LENGTH_BYTES + 1U);
  }
#else
  serial.writeYSFData(data, YSF_FRAME_LENGTH_BYTES + 1U);
#endif

  m_rssiAccum = 0U;
  m_rssiCount = 0U;
}

void CYSFRX::writeSync(uint16_t start)
{
  q15_t sync[YSF_SYNC_LENGTH_SYMBOLS];

  for (uint16_t i = 0U; i < YSF_SYNC_LENGTH_SYMBOLS; i++) {
    sync[i] = m_buffer[start];

    start += YSF_RADIO_SYMBOL_LENGTH;
    if (start >= YSF_FRAME_LENGTH_SAMPLES)
      start -= YSF_FRAME_LENGTH_SAMPLES;
  }

  serial.writeSamples(STATE_YSF, sync, YSF_SYNC_LENGTH_SYMBOLS);
}
