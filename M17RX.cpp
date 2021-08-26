/*
 *   Copyright (C) 2009-2017,2020,2021 by Jonathan Naylor G4KLX
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

#if defined(MODE_M17)

#include "Globals.h"
#include "M17RX.h"
#include "Utils.h"

const q15_t SCALING_FACTOR = 18750;      // Q15(0.55)

const uint8_t MAX_SYNC_BIT_START_ERRS = 0U;
const uint8_t MAX_SYNC_BIT_RUN_ERRS   = 2U;

const uint8_t MAX_SYNC_SYMBOL_START_ERRS = 0U;
const uint8_t MAX_SYNC_SYMBOL_RUN_ERRS   = 1U;

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])

const uint8_t NOAVEPTR = 99U;

const uint16_t NOENDPTR = 9999U;

const unsigned int MAX_SYNC_FRAMES = 3U + 1U;

CM17RX::CM17RX() :
m_state(M17RXS_NONE),
m_bitBuffer(),
m_buffer(),
m_bitPtr(0U),
m_dataPtr(0U),
m_startPtr(NOENDPTR),
m_endPtr(NOENDPTR),
m_syncPtr(NOENDPTR),
m_minSyncPtr(NOENDPTR),
m_maxSyncPtr(NOENDPTR),
m_maxCorr(0),
m_lostCount(0U),
m_countdown(0U),
m_nextState(M17RXS_NONE),
m_centre(),
m_centreVal(0),
m_threshold(),
m_thresholdVal(0),
m_averagePtr(NOAVEPTR),
m_rssiAccum(0U),
m_rssiCount(0U)
{
}

void CM17RX::reset()
{
  m_state        = M17RXS_NONE;
  m_dataPtr      = 0U;
  m_bitPtr       = 0U;
  m_maxCorr      = 0;
  m_averagePtr   = NOAVEPTR;
  m_startPtr     = NOENDPTR;
  m_endPtr       = NOENDPTR;
  m_syncPtr      = NOENDPTR;
  m_minSyncPtr   = NOENDPTR;
  m_maxSyncPtr   = NOENDPTR;
  m_centreVal    = 0;
  m_thresholdVal = 0;
  m_lostCount    = 0U;
  m_countdown    = 0U;
  m_nextState    = M17RXS_NONE;
  m_rssiAccum    = 0U;
  m_rssiCount    = 0U;
}

void CM17RX::samples(const q15_t* samples, uint16_t* rssi, uint8_t length)
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
    case M17RXS_LINK_SETUP:
    case M17RXS_STREAM:
      processData(sample);
      break;
    default:
      processNone(sample);
      break;
    }

    m_dataPtr++;
    if (m_dataPtr >= M17_FRAME_LENGTH_SAMPLES)
      m_dataPtr = 0U;

    m_bitPtr++;
    if (m_bitPtr >= M17_RADIO_SYMBOL_LENGTH)
      m_bitPtr = 0U;
  }
}

void CM17RX::processNone(q15_t sample)
{
  bool ret1 = correlateSync(M17_LINK_SETUP_SYNC_SYMBOLS, M17_LINK_SETUP_SYNC_SYMBOLS_VALUES, M17_LINK_SETUP_SYNC_BYTES, MAX_SYNC_SYMBOL_START_ERRS, MAX_SYNC_BIT_START_ERRS);
  bool ret2 = correlateSync(M17_STREAM_SYNC_SYMBOLS,     M17_STREAM_SYNC_SYMBOLS_VALUES,     M17_STREAM_SYNC_BYTES,     MAX_SYNC_SYMBOL_START_ERRS, MAX_SYNC_BIT_START_ERRS);

  if (ret1 || ret2) {
    // On the first sync, start the countdown to the state change
    if (m_countdown == 0U) {
      m_rssiAccum = 0U;
      m_rssiCount = 0U;

      io.setDecode(true);
      io.setADCDetection(true);

      m_averagePtr = NOAVEPTR;

      m_countdown = 5U;
      
      if (ret1) m_nextState = M17RXS_LINK_SETUP;
      if (ret2) m_nextState = M17RXS_STREAM;
    }
  }

  if (m_countdown > 0U)
    m_countdown--;

  if (m_countdown == 1U) {
    m_minSyncPtr = m_syncPtr + M17_FRAME_LENGTH_SAMPLES - 1U;
    if (m_minSyncPtr >= M17_FRAME_LENGTH_SAMPLES)
      m_minSyncPtr -= M17_FRAME_LENGTH_SAMPLES;

    m_maxSyncPtr = m_syncPtr + 1U;
    if (m_maxSyncPtr >= M17_FRAME_LENGTH_SAMPLES)
      m_maxSyncPtr -= M17_FRAME_LENGTH_SAMPLES;

    m_state     = m_nextState;
    m_countdown = 0U;
    m_nextState = M17RXS_NONE;
  }
}

void CM17RX::processData(q15_t sample)
{
  bool eof = false;

  if (m_minSyncPtr < m_maxSyncPtr) {
    if (m_dataPtr >= m_minSyncPtr && m_dataPtr <= m_maxSyncPtr) {
      bool ret = correlateSync(M17_STREAM_SYNC_SYMBOLS, M17_STREAM_SYNC_SYMBOLS_VALUES, M17_STREAM_SYNC_BYTES,  MAX_SYNC_SYMBOL_RUN_ERRS, MAX_SYNC_BIT_RUN_ERRS);

      eof = correlateSync(M17_EOF_SYNC_SYMBOLS, M17_EOF_SYNC_SYMBOLS_VALUES, M17_EOF_SYNC_BYTES, MAX_SYNC_SYMBOL_RUN_ERRS, MAX_SYNC_BIT_RUN_ERRS);

      if (ret) m_state = M17RXS_STREAM;
    }
  } else {
    if (m_dataPtr >= m_minSyncPtr || m_dataPtr <= m_maxSyncPtr) {
      bool ret = correlateSync(M17_STREAM_SYNC_SYMBOLS, M17_STREAM_SYNC_SYMBOLS_VALUES, M17_STREAM_SYNC_BYTES,  MAX_SYNC_SYMBOL_RUN_ERRS, MAX_SYNC_BIT_RUN_ERRS);

      eof = correlateSync(M17_EOF_SYNC_SYMBOLS, M17_EOF_SYNC_SYMBOLS_VALUES, M17_EOF_SYNC_BYTES, MAX_SYNC_SYMBOL_RUN_ERRS, MAX_SYNC_BIT_RUN_ERRS);

      if (ret) m_state = M17RXS_STREAM;
    }
  }

  if (eof) {
    DEBUG4("M17RX: eof sync found pos/centre/threshold", m_syncPtr, m_centreVal, m_thresholdVal);

    io.setDecode(false);
    io.setADCDetection(false);

    serial.writeM17EOT();

    m_state      = M17RXS_NONE;
    m_endPtr     = NOENDPTR;
    m_averagePtr = NOAVEPTR;
    m_countdown  = 0U;
    m_nextState  = M17RXS_NONE;
    m_maxCorr    = 0;
  }

  if (m_dataPtr == m_endPtr) {
    // Only update the centre and threshold if they are from a good sync
    if (m_lostCount == MAX_SYNC_FRAMES) {
      m_minSyncPtr = m_syncPtr + M17_FRAME_LENGTH_SAMPLES - 1U;
      if (m_minSyncPtr >= M17_FRAME_LENGTH_SAMPLES)
        m_minSyncPtr -= M17_FRAME_LENGTH_SAMPLES;

      m_maxSyncPtr = m_syncPtr + 1U;
      if (m_maxSyncPtr >= M17_FRAME_LENGTH_SAMPLES)
        m_maxSyncPtr -= M17_FRAME_LENGTH_SAMPLES;
    }

    calculateLevels(m_startPtr, M17_FRAME_LENGTH_SYMBOLS);

    switch (m_state) {
      case M17RXS_LINK_SETUP:
        DEBUG4("M17RX: link setup sync found pos/centre/threshold", m_syncPtr, m_centreVal, m_thresholdVal);
        break;
      case M17RXS_STREAM:
        DEBUG4("M17RX: stream sync found pos/centre/threshold", m_syncPtr, m_centreVal, m_thresholdVal);
        break;
      default:
        break;  
    }

    uint8_t frame[M17_FRAME_LENGTH_BYTES + 3U];
    samplesToBits(m_startPtr, M17_FRAME_LENGTH_SYMBOLS, frame, 8U, m_centreVal, m_thresholdVal);

    // We've not seen a stream sync for too long, signal RXLOST and change to RX_NONE
    m_lostCount--;
    if (m_lostCount == 0U) {
      DEBUG1("M17RX: sync timed out, lost lock");

      io.setDecode(false);
      io.setADCDetection(false);

      serial.writeM17Lost();

      m_state      = M17RXS_NONE;
      m_endPtr     = NOENDPTR;
      m_averagePtr = NOAVEPTR;
      m_countdown  = 0U;
      m_nextState  = M17RXS_NONE;
      m_maxCorr    = 0;
    } else {
      frame[0U] = m_lostCount == (MAX_SYNC_FRAMES - 1U) ? 0x01U : 0x00U;

      switch (m_state) {
        case M17RXS_LINK_SETUP:
          writeRSSILinkSetup(frame);
          break;
        case M17RXS_STREAM:
          writeRSSIStream(frame);
          break;
        default:
          break;  
      }

      m_maxCorr   = 0;
      m_nextState = M17RXS_NONE;
    }
  }
}

bool CM17RX::correlateSync(uint8_t syncSymbols, const int8_t* syncSymbolValues, const uint8_t* syncBytes, uint8_t maxSymbolErrs, uint8_t maxBitErrs)
{
  if (countBits8(m_bitBuffer[m_bitPtr] ^ syncSymbols) <= maxSymbolErrs) {
    uint16_t ptr = m_dataPtr + M17_FRAME_LENGTH_SAMPLES - M17_SYNC_LENGTH_SAMPLES + M17_RADIO_SYMBOL_LENGTH;
    if (ptr >= M17_FRAME_LENGTH_SAMPLES)
      ptr -= M17_FRAME_LENGTH_SAMPLES;

    q31_t corr = 0;
    q15_t min =  16000;
    q15_t max = -16000;

    for (uint8_t i = 0U; i < M17_SYNC_LENGTH_SYMBOLS; i++) {
      q15_t val = m_buffer[ptr];

      if (val > max)
        max = val;
      if (val < min)
        min = val;

      switch (syncSymbolValues[i]) {
      case +3:
        corr -= (val + val + val);
        break;
      case +1:
        corr -= val;
        break;
      case -1:
        corr += val;
        break;
      default:  // -3
        corr += (val + val + val);
        break;
      }

      ptr += M17_RADIO_SYMBOL_LENGTH;
      if (ptr >= M17_FRAME_LENGTH_SAMPLES)
        ptr -= M17_FRAME_LENGTH_SAMPLES;
    }

    if (corr > m_maxCorr) {
      if (m_averagePtr == NOAVEPTR) {
        m_centreVal = (max + min) >> 1;

        q31_t v1 = (max - m_centreVal) * SCALING_FACTOR;
        m_thresholdVal = q15_t(v1 >> 15);
      }

      uint16_t startPtr = m_dataPtr + M17_FRAME_LENGTH_SAMPLES - M17_SYNC_LENGTH_SAMPLES + M17_RADIO_SYMBOL_LENGTH;
      if (startPtr >= M17_FRAME_LENGTH_SAMPLES)
        startPtr -= M17_FRAME_LENGTH_SAMPLES;

      uint8_t sync[M17_SYNC_LENGTH_BYTES];
      samplesToBits(startPtr, M17_SYNC_LENGTH_SYMBOLS, sync, 0U, m_centreVal, m_thresholdVal);

      uint8_t errs = 0U;
      for (uint8_t i = 0U; i < M17_SYNC_LENGTH_BYTES; i++)
        errs += countBits8(sync[i] ^ syncBytes[i]);

      if (errs <= maxBitErrs) {
        m_maxCorr   = corr;
        m_lostCount = MAX_SYNC_FRAMES;
        m_syncPtr   = m_dataPtr;

        m_startPtr = startPtr;

        m_endPtr = m_dataPtr + M17_FRAME_LENGTH_SAMPLES - M17_SYNC_LENGTH_SAMPLES - 1U;
        if (m_endPtr >= M17_FRAME_LENGTH_SAMPLES)
          m_endPtr -= M17_FRAME_LENGTH_SAMPLES;

        return true;
      }
    }
  }

  return false;
}

void CM17RX::calculateLevels(uint16_t start, uint16_t count)
{
  q15_t maxPos = -16000;
  q15_t minPos =  16000;
  q15_t maxNeg =  16000;
  q15_t minNeg = -16000;

  for (uint16_t i = 0U; i < count; i++) {
    q15_t sample = m_buffer[start];

    if (sample > 0) {
      if (sample > maxPos)
        maxPos = sample;
      if (sample < minPos)
        minPos = sample;
    } else {
      if (sample < maxNeg)
        maxNeg = sample;
      if (sample > minNeg)
        minNeg = sample;
    }

    start += M17_RADIO_SYMBOL_LENGTH;
    if (start >= M17_FRAME_LENGTH_SAMPLES)
      start -= M17_FRAME_LENGTH_SAMPLES;
  }

  q15_t posThresh = (maxPos + minPos) >> 1;
  q15_t negThresh = (maxNeg + minNeg) >> 1;

  q15_t centre = (posThresh + negThresh) >> 1;

  q15_t threshold = posThresh - centre;

  DEBUG5("M17RX: pos/neg/centre/threshold", posThresh, negThresh, centre, threshold);

  if (m_averagePtr == NOAVEPTR) {
    for (uint8_t i = 0U; i < 16U; i++) {
      m_centre[i] = centre;
      m_threshold[i] = threshold;
    }

    m_averagePtr = 0U;
  } else {
    m_centre[m_averagePtr] = centre;
    m_threshold[m_averagePtr] = threshold;

    m_averagePtr++;
    if (m_averagePtr >= 16U)
      m_averagePtr = 0U;
  }

  m_centreVal = 0;
  m_thresholdVal = 0;

  for (uint8_t i = 0U; i < 16U; i++) {
    m_centreVal += m_centre[i];
    m_thresholdVal += m_threshold[i];
  }

  m_centreVal >>= 4;
  m_thresholdVal >>= 4;
}

void CM17RX::samplesToBits(uint16_t start, uint16_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold)
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

    start += M17_RADIO_SYMBOL_LENGTH;
    if (start >= M17_FRAME_LENGTH_SAMPLES)
      start -= M17_FRAME_LENGTH_SAMPLES;
  }
}

void CM17RX::writeRSSILinkSetup(uint8_t* data)
{
#if defined(SEND_RSSI_DATA)
  if (m_rssiCount > 0U) {
    uint16_t rssi = m_rssiAccum / m_rssiCount;

    data[49U] = (rssi >> 8) & 0xFFU;
    data[50U] = (rssi >> 0) & 0xFFU;

    serial.writeM17LinkSetup(data, M17_FRAME_LENGTH_BYTES + 3U);
  } else {
    serial.writeM17LinkSetup(data, M17_FRAME_LENGTH_BYTES + 1U);
  }
#else
  serial.writeM17LinkSetup(data, M17_FRAME_LENGTH_BYTES + 1U);
#endif

  m_rssiAccum = 0U;
  m_rssiCount = 0U;
}

void CM17RX::writeRSSIStream(uint8_t* data)
{
#if defined(SEND_RSSI_DATA)
  if (m_rssiCount > 0U) {
    uint16_t rssi = m_rssiAccum / m_rssiCount;

    data[49U] = (rssi >> 8) & 0xFFU;
    data[50U] = (rssi >> 0) & 0xFFU;

    serial.writeM17Stream(data, M17_FRAME_LENGTH_BYTES + 3U);
  } else {
    serial.writeM17Stream(data, M17_FRAME_LENGTH_BYTES + 1U);
  }
#else
  serial.writeM17Stream(data, M17_FRAME_LENGTH_BYTES + 1U);
#endif

  m_rssiAccum = 0U;
  m_rssiCount = 0U;
}

#endif

