/*
 *   Copyright (C) 2013,2015,2016 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

// #define  WANT_DEBUG

#include "Config.h"
#include "Globals.h"

#include "SerialPort.h"

const uint8_t MMDVM_FRAME_START  = 0xE0U;

const uint8_t MMDVM_GET_VERSION  = 0x00U;
const uint8_t MMDVM_GET_STATUS   = 0x01U;
const uint8_t MMDVM_SET_CONFIG   = 0x02U;
const uint8_t MMDVM_SET_MODE     = 0x03U;
const uint8_t MMDVM_SET_FREQ     = 0x04U;

const uint8_t MMDVM_CAL_DATA     = 0x08U;

const uint8_t MMDVM_SEND_CWID    = 0x0AU;

const uint8_t MMDVM_DSTAR_HEADER = 0x10U;
const uint8_t MMDVM_DSTAR_DATA   = 0x11U;
const uint8_t MMDVM_DSTAR_LOST   = 0x12U;
const uint8_t MMDVM_DSTAR_EOT    = 0x13U;

const uint8_t MMDVM_DMR_DATA1    = 0x18U;
const uint8_t MMDVM_DMR_LOST1    = 0x19U;
const uint8_t MMDVM_DMR_DATA2    = 0x1AU;
const uint8_t MMDVM_DMR_LOST2    = 0x1BU;
const uint8_t MMDVM_DMR_SHORTLC  = 0x1CU;
const uint8_t MMDVM_DMR_START    = 0x1DU;
const uint8_t MMDVM_DMR_ABORT    = 0x1EU;

const uint8_t MMDVM_YSF_DATA     = 0x20U;
const uint8_t MMDVM_YSF_LOST     = 0x21U;

const uint8_t MMDVM_ACK          = 0x70U;
const uint8_t MMDVM_NAK          = 0x7FU;

const uint8_t MMDVM_DEBUG1       = 0xF1U;
const uint8_t MMDVM_DEBUG2       = 0xF2U;
const uint8_t MMDVM_DEBUG3       = 0xF3U;
const uint8_t MMDVM_DEBUG4       = 0xF4U;
const uint8_t MMDVM_DEBUG5       = 0xF5U;

const uint8_t HARDWARE[]         = "MMDVM 20160624 24kHz (D-Star/DMR/System Fusion/CW Id)";

const uint8_t PROTOCOL_VERSION   = 1U;


CSerialPort::CSerialPort() :
#if defined(__MBED__)
m_serial(SERIAL_TX, SERIAL_RX),
#endif
m_buffer(),
m_ptr(0U),
m_len(0U)
{
}

void CSerialPort::sendACK()
{
  uint8_t reply[4U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = MMDVM_ACK;
  reply[3U] = m_buffer[2U];

  write(reply, 4);
}

void CSerialPort::sendNAK(uint8_t err)
{
  uint8_t reply[5U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 5U;
  reply[2U] = MMDVM_NAK;
  reply[3U] = m_buffer[2U];
  reply[4U] = err;

  write(reply, 5);
}

void CSerialPort::getStatus()
{
  io.resetWatchdog();

  uint8_t reply[11U];

  // Send all sorts of interesting internal values
  reply[0U]  = MMDVM_FRAME_START;
  reply[1U]  = 10U;
  reply[2U]  = MMDVM_GET_STATUS;

  reply[3U]  = 0x00U;
  if (m_dstarEnable)
    reply[3U] |= 0x01U;
  if (m_dmrEnable)
    reply[3U] |= 0x02U;
  if (m_ysfEnable)
    reply[3U] |= 0x04U;

  reply[4U]  = uint8_t(m_modemState);

  reply[5U]  = m_tx ? 0x01U : 0x00U;

  bool adcOverflow;
  bool dacOverflow;
  io.getOverflow(adcOverflow, dacOverflow);

  if (adcOverflow)
    reply[5U] |= 0x02U;

  if (io.hasRXOverflow())
    reply[5U] |= 0x04U;

  if (io.hasTXOverflow())
    reply[5U] |= 0x08U;

  if (io.hasLockout())
    reply[5U] |= 0x10U;

  if (dacOverflow)
    reply[5U] |= 0x20U;

  if (m_dstarEnable)
    reply[6U] = dstarTX.getSpace();
  else
    reply[6U] = 0U;

  if (m_dmrEnable) {
    reply[7U] = dmrTX.getSpace1();
    reply[8U] = dmrTX.getSpace2();
  } else {
    reply[7U] = 0U;
    reply[8U] = 0U;
  }

  if (m_ysfEnable)
    reply[9U] = ysfTX.getSpace();
  else
    reply[9U] = 0U;

  write(reply, 10);
}

void CSerialPort::getVersion()
{
  uint8_t reply[100U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_GET_VERSION;

  reply[3U] = PROTOCOL_VERSION;

  uint8_t count = 4U;
  for (uint8_t i = 0U; HARDWARE[i] != 0x00U; i++, count++)
    reply[count] = HARDWARE[i];

  reply[1U] = count;

  write(reply, count);
}

uint8_t CSerialPort::setConfig(const uint8_t* data, uint8_t length)
{
  if (length < 9U)
    return 4U;

  bool rxInvert  = (data[0U] & 0x01U) == 0x01U;
  bool txInvert  = (data[0U] & 0x02U) == 0x02U;
  bool pttInvert = (data[0U] & 0x04U) == 0x04U;

  bool dstarEnable = (data[1U] & 0x01U) == 0x01U;
  bool dmrEnable   = (data[1U] & 0x02U) == 0x02U;
  bool ysfEnable   = (data[1U] & 0x04U) == 0x04U;

  uint8_t txDelay = data[2U];
  if (txDelay > 50U)
    return 4U;

  MMDVM_STATE modemState = MMDVM_STATE(data[3U]);

  if (modemState != STATE_IDLE && modemState != STATE_DSTAR && modemState != STATE_DMR && modemState != STATE_YSF && modemState != STATE_DSTARCAL && modemState != STATE_DMRCAL)
    return 4U;
  if (modemState == STATE_DSTAR && !dstarEnable)
    return 4U;
  if (modemState == STATE_DMR && !dmrEnable)
    return 4U;
  if (modemState == STATE_YSF && !ysfEnable)
    return 4U;

  uint8_t rxLevel = data[4U];
  uint8_t txLevel = data[5U];

  uint8_t colorCode = data[6U];
  if (colorCode > 15U)
    return 4U;

  uint8_t dmrDelay = data[7U];

  int8_t oscOffset = int8_t(data[8U]) - 128;
  if (oscOffset < 0) {
    m_sampleCount = 1000000U / uint32_t(-oscOffset);
    m_sampleInsert = false;
  } else if (oscOffset > 0) {
    m_sampleCount = 1000000U / uint32_t(oscOffset);
    m_sampleInsert = true;
  } else {
    m_sampleCount = 0U;
    m_sampleInsert = false;
  }

  m_modemState  = modemState;

  m_dstarEnable = dstarEnable;
  m_dmrEnable   = dmrEnable;
  m_ysfEnable   = ysfEnable;

  dstarTX.setTXDelay(txDelay);
  ysfTX.setTXDelay(txDelay);

  dmrTX.setColorCode(colorCode);
  dmrRX.setColorCode(colorCode);
  dmrRX.setDelay(dmrDelay);
  dmrIdleRX.setColorCode(colorCode);

  io.setParameters(rxInvert, txInvert, pttInvert, rxLevel, txLevel);

  io.start();

  return 0U;
}

uint8_t CSerialPort::setMode(const uint8_t* data, uint8_t length)
{
  if (length < 1U)
    return 4U;

  MMDVM_STATE modemState = MMDVM_STATE(data[0U]);

  if (modemState == m_modemState)
    return 0U;

  if (modemState != STATE_IDLE && modemState != STATE_DSTAR && modemState != STATE_DMR && modemState != STATE_YSF && modemState != STATE_DSTARCAL && modemState != STATE_DMRCAL)
    return 4U;
  if (modemState == STATE_DSTAR && !m_dstarEnable)
    return 4U;
  if (modemState == STATE_DMR && !m_dmrEnable)
    return 4U;
  if (modemState == STATE_YSF && !m_ysfEnable)
    return 4U;

  setMode(modemState);

  return 0U;
}

void CSerialPort::setMode(MMDVM_STATE modemState)
{
  switch (modemState) {
    case STATE_DMR:
      DEBUG1("Mode set to DMR");
      dstarRX.reset();
      ysfRX.reset();
      cwIdTX.reset();
      break;
    case STATE_DSTAR:
      DEBUG1("Mode set to D-Star");
      dmrIdleRX.reset();
      dmrRX.reset();
      ysfRX.reset();
      cwIdTX.reset();
      break;
    case STATE_YSF:
      DEBUG1("Mode set to System Fusion");
      dmrIdleRX.reset();
      dmrRX.reset();
      dstarRX.reset();
      cwIdTX.reset();
      break;
    case STATE_DSTARCAL:
      DEBUG1("Mode set to D-Star Calibrate");
      dmrIdleRX.reset();
      dmrRX.reset();
      dstarRX.reset();
      ysfRX.reset();
      cwIdTX.reset();
      break;
    case STATE_DMRCAL:
      DEBUG1("Mode set to DMR Calibrate");
      dmrIdleRX.reset();
      dmrRX.reset();
      dstarRX.reset();
      ysfRX.reset();
      cwIdTX.reset();
      break;
    default:
      DEBUG1("Mode set to Idle");
      // STATE_IDLE
      break;
  }

  m_modemState = modemState;

  io.setMode();
}

void CSerialPort::start()
{
#if defined(__MBED__)
  m_serial.baud(115200);
#else
  Serial.begin(115200);
#endif
}

void CSerialPort::process()
{
#if defined(__MBED__)
  while (m_serial.readable()) {
    uint8_t c = m_serial.getc();
#else
  while (Serial.available()) {
    uint8_t c = Serial.read();
#endif

    if (m_ptr == 0U && c == MMDVM_FRAME_START) {
      // Handle the frame start correctly
      m_buffer[0U] = c;
      m_ptr = 1U;
      m_len = 0U;
    } else if (m_ptr > 0U) {
      // Any other bytes are added to the buffer
      m_buffer[m_ptr] = c;
      m_ptr++;

      // Once we have enough bytes, calculate the expected length
      if (m_ptr == 2U)
        m_len = m_buffer[1U];

      if (m_ptr == 3U && m_len > 130U) {
        sendNAK(3U);
        m_ptr = 0U;
        m_len = 0U;
      }

      // The full packet has been received, process it
      if (m_ptr == m_len) {
        uint8_t err = 2U;

        switch (m_buffer[2U]) {
          case MMDVM_GET_STATUS:
            getStatus();
            break;

          case MMDVM_GET_VERSION:
            getVersion();
            break;

          case MMDVM_SET_CONFIG:
            err = setConfig(m_buffer + 3U, m_len - 3U);
            if (err == 0U)
              sendACK();
            else
              sendNAK(err);
            break;

          case MMDVM_SET_MODE:
            err = setMode(m_buffer + 3U, m_len - 3U);
            if (err == 0U)
              sendACK();
            else
              sendNAK(err);
            break;

          case MMDVM_SET_FREQ:
            sendACK();
            break;

          case MMDVM_CAL_DATA:
            if (m_modemState == STATE_DSTARCAL)
              err = calDStarTX.write(m_buffer + 3U, m_len - 3U);
            if (m_modemState == STATE_DMRCAL)
              err = calDMR.write(m_buffer + 3U, m_len - 3U);
            if (err == 0U) {
              sendACK();
            } else {
              DEBUG2("Received invalid calibration data", err);
              sendNAK(err);
            }
            break;

          case MMDVM_SEND_CWID:
            err = 5U;
            if (m_modemState == STATE_IDLE)
              err = cwIdTX.write(m_buffer + 3U, m_len - 3U);
            if (err != 0U) {
              DEBUG2("Invalid CW Id data", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DSTAR_HEADER:
            if (m_dstarEnable) {
              if (m_modemState == STATE_IDLE || m_modemState == STATE_DSTAR)
                err = dstarTX.writeHeader(m_buffer + 3U, m_len - 3U);
            }
            if (err == 0U) {
              if (m_modemState == STATE_IDLE)
                setMode(STATE_DSTAR);
            } else {
              DEBUG2("Received invalid D-Star header", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DSTAR_DATA:
            if (m_dstarEnable) {
              if (m_modemState == STATE_IDLE || m_modemState == STATE_DSTAR)
                err = dstarTX.writeData(m_buffer + 3U, m_len - 3U);
            }
            if (err == 0U) {
              if (m_modemState == STATE_IDLE)
                setMode(STATE_DSTAR);
            } else {
              DEBUG2("Received invalid D-Star data", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DSTAR_EOT:
            if (m_dstarEnable) {
              if (m_modemState == STATE_IDLE || m_modemState == STATE_DSTAR)
                err = dstarTX.writeEOT();
            }
            if (err == 0U) {
              if (m_modemState == STATE_IDLE)
                setMode(STATE_DSTAR);
            } else {
              DEBUG2("Received invalid D-Star EOT", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DMR_DATA1:
            if (m_dmrEnable) {
              if (m_modemState == STATE_IDLE || m_modemState == STATE_DMR)
                err = dmrTX.writeData1(m_buffer + 3U, m_len - 3U);
            }
            if (err == 0U) {
              if (m_modemState == STATE_IDLE)
                setMode(STATE_DMR);
            } else {
              DEBUG2("Received invalid DMR data", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DMR_DATA2:
            if (m_dmrEnable) {
              if (m_modemState == STATE_IDLE || m_modemState == STATE_DMR)
                err = dmrTX.writeData2(m_buffer + 3U, m_len - 3U);
            }
            if (err == 0U) {
              if (m_modemState == STATE_IDLE)
                setMode(STATE_DMR);
            } else {
              DEBUG2("Received invalid DMR data", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DMR_START:
            if (m_dmrEnable) {
              err = 4U;
              if (m_len == 4U) {
                if (m_buffer[3U] == 0x01U && m_modemState == STATE_DMR) {
                  if (!m_tx)
                    dmrTX.setStart(true);
                  err = 0U;
                } else if (m_buffer[3U] == 0x00U && m_modemState == STATE_DMR) {
                  if (m_tx)
                    dmrTX.setStart(false);
                  err = 0U;
                }
              }
            }
            if (err != 0U) {
              DEBUG2("Received invalid DMR start", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DMR_SHORTLC:
            if (m_dmrEnable)
              err = dmrTX.writeShortLC(m_buffer + 3U, m_len - 3U);
            if (err != 0U) {
              DEBUG2("Received invalid DMR Short LC", err);
              sendNAK(err);
            }
            break;

          case MMDVM_DMR_ABORT:
            if (m_dmrEnable)
              err = dmrTX.writeAbort(m_buffer + 3U, m_len - 3U);
            if (err != 0U) {
              DEBUG2("Received invalid DMR Abort", err);
              sendNAK(err);
            }
            break;

          case MMDVM_YSF_DATA:
            if (m_ysfEnable) {
              if (m_modemState == STATE_IDLE || m_modemState == STATE_YSF)
                err = ysfTX.writeData(m_buffer + 3U, m_len - 3U);
            }
            if (err == 0U) {
              if (m_modemState == STATE_IDLE)
                setMode(STATE_YSF);
            } else {
              DEBUG2("Received invalid System Fusion data", err);
              sendNAK(err);
            }
            break;

          default:
            // Handle this, send a NAK back
            sendNAK(1U);
            break;
        }

        m_ptr = 0U;
        m_len = 0U;
      }
    }
  }
}

void CSerialPort::writeDStarHeader(const uint8_t* header, uint8_t length)
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[50U];
  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DSTAR_HEADER;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = header[i];

  reply[1U] = count;

  write(reply, count);
}

void CSerialPort::writeDStarData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[20U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DSTAR_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  write(reply, count);
}

void CSerialPort::writeDStarLost()
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_DSTAR_LOST;

  write(reply, 3);
}

void CSerialPort::writeDStarEOT()
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_DSTAR_EOT;

  write(reply, 3);
}

void CSerialPort::writeDMRData(bool slot, const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_DMR && m_modemState != STATE_IDLE)
    return;

  if (!m_dmrEnable)
    return;

  uint8_t reply[40U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = slot ? MMDVM_DMR_DATA2 : MMDVM_DMR_DATA1;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  write(reply, count);
}

void CSerialPort::writeDMRLost(bool slot)
{
  if (m_modemState != STATE_DMR && m_modemState != STATE_IDLE)
    return;

  if (!m_dmrEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = slot ? MMDVM_DMR_LOST2 : MMDVM_DMR_LOST1;

  write(reply, 3);
}

void CSerialPort::writeYSFData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_YSF && m_modemState != STATE_IDLE)
    return;

  if (!m_ysfEnable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_YSF_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  write(reply, count);
}

void CSerialPort::writeYSFLost()
{
  if (m_modemState != STATE_YSF && m_modemState != STATE_IDLE)
    return;

  if (!m_ysfEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_YSF_LOST;

  write(reply, 3);
}

void CSerialPort::writeCalData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_DSTARCAL)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_CAL_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  write(reply, count);
}

void CSerialPort::write(const uint8_t* data, uint16_t length, bool flush)
{
#if defined(__MBED__)
  for (uint16_t i = 0U; i < length; i++)
    m_serial.putc(data[i]);
  // No explicit flush function
#else
  Serial.write(data, length);
  if (flush)
    Serial.flush();
#endif
}

void CSerialPort::writeDebug(const char* text)
{
  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG1;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[1U] = count;

  write(reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1)
{
  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG2;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[1U] = count;

  write(reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2)
{
  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG3;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[1U] = count;

  write(reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3)
{
  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG4;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[count++] = (n3 >> 8) & 0xFF;
  reply[count++] = (n3 >> 0) & 0xFF;

  reply[1U] = count;

  write(reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4)
{
  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG5;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[count++] = (n3 >> 8) & 0xFF;
  reply[count++] = (n3 >> 0) & 0xFF;

  reply[count++] = (n4 >> 8) & 0xFF;
  reply[count++] = (n4 >> 0) & 0xFF;

  reply[1U] = count;

  write(reply, count, true);
}

void CSerialPort::writeAssert(bool cond, const char* text, const char* file, long line)
{
  if (cond)
    return;

  uint8_t reply[200U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG2;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = ' ';
  
  for (uint8_t i = 0U; file[i] != '\0'; i++, count++)
    reply[count] = file[i];

  reply[count++] = (line >> 8) & 0xFF;
  reply[count++] = (line >> 0) & 0xFF;

  reply[1U] = count;

  write(reply, count, true);
}

