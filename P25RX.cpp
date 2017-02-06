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
#include "P25RX.h"
#include "Utils.h"

const q15_t SCALING_FACTOR = 18750;      // Q15(0.55)

const uint8_t MAX_SYNC_BIT_START_ERRS = 2U;
const uint8_t MAX_SYNC_BIT_RUN_ERRS   = 4U;

const uint8_t MAX_SYNC_SYMBOLS_ERRS = 2U;

const uint8_t BIT_MASK_TABLE[] = { 0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U };

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])

const uint16_t NOENDPTR = 9999U;

const unsigned int MAX_SYNC_FRAMES = 4U + 1U;

CP25RX::CP25RX() :
m_state(P25RXS_NONE),
m_bitBuffer(),
m_buffer(),
m_bitPtr(0U),
m_dataPtr(0U),
m_hdrStartPtr(NOENDPTR),
m_lduStartPtr(NOENDPTR),
m_lduEndPtr(NOENDPTR),
m_minSyncPtr(NOENDPTR),
m_maxSyncPtr(NOENDPTR),
m_hdrSyncPtr(NOENDPTR),
m_lduSyncPtr(NOENDPTR),
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

void CP25RX::reset()
{
  m_state         = P25RXS_NONE;
  m_dataPtr       = 0U;
  m_bitPtr        = 0U;
  m_maxCorr       = 0;
  m_averagePtr    = 0U;
  m_hdrStartPtr   = NOENDPTR;
  m_lduStartPtr   = NOENDPTR;
  m_lduEndPtr     = NOENDPTR;
  m_hdrSyncPtr    = NOENDPTR;
  m_lduSyncPtr    = NOENDPTR;
  m_minSyncPtr    = NOENDPTR;
  m_maxSyncPtr    = NOENDPTR;
  m_centreVal     = 0;
  m_thresholdVal  = 0;
  m_lostCount     = 0U;
  m_countdown     = 0U;
  m_rssiAccum     = 0U;
  m_rssiCount     = 0U;
}

void CP25RX::samples(const q15_t* samples, uint16_t* rssi, uint8_t length)
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
    case P25RXS_HDR:
      processHdr(sample);
      break;
    case P25RXS_LDU:
      processLdu(sample);
      break;
    default:
      processNone(sample);
      break;
    }

    m_dataPtr++;
    if (m_dataPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
      m_dataPtr = 0U;

    m_bitPtr++;
    if (m_bitPtr >= P25_RADIO_SYMBOL_LENGTH)
      m_bitPtr = 0U;
  }
}

void CP25RX::processNone(q15_t sample)
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

      // These are the sync positions for the following LDU after a HDR
      m_minSyncPtr = m_hdrSyncPtr + P25_HDR_FRAME_LENGTH_SAMPLES - 1U;
      if (m_minSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
        m_minSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

      m_maxSyncPtr = m_hdrSyncPtr + P25_HDR_FRAME_LENGTH_SAMPLES + 1U;
      if (m_maxSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
        m_maxSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

      m_state     = P25RXS_HDR;
      m_countdown = 0U;
  }
}

void CP25RX::processHdr(q15_t sample)
{
  if (m_minSyncPtr < m_maxSyncPtr) {
    if (m_dataPtr >= m_minSyncPtr && m_dataPtr <= m_maxSyncPtr)
      correlateSync();
  } else {
    if (m_dataPtr >= m_minSyncPtr || m_dataPtr <= m_maxSyncPtr)
      correlateSync();
  }

  if (m_dataPtr == m_maxSyncPtr) {
    bool isSync = false;
    if (m_minSyncPtr < m_maxSyncPtr) {
      if (m_lduSyncPtr >= m_minSyncPtr && m_lduSyncPtr <= m_maxSyncPtr)
        isSync = true;
    } else {
      if (m_lduSyncPtr >= m_minSyncPtr || m_lduSyncPtr <= m_maxSyncPtr)
        isSync = true;
    }

    if (isSync) {
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

      DEBUG4("P25RX: sync found in Hdr (best) pos/centre/threshold", m_hdrSyncPtr, m_centreBest, m_thresholdBest);
      DEBUG4("P25RX: sync found in Hdr pos/centre/threshold", m_hdrSyncPtr, m_centreVal, m_thresholdVal);

      uint8_t frame[P25_HDR_FRAME_LENGTH_BYTES + 1U];
      samplesToBits(m_hdrStartPtr, P25_HDR_FRAME_LENGTH_SYMBOLS, frame, 8U, m_centreVal, m_thresholdVal);
#if defined(DUMP_SAMPLES)
      writeSync(m_hdrStartPtr);
#endif

      frame[0U] = 0x01U;
      serial.writeP25Hdr(frame, P25_HDR_FRAME_LENGTH_BYTES + 1U);
    }

    m_minSyncPtr = m_lduSyncPtr + P25_LDU_FRAME_LENGTH_SAMPLES - 1U;
    if (m_minSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
      m_minSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

    m_maxSyncPtr = m_lduSyncPtr + 1U;
    if (m_maxSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
      m_maxSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

    m_state   = P25RXS_LDU;
    m_maxCorr = 0;
  }
}

void CP25RX::processLdu(q15_t sample)
{
  if (m_minSyncPtr < m_maxSyncPtr) {
    if (m_dataPtr >= m_minSyncPtr && m_dataPtr <= m_maxSyncPtr)
      correlateSync();
  } else {
    if (m_dataPtr >= m_minSyncPtr || m_dataPtr <= m_maxSyncPtr)
      correlateSync();
  }

  if (m_dataPtr == m_lduEndPtr) {
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

      DEBUG4("P25RX: sync found in Ldu (best) pos/centre/threshold", m_lduSyncPtr, m_centreBest, m_thresholdBest);
      DEBUG4("P25RX: sync found in Ldu pos/centre/threshold", m_lduSyncPtr, m_centreVal, m_thresholdVal);

      m_minSyncPtr = m_lduSyncPtr + P25_LDU_FRAME_LENGTH_SAMPLES - 1U;
      if (m_minSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
        m_minSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

      m_maxSyncPtr = m_lduSyncPtr + 1U;
      if (m_maxSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
        m_maxSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;
    }

    uint8_t frame[P25_LDU_FRAME_LENGTH_BYTES + 3U];
    samplesToBits(m_lduStartPtr, P25_LDU_FRAME_LENGTH_SYMBOLS, frame, 8U, m_centreVal, m_thresholdVal);
#if defined(DUMP_SAMPLES)
    writeSync(m_lduStartPtr);
#endif

    // We've not seen a data sync for too long, signal RXLOST and change to RX_NONE
    m_lostCount--;
    if (m_lostCount == 0U) {
      DEBUG1("P25RX: sync timed out, lost lock");

      io.setDecode(false);
      io.setADCDetection(false);

      serial.writeP25Lost();

      m_state     = P25RXS_NONE;
      m_lduEndPtr = NOENDPTR;
      m_maxCorr   = 0;
		} else {
      frame[0U] = m_lostCount == (MAX_SYNC_FRAMES - 1U) ? 0x01U : 0x00U;

      writeRSSILdu(frame);

      m_maxCorr = 0;
    }
  }
}

bool CP25RX::correlateSync()
{
  if (countBits32((m_bitBuffer[m_bitPtr] & P25_SYNC_SYMBOLS_MASK) ^ P25_SYNC_SYMBOLS) <= MAX_SYNC_SYMBOLS_ERRS) {
    uint16_t ptr = m_dataPtr + P25_LDU_FRAME_LENGTH_SAMPLES - P25_SYNC_LENGTH_SAMPLES + P25_RADIO_SYMBOL_LENGTH;
    if (ptr >= P25_LDU_FRAME_LENGTH_SAMPLES)
      ptr -= P25_LDU_FRAME_LENGTH_SAMPLES;

    q31_t corr = 0;
    q15_t max = -16000;
    q15_t min = 16000;

    for (uint8_t i = 0U; i < P25_SYNC_LENGTH_SYMBOLS; i++) {
      q15_t val = m_buffer[ptr];

      if (val > max)
        max = val;
      if (val < min)
        min = val;

      switch (P25_SYNC_SYMBOLS_VALUES[i]) {
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

      ptr += P25_RADIO_SYMBOL_LENGTH;
      if (ptr >= P25_LDU_FRAME_LENGTH_SAMPLES)
        ptr -= P25_LDU_FRAME_LENGTH_SAMPLES;
    }

    if (corr > m_maxCorr) {
      q15_t centre = (max + min) >> 1;

      q31_t v1 = (max - centre) * SCALING_FACTOR;
      q15_t threshold = q15_t(v1 >> 15);

      uint8_t sync[P25_SYNC_BYTES_LENGTH];

      uint16_t startPtr = m_dataPtr + P25_LDU_FRAME_LENGTH_SAMPLES - P25_SYNC_LENGTH_SAMPLES + P25_RADIO_SYMBOL_LENGTH;
      if (startPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
        startPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

      uint8_t maxErrs;
      if (m_state == P25RXS_NONE) {
        samplesToBits(startPtr, P25_SYNC_LENGTH_SYMBOLS, sync, 0U, centre, threshold);
        maxErrs = MAX_SYNC_BIT_START_ERRS;
      } else {
        samplesToBits(startPtr, P25_SYNC_LENGTH_SYMBOLS, sync, 0U, m_centreVal, m_thresholdVal);
        maxErrs = MAX_SYNC_BIT_RUN_ERRS;
      }

      uint8_t errs = 0U;
      for (uint8_t i = 0U; i < P25_SYNC_BYTES_LENGTH; i++)
        errs += countBits8(sync[i] ^ P25_SYNC_BYTES[i]);

      if (errs <= maxErrs) {
        m_maxCorr       = corr;
        m_thresholdBest = threshold;
        m_centreBest    = centre;
        m_lostCount     = MAX_SYNC_FRAMES;

        m_lduSyncPtr    = m_dataPtr;

        // These are the positions of the start and end of an LDU
        m_lduStartPtr   = startPtr;

        m_lduEndPtr = m_dataPtr + P25_LDU_FRAME_LENGTH_SAMPLES - P25_SYNC_LENGTH_SAMPLES - 1U;
        if (m_lduEndPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
          m_lduEndPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

        if (m_state == P25RXS_NONE) {
          m_hdrSyncPtr = m_dataPtr;

          // This is the position of the start of a HDR
          m_hdrStartPtr = startPtr;

          // These are the range of positions for a sync for an LDU following a HDR
          m_minSyncPtr = m_dataPtr + P25_HDR_FRAME_LENGTH_SAMPLES - 1U;
          if (m_minSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
            m_minSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;

          m_maxSyncPtr = m_dataPtr + P25_HDR_FRAME_LENGTH_SAMPLES + 1U;
          if (m_maxSyncPtr >= P25_LDU_FRAME_LENGTH_SAMPLES)
            m_maxSyncPtr -= P25_LDU_FRAME_LENGTH_SAMPLES;
        }

        return true;
      }
    }
  }

  return false;
}

void CP25RX::samplesToBits(uint16_t start, uint16_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold)
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

    start += P25_RADIO_SYMBOL_LENGTH;
    if (start >= P25_LDU_FRAME_LENGTH_SAMPLES)
      start -= P25_LDU_FRAME_LENGTH_SAMPLES;
  }
}

void CP25RX::writeRSSILdu(uint8_t* ldu)
{
#if defined(SEND_RSSI_DATA)
  if (m_rssiCount > 0U) {
    uint16_t rssi = m_rssiAccum / m_rssiCount;

    ldu[216U] = (rssi >> 8) & 0xFFU;
    ldu[217U] = (rssi >> 0) & 0xFFU;

    serial.writeP25Ldu(ldu, P25_LDU_FRAME_LENGTH_BYTES + 3U);
  } else {
    serial.writeP25Ldu(ldu, P25_LDU_FRAME_LENGTH_BYTES + 1U);
  }
#else
  serial.writeP25Ldu(ldu, P25_LDU_FRAME_LENGTH_BYTES + 1U);
#endif

  m_rssiAccum = 0U;
  m_rssiCount = 0U;
}

#if defined(DUMP_SAMPLES)
void CP25RX::writeSync(uint16_t start)
{
  q15_t sync[P25_SYNC_LENGTH_SYMBOLS];

  for (uint16_t i = 0U; i < P25_SYNC_LENGTH_SYMBOLS; i++) {
    if (i == (P25_SYNC_LENGTH_SYMBOLS - 1U))
      DEBUG4("P25RX: sync1/sync2/start", m_hdrSyncPtr, m_lduSyncPtr, start);

    sync[i] = m_buffer[start];

    start += P25_RADIO_SYMBOL_LENGTH;
    if (start >= P25_LDU_FRAME_LENGTH_SAMPLES)
      start -= P25_LDU_FRAME_LENGTH_SAMPLES;
  }

  serial.writeSamples(STATE_P25, sync, P25_SYNC_LENGTH_SYMBOLS);
}
#endif

