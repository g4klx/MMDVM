/*
 *   Copyright (C) 2013,2015-2021 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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

#if defined(MADEBYMAKEFILE)
#include "GitVersion.h"
#endif

#include "SerialPort.h"
#include "Version.h"

const uint8_t MMDVM_FRAME_START  = 0xE0U;

const uint8_t MMDVM_GET_VERSION  = 0x00U;
const uint8_t MMDVM_GET_STATUS   = 0x01U;
const uint8_t MMDVM_SET_CONFIG   = 0x02U;
const uint8_t MMDVM_SET_MODE     = 0x03U;
const uint8_t MMDVM_SET_FREQ     = 0x04U;

const uint8_t MMDVM_CAL_DATA     = 0x08U;
const uint8_t MMDVM_RSSI_DATA    = 0x09U;

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

const uint8_t MMDVM_P25_HDR      = 0x30U;
const uint8_t MMDVM_P25_LDU      = 0x31U;
const uint8_t MMDVM_P25_LOST     = 0x32U;

const uint8_t MMDVM_NXDN_DATA    = 0x40U;
const uint8_t MMDVM_NXDN_LOST    = 0x41U;

const uint8_t MMDVM_M17_LINK_SETUP = 0x45U;
const uint8_t MMDVM_M17_STREAM     = 0x46U;
const uint8_t MMDVM_M17_PACKET     = 0x47U;
const uint8_t MMDVM_M17_LOST       = 0x48U;
const uint8_t MMDVM_M17_EOT        = 0x49U;

const uint8_t MMDVM_POCSAG_DATA  = 0x50U;

const uint8_t MMDVM_AX25_DATA    = 0x55U;

const uint8_t MMDVM_FM_PARAMS1   = 0x60U;
const uint8_t MMDVM_FM_PARAMS2   = 0x61U;
const uint8_t MMDVM_FM_PARAMS3   = 0x62U;
const uint8_t MMDVM_FM_PARAMS4   = 0x63U;
const uint8_t MMDVM_FM_DATA      = 0x65U;
const uint8_t MMDVM_FM_STATUS    = 0x66U;
const uint8_t MMDVM_FM_EOT       = 0x67U;

const uint8_t MMDVM_ACK          = 0x70U;
const uint8_t MMDVM_NAK          = 0x7FU;

const uint8_t MMDVM_SERIAL_DATA  = 0x80U;
const uint8_t MMDVM_I2C_DATA     = 0x81U;

const uint8_t MMDVM_TRANSPARENT  = 0x90U;
const uint8_t MMDVM_QSO_INFO     = 0x91U;

const uint8_t MMDVM_DEBUG1       = 0xF1U;
const uint8_t MMDVM_DEBUG2       = 0xF2U;
const uint8_t MMDVM_DEBUG3       = 0xF3U;
const uint8_t MMDVM_DEBUG4       = 0xF4U;
const uint8_t MMDVM_DEBUG5       = 0xF5U;
const uint8_t MMDVM_DEBUG_DUMP   = 0xFAU;

#if EXTERNAL_OSC == 12000000
#define TCXO "12.0000 MHz"
#elif EXTERNAL_OSC == 12288000
#define TCXO "12.2880 MHz"
#elif EXTERNAL_OSC == 14400000
#define TCXO "14.4000 MHz"
#elif EXTERNAL_OSC == 19200000
#define TCXO "19.2000 Mhz"
#else
#define TCXO "NO TCXO"
#endif

#if defined(DRCC_DVM_NQF)
#define	HW_TYPE	"MMDVM DRCC_DVM_NQF"
#elif defined(DRCC_DVM_HHP446)
#define	HW_TYPE	"MMDVM DRCC_DVM_HHP(446)"
#elif defined(DRCC_DVM_722)
#define HW_TYPE "MMDVM RB_STM32_DVM(722)"
#elif defined(DRCC_DVM_446)
#define HW_TYPE "MMDVM RB_STM32_DVM(446)"
#else
#define	HW_TYPE	"MMDVM"
#endif

#if defined(GITVERSION)
#define concat(h, a, b, c) h " " a " " b " GitID #" c ""
const char HARDWARE[] = concat(HW_TYPE, VERSION, TCXO, GITVERSION);
#else
#define concat(h, a, b, c, d) h " " a " " b " (Build: " c " " d ")"
const char HARDWARE[] = concat(HW_TYPE, VERSION, TCXO, __TIME__, __DATE__);
#endif

const uint8_t PROTOCOL_VERSION   = 2U;

// Parameters for batching serial data
const int      MAX_SERIAL_DATA  = 250;
const uint16_t MAX_SERIAL_COUNT = 100U;

CSerialPort::CSerialPort() :
m_buffer(),
m_ptr(0U),
m_len(0U),
m_debug(false),
m_serialData(),
m_lastSerialAvail(0),
m_lastSerialAvailCount(0U),
m_i2CData()
{
}

void CSerialPort::sendACK(uint8_t type)
{
  uint8_t reply[4U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = MMDVM_ACK;
  reply[3U] = type;

  writeInt(1U, reply, 4);
}

void CSerialPort::sendNAK(uint8_t type, uint8_t err)
{
  uint8_t reply[5U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 5U;
  reply[2U] = MMDVM_NAK;
  reply[3U] = type;
  reply[4U] = err;

  writeInt(1U, reply, 5);
}

void CSerialPort::getStatus()
{
  io.resetWatchdog();

  uint8_t reply[30U];

  // Send all sorts of interesting internal values
  reply[0U]  = MMDVM_FRAME_START;
  reply[1U]  = 20U;
  reply[2U]  = MMDVM_GET_STATUS;

  reply[3U]  = uint8_t(m_modemState);

  reply[4U]  = m_tx  ? 0x01U : 0x00U;

  bool adcOverflow;
  bool dacOverflow;
  io.getOverflow(adcOverflow, dacOverflow);

  if (adcOverflow)
    reply[4U] |= 0x02U;

  if (io.hasRXOverflow())
    reply[4U] |= 0x04U;

  if (io.hasTXOverflow())
    reply[4U] |= 0x08U;

  if (io.hasLockout())
    reply[4U] |= 0x10U;

  if (dacOverflow)
    reply[4U] |= 0x20U;
    
  reply[4U] |= m_dcd ? 0x40U : 0x00U;

  reply[5U] = 0x00U;

#if defined(MODE_DSTAR)
  if (m_dstarEnable)
    reply[6U] = dstarTX.getSpace();
  else
    reply[6U] = 0U;
#else
  reply[6U] = 0U;
#endif

#if defined(MODE_DMR)
  if (m_dmrEnable) {
    if (m_duplex) {
      reply[7U] = dmrTX.getSpace1();
      reply[8U] = dmrTX.getSpace2();
    } else {
      reply[7U] = 10U;
      reply[8U] = dmrDMOTX.getSpace();
    }
  } else {
    reply[7U] = 0U;
    reply[8U] = 0U;
  }
#else
  reply[7U] = 0U;
  reply[8U] = 0U;
#endif

#if defined(MODE_YSF)
  if (m_ysfEnable)
    reply[9U] = ysfTX.getSpace();
  else
    reply[9U] = 0U;
#else
  reply[9U] = 0U;
#endif

#if defined(MODE_P25)
  if (m_p25Enable)
    reply[10U] = p25TX.getSpace();
  else
    reply[10U] = 0U;
#else
  reply[10U] = 0U;
#endif

#if defined(MODE_NXDN)
  if (m_nxdnEnable)
    reply[11U] = nxdnTX.getSpace();
  else
    reply[11U] = 0U;
#else
  reply[11U] = 0U;
#endif

#if defined(MODE_M17)
  if (m_m17Enable)
    reply[12U] = m17TX.getSpace();
  else
    reply[12U] = 0U;
#else
  reply[12U] = 0U;
#endif

#if defined(MODE_FM)
  if (m_fmEnable)
    reply[13U] = fm.getSpace();
  else
    reply[13U] = 0U;
#else
  reply[13U] = 0U;
#endif

#if defined(MODE_POCSAG)
  if (m_pocsagEnable)
    reply[14U] = pocsagTX.getSpace();
  else
    reply[14U] = 0U;
#else
  reply[14U] = 0U;
#endif

#if defined(MODE_AX25)
  if (m_ax25Enable)
    reply[15U] = ax25TX.getSpace();
  else
    reply[15U] = 0U;
#else
  reply[15U] = 0U;
#endif

  reply[16U] = 0x00U;
  reply[17U] = 0x00U;
  reply[18U] = 0x00U;
  reply[19U] = 0x00U;

  writeInt(1U, reply, 20);
}

void CSerialPort::getVersion()
{
  uint8_t reply[200U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_GET_VERSION;

  reply[3U] = PROTOCOL_VERSION;

  // Return two bytes of mode capabilities
  reply[4U] = 0x00U;
#if defined(MODE_DSTAR)
  reply[4U] |= 0x01U;
#endif
#if defined(MODE_DMR)
  reply[4U] |= 0x02U;
#endif
#if defined(MODE_YSF)
  reply[4U] |= 0x04U;
#endif
#if defined(MODE_P25)
  reply[4U] |= 0x08U;
#endif
#if defined(MODE_NXDN)
  reply[4U] |= 0x10U;
#endif
#if defined(MODE_M17)
  reply[4U] |= 0x20U;
#endif
#if defined(MODE_FM)
  reply[4U] |= 0x40U;
#endif

  reply[5U] = 0x00U;
#if defined(MODE_POCSAG)
  reply[5U] |= 0x01U;
#endif
#if defined(MODE_AX25)
  reply[5U] |= 0x02U;
#endif

  // CPU type/manufacturer. 0=Atmel ARM, 1=NXP ARM, 2=St-Micro ARM
  reply[6U] = io.getCPU();

  // Reserve 16 bytes for the UDID
  ::memset(reply + 7U, 0x00U, 16U);
  io.getUDID(reply + 7U);

  uint8_t count = 23U;
  for (uint8_t i = 0U; HARDWARE[i] != 0x00U; i++, count++)
    reply[count] = HARDWARE[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

uint8_t CSerialPort::setConfig(const uint8_t* data, uint16_t length)
{
  if (length < 37U)
    return 4U;

  bool rxInvert        = (data[0U] & 0x01U) == 0x01U;
  bool txInvert        = (data[0U] & 0x02U) == 0x02U;
  bool pttInvert       = (data[0U] & 0x04U) == 0x04U;
#if defined(MODE_YSF)
  bool ysfLoDev        = (data[0U] & 0x08U) == 0x08U;
#endif
  bool useCOSAsLockout = (data[0U] & 0x20U) == 0x20U;
  bool simplex         = (data[0U] & 0x80U) == 0x80U;

  m_debug = (data[0U] & 0x10U) == 0x10U;

#if defined(MODE_DSTAR)
  bool dstarEnable  = (data[1U] & 0x01U) == 0x01U;
#endif
#if defined(MODE_DMR)
  bool dmrEnable    = (data[1U] & 0x02U) == 0x02U;
#endif
#if defined(MODE_YSF)
  bool ysfEnable    = (data[1U] & 0x04U) == 0x04U;
#endif
#if defined(MODE_P25)
  bool p25Enable    = (data[1U] & 0x08U) == 0x08U;
#endif
#if defined(MODE_NXDN)
  bool nxdnEnable   = (data[1U] & 0x10U) == 0x10U;
#endif
#if defined(MODE_FM)
  bool fmEnable     = (data[1U] & 0x20U) == 0x20U;
#endif
#if defined(MODE_M17)
  bool m17Enable    = (data[1U] & 0x40U) == 0x40U;
#endif
#if defined(MODE_POCSAG)
  bool pocsagEnable = (data[2U] & 0x01U) == 0x01U;
#endif
#if defined(MODE_AX25)
  bool ax25Enable   = (data[2U] & 0x02U) == 0x02U;
#endif

  uint8_t txDelay = data[3U];
  if (txDelay > 50U)
    return 4U;

  MMDVM_STATE modemState = MMDVM_STATE(data[4U]);

  if (modemState != STATE_IDLE && modemState != STATE_DSTAR && modemState != STATE_DMR && modemState != STATE_YSF && modemState != STATE_P25 && modemState != STATE_NXDN && modemState != STATE_M17 && modemState != STATE_POCSAG && modemState != STATE_FM &&
      modemState != STATE_DSTARCAL && modemState != STATE_DMRCAL && modemState != STATE_RSSICAL && modemState != STATE_LFCAL && modemState != STATE_DMRCAL1K && modemState != STATE_P25CAL1K && modemState != STATE_DMRDMO1K && modemState != STATE_NXDNCAL1K && modemState != STATE_M17CAL && modemState != STATE_POCSAGCAL &&
      modemState != STATE_FMCAL10K && modemState != STATE_FMCAL12K && modemState != STATE_FMCAL15K && modemState != STATE_FMCAL20K && modemState != STATE_FMCAL25K && modemState != STATE_FMCAL30K)
    return 4U;

#if defined(MODE_DSTAR)
  if (modemState == STATE_DSTAR && !dstarEnable)
    return 4U;
#else
  if (modemState == STATE_DSTAR)
    return 4U;
#endif

#if defined(MODE_DMR)
  if (modemState == STATE_DMR && !dmrEnable)
    return 4U;
#else
  if (modemState == STATE_DMR)
    return 4U;
#endif

#if defined(MODE_YSF)
  if (modemState == STATE_YSF && !ysfEnable)
    return 4U;
#else
  if (modemState == STATE_YSF)
    return 4U;
#endif

#if defined(MODE_P25)
  if (modemState == STATE_P25 && !p25Enable)
    return 4U;
#else
  if (modemState == STATE_P25)
    return 4U;
#endif

#if defined(MODE_NXDN)
  if (modemState == STATE_NXDN && !nxdnEnable)
    return 4U;
#else
  if (modemState == STATE_NXDN)
    return 4U;
#endif

#if defined(MODE_M17)
  if (modemState == STATE_M17 && !m17Enable)
    return 4U;
#else
  if (modemState == STATE_M17)
    return 4U;
#endif

#if defined(MODE_POCSAG)
  if (modemState == STATE_POCSAG && !pocsagEnable)
    return 4U;
#else
  if (modemState == STATE_POCSAG)
    return 4U;
#endif

#if defined(MODE_FM)
  if (modemState == STATE_FM && !fmEnable)
    return 4U;
#else
  if (modemState == STATE_FM)
    return 4U;
#endif

  int16_t txDCOffset = int16_t(data[5U]) - 128;
  int16_t rxDCOffset = int16_t(data[6U]) - 128;

  uint8_t rxLevel = data[7U];

  uint8_t cwIdTXLevel   = data[8U];
  uint8_t dstarTXLevel  = data[9U];
  uint8_t dmrTXLevel    = data[10U];
  uint8_t ysfTXLevel    = data[11U];
  uint8_t p25TXLevel    = data[12U];
  uint8_t nxdnTXLevel   = data[13U];
  uint8_t m17TXLevel    = data[14U];
  uint8_t pocsagTXLevel = data[15U];
  uint8_t fmTXLevel     = data[16U];
  uint8_t ax25TXLevel   = data[17U];

#if defined(MODE_YSF)
  uint8_t ysfTXHang     = data[20U];
#endif
#if defined(MODE_P25)
  uint8_t p25TXHang     = data[21U];
#endif
#if defined(MODE_NXDN)
  uint8_t nxdnTXHang    = data[22U];
#endif
#if defined(MODE_M17)
  uint8_t m17TXHang     = data[23U];
#endif

#if defined(MODE_DMR)
  uint8_t colorCode = data[26U];
  if (colorCode > 15U)
    return 4U;

  uint8_t dmrDelay = data[27U];
#endif

#if defined(MODE_AX25)
  int8_t ax25RXTwist    = int8_t(data[28U]) - 128;
  if (ax25RXTwist < -4 || ax25RXTwist > 10)
    return 4U;

  uint8_t ax25TXDelay   = data[29U];
  uint8_t ax25SlotTime  = data[30U];
  uint8_t ax25PPersist  = data[31U];
#endif

  setMode(modemState);

  m_duplex       = !simplex;

#if defined(MODE_DSTAR)
  m_dstarEnable  = dstarEnable;
  dstarTX.setTXDelay(txDelay);
#endif
#if defined(MODE_DMR)
  m_dmrEnable    = dmrEnable;
  dmrDMOTX.setTXDelay(txDelay);

  dmrTX.setColorCode(colorCode);
  dmrRX.setColorCode(colorCode);
  dmrRX.setDelay(dmrDelay);
  dmrDMORX.setColorCode(colorCode);
  dmrIdleRX.setColorCode(colorCode);
#endif
#if defined(MODE_YSF)
  m_ysfEnable    = ysfEnable;
  ysfTX.setTXDelay(txDelay);
  ysfTX.setParams(ysfLoDev, ysfTXHang);
#endif
#if defined(MODE_P25)
  m_p25Enable    = p25Enable;
  p25TX.setTXDelay(txDelay);
  p25TX.setParams(p25TXHang);
#endif
#if defined(MODE_NXDN)
  m_nxdnEnable   = nxdnEnable;
  nxdnTX.setTXDelay(txDelay);
  nxdnTX.setParams(nxdnTXHang);
#endif
#if defined(MODE_M17)
  m_m17Enable    = m17Enable;
  m17TX.setTXDelay(txDelay);
  m17TX.setParams(m17TXHang);
#endif
#if defined(MODE_POCSAG)
  m_pocsagEnable = pocsagEnable;
  pocsagTX.setTXDelay(txDelay);
#endif
#if defined(MODE_AX25)
  m_ax25Enable   = ax25Enable;
  ax25TX.setTXDelay(ax25TXDelay);
  ax25RX.setParams(ax25RXTwist, ax25SlotTime, ax25PPersist);
#endif
#if defined(MODE_FM)
  m_fmEnable     = fmEnable;
#endif

  io.setParameters(rxInvert, txInvert, pttInvert, rxLevel, cwIdTXLevel, dstarTXLevel, dmrTXLevel, ysfTXLevel, p25TXLevel, nxdnTXLevel, m17TXLevel, pocsagTXLevel, fmTXLevel, ax25TXLevel, txDCOffset, rxDCOffset, useCOSAsLockout);

  io.start();

  return 0U;
}

#if defined(MODE_FM)
uint8_t CSerialPort::setFMParams1(const uint8_t* data, uint16_t length)
{
  if (length < 8U)
    return 4U;

  uint8_t  speed     = data[0U];;
  uint16_t frequency = data[1U] * 10U;
  uint8_t  time      = data[2U];
  uint8_t  holdoff   = data[3U];
  uint8_t  highLevel = data[4U];
  uint8_t  lowLevel  = data[5U];

  bool callAtStart = (data[6U] & 0x01U) == 0x01U;
  bool callAtEnd   = (data[6U] & 0x02U) == 0x02U;
  bool callAtLatch = (data[6U] & 0x04U) == 0x04U;

  char callsign[50U];
  uint8_t n = 0U;
  for (uint8_t i = 7U; i < length; i++, n++)
    callsign[n] = data[i];
  callsign[n] = '\0';

  return fm.setCallsign(callsign, speed, frequency, time, holdoff, highLevel, lowLevel, callAtStart, callAtEnd, callAtLatch);
}

uint8_t CSerialPort::setFMParams2(const uint8_t* data, uint16_t length)
{
  if (length < 6U)
    return 4U;

  uint8_t  speed     = data[0U];
  uint16_t frequency = data[1U] * 10U;
  uint8_t  minTime   = data[2U];
  uint16_t delay     = data[3U] * 10U;
  uint8_t  level     = data[4U];

  char ack[50U];
  uint8_t n = 0U;
  for (uint8_t i = 5U; i < length; i++, n++)
    ack[n] = data[i];
  ack[n] = '\0';

  return fm.setAck(ack, speed, frequency, minTime, delay, level);
}

uint8_t CSerialPort::setFMParams3(const uint8_t* data, uint16_t length)
{
  if (length < 14U)
    return 4U;

  uint16_t timeout        = data[0U] * 5U;
  uint8_t  timeoutLevel   = data[1U];

  uint8_t  ctcssFrequency     = data[2U];
  uint8_t  ctcssHighThreshold = data[3U];
  uint8_t  ctcssLowThreshold  = data[4U];
  uint8_t  ctcssLevel         = data[5U];

  uint8_t  kerchunkTime   = data[6U];
  uint8_t  hangTime       = data[7U];

  uint8_t  accessMode     = data[8U] & 0x0FU;
  bool     linkMode       = (data[8U] & 0x20U) == 0x20U;
  bool     noiseSquelch   = (data[8U] & 0x40U) == 0x40U;
  bool     cosInvert      = (data[8U] & 0x80U) == 0x80U;

  uint8_t  rfAudioBoost   = data[9U];
  uint8_t  maxDev         = data[10U];
  uint8_t  rxLevel        = data[11U];

  uint8_t  squelchHighThreshold = data[12U];
  uint8_t  squelchLowThreshold  = data[13U];

  return fm.setMisc(timeout, timeoutLevel, ctcssFrequency, ctcssHighThreshold, ctcssLowThreshold, ctcssLevel, kerchunkTime, hangTime, accessMode, linkMode, cosInvert, noiseSquelch, squelchHighThreshold, squelchLowThreshold, rfAudioBoost, maxDev, rxLevel);
}

uint8_t CSerialPort::setFMParams4(const uint8_t* data, uint16_t length)
{
  if (length < 4U)
    return 4U;

  uint8_t  audioBoost = data[0U];
  uint8_t  speed      = data[1U];
  uint16_t frequency  = data[2U] * 10U;
  uint8_t  level      = data[3U];

  char ack[50U];
  uint8_t n = 0U;
  for (uint8_t i = 4U; i < length; i++, n++)
    ack[n] = data[i];
  ack[n] = '\0';

  return fm.setExt(ack, audioBoost, speed, frequency, level);
}
#endif

uint8_t CSerialPort::setMode(const uint8_t* data, uint16_t length)
{
  if (length < 1U)
    return 4U;

  MMDVM_STATE modemState = MMDVM_STATE(data[0U]);

  if (modemState == m_modemState)
    return 0U;

  if (modemState != STATE_IDLE && modemState != STATE_DSTAR && modemState != STATE_DMR && modemState != STATE_YSF && modemState != STATE_P25 && modemState != STATE_NXDN && modemState != STATE_M17 && modemState != STATE_POCSAG && modemState != STATE_FM &&
      modemState != STATE_DSTARCAL && modemState != STATE_DMRCAL && modemState != STATE_RSSICAL && modemState != STATE_LFCAL && modemState != STATE_DMRCAL1K && modemState != STATE_P25CAL1K && modemState != STATE_DMRDMO1K && modemState != STATE_NXDNCAL1K && modemState != STATE_M17CAL && modemState != STATE_POCSAGCAL &&
      modemState != STATE_FMCAL10K  && modemState != STATE_FMCAL12K && modemState != STATE_FMCAL15K && modemState != STATE_FMCAL20K && modemState != STATE_FMCAL25K && modemState != STATE_FMCAL30K)
    return 4U;

#if defined(MODE_DSTAR)
  if (modemState == STATE_DSTAR && !m_dstarEnable)
    return 4U;
#else
  if (modemState == STATE_DSTAR)
    return 4U;
#endif

#if defined(MODE_DMR)
  if (modemState == STATE_DMR && !m_dmrEnable)
    return 4U;
#else
  if (modemState == STATE_DMR)
    return 4U;
#endif

#if defined(MODE_YSF)
  if (modemState == STATE_YSF && !m_ysfEnable)
    return 4U;
#else
  if (modemState == STATE_YSF)
    return 4U;
#endif

#if defined(MODE_P25)
  if (modemState == STATE_P25 && !m_p25Enable)
    return 4U;
#else
  if (modemState == STATE_P25)
    return 4U;
#endif

#if defined(MODE_NXDN)
  if (modemState == STATE_NXDN && !m_nxdnEnable)
    return 4U;
#else
  if (modemState == STATE_NXDN)
    return 4U;
#endif

#if defined(MODE_M17)
  if (modemState == STATE_M17 && !m_m17Enable)
    return 4U;
#else
  if (modemState == STATE_M17)
    return 4U;
#endif

#if defined(MODE_POCSAG)
  if (modemState == STATE_POCSAG && !m_pocsagEnable)
    return 4U;
#else
  if (modemState == STATE_POCSAG)
    return 4U;
#endif

#if defined(MODE_FM)
  if (modemState == STATE_FM && !m_fmEnable)
    return 4U;
#else
  if (modemState == STATE_FM)
    return 4U;
#endif

  setMode(modemState);

  return 0U;
}

void CSerialPort::setMode(MMDVM_STATE modemState)
{
  switch (modemState) {
    case STATE_DSTAR:
      DEBUG1("Mode set to D-Star");
      break;
    case STATE_DMR:
      DEBUG1("Mode set to DMR");
      break;
    case STATE_YSF:
      DEBUG1("Mode set to System Fusion");
      break;
    case STATE_P25:
      DEBUG1("Mode set to P25");
      break;
    case STATE_NXDN:
      DEBUG1("Mode set to NXDN");
      break;
    case STATE_M17:
      DEBUG1("Mode set to M17");
      break;
    case STATE_POCSAG:
      DEBUG1("Mode set to POCSAG");
      break;
    case STATE_FM:
      DEBUG1("Mode set to FM");
      break;
    case STATE_DSTARCAL:
      DEBUG1("Mode set to D-Star Calibrate");
      break;
    case STATE_DMRCAL:
      DEBUG1("Mode set to DMR Calibrate");
      break;
    case STATE_RSSICAL:
      DEBUG1("Mode set to RSSI Calibrate");
      break;
    case STATE_LFCAL:
      DEBUG1("Mode set to 80 Hz Calibrate");
      break;
    case STATE_FMCAL10K:
      DEBUG1("Mode set to FM 10Khz Calibrate");
      break;
    case STATE_FMCAL12K:
      DEBUG1("Mode set to FM 12.5Khz Calibrate");
      break;
    case STATE_FMCAL15K:
      DEBUG1("Mode set to FM 15Khz Calibrate");
      break;
    case STATE_FMCAL20K:
      DEBUG1("Mode set to FM 20Khz Calibrate");
      break;
    case STATE_FMCAL25K:
      DEBUG1("Mode set to FM 10Khz Calibrate");
      break;
    case STATE_FMCAL30K:
      DEBUG1("Mode set to FM 30Khz Calibrate");
      break;
    case STATE_P25CAL1K:
      DEBUG1("Mode set to P25 1011 Hz Calibrate");
      break;
    case STATE_DMRDMO1K:
      DEBUG1("Mode set to DMR MS 1031 Hz Calibrate");
      break;
    case STATE_NXDNCAL1K:
      DEBUG1("Mode set to NXDN 1031 Hz Calibrate");
      break;
    case STATE_POCSAGCAL:
      DEBUG1("Mode set to POCSAG Calibrate");
      break;
    case STATE_M17CAL:
      DEBUG1("Mode set to M17 Calibrate");
      break;
    default:        // STATE_IDLE
      DEBUG1("Mode set to Idle");
      break;
  }

#if defined(MODE_DSTAR)
  if (modemState != STATE_DSTAR)
    dstarRX.reset();
#endif

#if defined(MODE_DMR)
  if (modemState != STATE_DMR) {
    dmrIdleRX.reset();
    dmrDMORX.reset();
    dmrRX.reset();
  }
#endif

#if defined(MODE_YSF)
  if (modemState != STATE_YSF)
    ysfRX.reset();
#endif

#if defined(MODE_P25)
  if (modemState != STATE_P25)
    p25RX.reset();
#endif

#if defined(MODE_NXDN)
  if (modemState != STATE_NXDN)
    nxdnRX.reset();
#endif

#if defined(MODE_M17)
  if (modemState != STATE_M17)
    m17RX.reset();
#endif

#if defined(MODE_FM)
  if (modemState != STATE_FM)
    fm.reset();
#endif

  cwIdTX.reset();

  io.setMode(modemState);
}

void CSerialPort::start()
{
  beginInt(1U, SERIAL_SPEED);

#if defined(SERIAL_REPEATER)
  #if defined(SERIAL_REPEATER_BAUD_RATE)
    beginInt(3U, SERIAL_REPEATER_BAUD_RATE);
  #else
    beginInt(3U, 9600);
  #endif
#endif
#if defined(I2C_REPEATER)
  beginInt(10U, 9600);
#endif
}

void CSerialPort::process()
{
  while (availableForReadInt(1U)) {
    uint8_t c = readInt(1U);

    if (m_ptr == 0U) {
      if (c == MMDVM_FRAME_START) {
        // Handle the frame start correctly
        m_buffer[0U] = c;
        m_ptr = 1U;
        m_len = 0U;
      } else {
        m_ptr = 0U;
        m_len = 0U;
      }
    } else if (m_ptr == 1U) {
      // Handle the frame length, 1/2
      m_len = m_buffer[m_ptr] = c;
      m_ptr = 2U;
    } else if (m_ptr == 2U) {
      // Handle the frame length, 2/2
      m_buffer[m_ptr] = c;
      m_ptr = 3U;

      if (m_len == 0U)
        m_len = c + 255U;

      // The full packet has been received, process it
      if (m_ptr == m_len)
        processMessage(m_buffer[2U], m_buffer + 3U, m_len - 3U);
    } else {
      // Any other bytes are added to the buffer
      m_buffer[m_ptr] = c;
      m_ptr++;

      // The full packet has been received, process it
      if (m_ptr == m_len) {
        if (m_len > 255U)
          processMessage(m_buffer[3U], m_buffer + 4U, m_len - 4U);
        else
          processMessage(m_buffer[2U], m_buffer + 3U, m_len - 3U);
      }
    }
  }

  if (io.getWatchdog() >= 48000U) {
    m_ptr = 0U;
    m_len = 0U;
  }

#if defined(SERIAL_REPEATER)
  // Write any outgoing serial data
  uint16_t serialSpace = m_serialData.getData();
  if (serialSpace > 0U) {
    int avail = availableForWriteInt(3U);
    if (avail < serialSpace)
      serialSpace = avail;

    for (uint16_t i = 0U; i < serialSpace; i++) {
      uint8_t c = 0U;
      m_serialData.get(c);
      writeInt(3U, &c, 1U);
    }
  }

  // Read any incoming serial data, and send out in batches
  int serialAvail = availableForReadInt(3U);
  if ((serialAvail > 0 && serialAvail == m_lastSerialAvail && m_lastSerialAvailCount >= MAX_SERIAL_COUNT) || (serialAvail >= MAX_SERIAL_DATA)) {
    uint8_t buffer[MAX_SERIAL_DATA];
    for (int i = 0; i < serialAvail && i < MAX_SERIAL_DATA; i++) {
      buffer[i] = readInt(3U);
      m_lastSerialAvail--;
    }
    writeSerialData(buffer, serialAvail - m_lastSerialAvail);
    m_lastSerialAvailCount = 0U;
  } else if (serialAvail > 0 && serialAvail == m_lastSerialAvail) {
    m_lastSerialAvailCount++;
  } else {
    m_lastSerialAvail      = serialAvail;
    m_lastSerialAvailCount = 0U;
  }
#endif

#if defined(I2C_REPEATER)
  // Write any outgoing serial data
  uint16_t i2CSpace = m_i2CData.getData();
  if (i2CSpace > 0U) {
    int avail = availableForWriteInt(10U);
    if (avail < i2CSpace)
      i2CSpace = avail;

    for (uint16_t i = 0U; i < i2CSpace; i++) {
      uint8_t c = 0U;
      m_i2CData.get(c);
      writeInt(10U, &c, 1U);
    }
  }
#endif
}

void CSerialPort::processMessage(uint8_t type, const uint8_t* buffer, uint16_t length)
{
  uint8_t err = 2U;

  switch (type) {
    case MMDVM_GET_STATUS:
      getStatus();
      break;

    case MMDVM_GET_VERSION:
      getVersion();
      break;

    case MMDVM_SET_CONFIG:
      err = setConfig(buffer, length);
      if (err == 0U)
        sendACK(type);
      else
        sendNAK(type, err);
      break;

    case MMDVM_SET_MODE:
      err = setMode(buffer, length);
      if (err == 0U)
        sendACK(type);
      else
        sendNAK(type, err);
      break;

    case MMDVM_SET_FREQ:
      sendACK(type);
      break;

#if defined(MODE_FM)
    case MMDVM_FM_PARAMS1:
      err = setFMParams1(buffer, length);
      if (err == 0U) {
        sendACK(type);
      } else {
        DEBUG2("Received invalid FM params 1", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_FM_PARAMS2:
      err = setFMParams2(buffer, length);
      if (err == 0U) {
        sendACK(type);
      } else {
        DEBUG2("Received invalid FM params 2", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_FM_PARAMS3:
      err = setFMParams3(buffer, length);
      if (err == 0U) {
        sendACK(type);
      } else {
        DEBUG2("Received invalid FM params 3", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_FM_PARAMS4:
      err = setFMParams4(buffer, length);
      if (err == 0U) {
        sendACK(type);
      } else {
        DEBUG2("Received invalid FM params 4", err);
        sendNAK(type, err);
      }
      break;
#else
    case MMDVM_FM_PARAMS1:
    case MMDVM_FM_PARAMS2:
    case MMDVM_FM_PARAMS3:
    case MMDVM_FM_PARAMS4:
      sendACK(type);
      break;
#endif

    case MMDVM_CAL_DATA:
#if defined(MODE_DSTAR)
      if (m_modemState == STATE_DSTARCAL)
        err = calDStarTX.write(buffer, length);
#endif
#if defined(MODE_DMR)
      if (m_modemState == STATE_DMRCAL || m_modemState == STATE_LFCAL || m_modemState == STATE_DMRCAL1K || m_modemState == STATE_DMRDMO1K)
        err = calDMR.write(buffer, length);
#endif
#if defined(MODE_FM)
      if (m_modemState == STATE_FMCAL10K || m_modemState == STATE_FMCAL12K || m_modemState == STATE_FMCAL15K || m_modemState == STATE_FMCAL20K || m_modemState == STATE_FMCAL25K || m_modemState == STATE_FMCAL30K)
        err = calFM.write(buffer, length);
#endif
#if defined(MODE_P25)
      if (m_modemState == STATE_P25CAL1K)
        err = calP25.write(buffer, length);
#endif
#if defined(MODE_NXDN)
      if (m_modemState == STATE_NXDNCAL1K)
        err = calNXDN.write(buffer, length);
#endif
#if defined(MODE_M17)
      if (m_modemState == STATE_M17CAL)
        err = calM17.write(buffer, length);
#endif
#if defined(MODE_POCSAG)
      if (m_modemState == STATE_POCSAGCAL)
        err = calPOCSAG.write(buffer, length);
#endif
      if (err == 0U) {
        sendACK(type);
      } else {
        DEBUG2("Received invalid calibration data", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_SEND_CWID:
      err = 5U;
      if (m_modemState == STATE_IDLE)
        err = cwIdTX.write(buffer, length);
      if (err != 0U) {
        DEBUG2("Invalid CW Id data", err);
        sendNAK(type, err);
      }
      break;

#if defined(MODE_DSTAR)
    case MMDVM_DSTAR_HEADER:
      if (m_dstarEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_DSTAR)
          err = dstarTX.writeHeader(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_DSTAR);
      } else {
        DEBUG2("Received invalid D-Star header", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_DSTAR_DATA:
      if (m_dstarEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_DSTAR)
          err = dstarTX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_DSTAR);
      } else {
        DEBUG2("Received invalid D-Star data", err);
        sendNAK(type, err);
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
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_DMR)
    case MMDVM_DMR_DATA1:
      if (m_dmrEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_DMR) {
          if (m_duplex)
            err = dmrTX.writeData1(buffer, length);
        }
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_DMR);
      } else {
        DEBUG2("Received invalid DMR data", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_DMR_DATA2:
      if (m_dmrEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_DMR) {
          if (m_duplex)
            err = dmrTX.writeData2(buffer, length);
          else
            err = dmrDMOTX.writeData(buffer, length);
        }
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_DMR);
      } else {
        DEBUG2("Received invalid DMR data", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_DMR_START:
      if (m_dmrEnable) {
        err = 4U;
        if (length == 1U) {
          if (buffer[0U] == 0x01U && m_modemState == STATE_DMR) {
            if (!m_tx)
              dmrTX.setStart(true);
            err = 0U;
          } else if (buffer[0U] == 0x00U && m_modemState == STATE_DMR) {
            if (m_tx)
              dmrTX.setStart(false);
            err = 0U;
          }
        }
      }
      if (err != 0U) {
        DEBUG2("Received invalid DMR start", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_DMR_SHORTLC:
      if (m_dmrEnable)
        err = dmrTX.writeShortLC(buffer, length);
      if (err != 0U) {
        DEBUG2("Received invalid DMR Short LC", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_DMR_ABORT:
      if (m_dmrEnable)
        err = dmrTX.writeAbort(buffer, length);
      if (err != 0U) {
        DEBUG2("Received invalid DMR Abort", err);
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_YSF)
    case MMDVM_YSF_DATA:
      if (m_ysfEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_YSF)
          err = ysfTX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_YSF);
      } else {
        DEBUG2("Received invalid System Fusion data", err);
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_P25)
    case MMDVM_P25_HDR:
      if (m_p25Enable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_P25)
          err = p25TX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_P25);
      } else {
        DEBUG2("Received invalid P25 header", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_P25_LDU:
      if (m_p25Enable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_P25)
          err = p25TX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_P25);
      } else {
        DEBUG2("Received invalid P25 LDU", err);
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_NXDN)
    case MMDVM_NXDN_DATA:
      if (m_nxdnEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_NXDN)
          err = nxdnTX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_NXDN);
      } else {
        DEBUG2("Received invalid NXDN data", err);
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_M17)
    case MMDVM_M17_LINK_SETUP:
      if (m_m17Enable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_M17)
          err = m17TX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_M17);
      } else {
        DEBUG2("Received invalid M17 link setup data", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_M17_STREAM:
      if (m_m17Enable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_M17)
          err = m17TX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_M17);
      } else {
        DEBUG2("Received invalid M17 stream data", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_M17_EOT:
      if (m_m17Enable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_M17)
          err = m17TX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_M17);
      } else {
        DEBUG2("Received invalid M17 EOT", err);
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_POCSAG)
    case MMDVM_POCSAG_DATA:
      if (m_pocsagEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_POCSAG)
          err = pocsagTX.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_POCSAG);
      } else {
        DEBUG2("Received invalid POCSAG data", err);
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_FM)
    case MMDVM_FM_DATA:
      if (m_fmEnable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_FM)
          err = fm.writeData(buffer, length);
      }
      if (err == 0U) {
        if (m_modemState == STATE_IDLE)
          setMode(STATE_FM);
      } else {
        DEBUG2("Received invalid FM data", err);
        sendNAK(type, err);
      }
      break;
#endif

#if defined(MODE_AX25)
    case MMDVM_AX25_DATA:
      if (m_ax25Enable) {
        if (m_modemState == STATE_IDLE || m_modemState == STATE_FM)
          err = ax25TX.writeData(buffer, length);
      }
      if (err != 0U) {
        DEBUG2("Received invalid AX.25 data", err);
        sendNAK(type, err);
      }
      break;
#endif

    case MMDVM_TRANSPARENT:
    case MMDVM_QSO_INFO:
      // Do nothing on the MMDVM.
      break;

#if defined(SERIAL_REPEATER)
    case MMDVM_SERIAL_DATA: {
      for (uint16_t i = 0U; i < length; i++)
        m_serialData.put(buffer[i]);
      }
      break;
#endif

#if defined(I2C_REPEATER)
    case MMDVM_I2C_DATA: {
      for (uint16_t i = 0U; i < length; i++)
        m_i2CData.put(buffer[i]);
      }
      break;
#endif

    default:
      // Handle this, send a NAK back
      sendNAK(type, 1U);
      break;
  }

  m_ptr = 0U;
  m_len = 0U;
}

#if defined(MODE_DSTAR)
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

  writeInt(1U, reply, count);
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

  writeInt(1U, reply, count);
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

  writeInt(1U, reply, 3);
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

  writeInt(1U, reply, 3);
}
#endif

#if defined(MODE_DMR)
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

  writeInt(1U, reply, count);
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

  writeInt(1U, reply, 3);
}
#endif

#if defined(MODE_YSF)
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

  writeInt(1U, reply, count);
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

  writeInt(1U, reply, 3);
}
#endif

#if defined(MODE_P25)
void CSerialPort::writeP25Hdr(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_P25 && m_modemState != STATE_IDLE)
    return;

  if (!m_p25Enable)
    return;

  uint8_t reply[120U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_P25_HDR;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeP25Ldu(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_P25 && m_modemState != STATE_IDLE)
    return;

  if (!m_p25Enable)
    return;

  uint8_t reply[250U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_P25_LDU;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeP25Lost()
{
  if (m_modemState != STATE_P25 && m_modemState != STATE_IDLE)
    return;

  if (!m_p25Enable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_P25_LOST;

  writeInt(1U, reply, 3);
}
#endif

#if defined(MODE_NXDN)
void CSerialPort::writeNXDNData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_NXDN && m_modemState != STATE_IDLE)
    return;

  if (!m_nxdnEnable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_NXDN_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeNXDNLost()
{
  if (m_modemState != STATE_NXDN && m_modemState != STATE_IDLE)
    return;

  if (!m_nxdnEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_NXDN_LOST;

  writeInt(1U, reply, 3);
}
#endif

#if defined(MODE_M17)
void CSerialPort::writeM17LinkSetup(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_M17_LINK_SETUP;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeM17Stream(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_M17_STREAM;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeM17EOT()
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_M17_EOT;

  writeInt(1U, reply, 3);
}

void CSerialPort::writeM17Lost()
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_M17_LOST;

  writeInt(1U, reply, 3);
}
#endif

#if defined(MODE_FM)
void CSerialPort::writeFMData(const uint8_t* data, uint16_t length)
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_fmEnable)
    return;

  uint8_t reply[512U];

  reply[0U] = MMDVM_FRAME_START;

  if (length > 252U) {
    reply[1U] = 0U;
    reply[2U] = (length + 4U) - 255U;
    reply[3U] = MMDVM_FM_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 4U] = data[i];

    writeInt(1U, reply, length + 4U);
  } else {
    reply[1U] = length + 3U;
    reply[2U] = MMDVM_FM_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 3U] = data[i];

    writeInt(1U, reply, length + 3U);
  }
}

void CSerialPort::writeFMStatus(uint8_t status)
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_fmEnable)
    return;

  uint8_t reply[10U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = MMDVM_FM_STATUS;
  reply[3U] = status;

  writeInt(1U, reply, 4U);
}

void CSerialPort::writeFMEOT()
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_fmEnable)
    return;

  uint8_t reply[10U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_FM_EOT;

  writeInt(1U, reply, 3U);
}
#endif

#if defined(MODE_AX25)
void CSerialPort::writeAX25Data(const uint8_t* data, uint16_t length)
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_ax25Enable)
    return;

  uint8_t reply[512U];

  reply[0U] = MMDVM_FRAME_START;

  if (length > 252U) {
    reply[1U] = 0U;
    reply[2U] = (length + 4U) - 255U;
    reply[3U] = MMDVM_AX25_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 4U] = data[i];

    writeInt(1U, reply, length + 4U);
  } else {
    reply[1U] = length + 3U;
    reply[2U] = MMDVM_AX25_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 3U] = data[i];

    writeInt(1U, reply, length + 3U);
  }
}
#endif

#if defined(SERIAL_REPEATER)
void CSerialPort::writeSerialData(const uint8_t* data, uint8_t length)
{
  uint8_t reply[255U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_SERIAL_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}
#endif

#if defined(I2C_REPEATER)
void CSerialPort::writeI2CData(const uint8_t* data, uint8_t length)
{
  uint8_t reply[255U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_I2C_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}
#endif

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

  writeInt(1U, reply, count);
}

void CSerialPort::writeRSSIData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_RSSICAL)
    return;

  uint8_t reply[30U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_RSSI_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDebug(const char* text)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG1;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1)
{
  if (!m_debug)
    return;

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

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2)
{
  if (!m_debug)
    return;

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

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3)
{
  if (!m_debug)
    return;

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

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4)
{
  if (!m_debug)
    return;

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

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebugDump(const uint8_t* data, uint16_t length)
{
  uint8_t reply[512U];

  reply[0U] = MMDVM_FRAME_START;

  if (length > 252U) {
    reply[1U] = 0U;
    reply[2U] = (length + 4U) - 255U;
    reply[3U] = MMDVM_DEBUG_DUMP;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 4U] = data[i];

    writeInt(1U, reply, length + 4U);
  } else {
    reply[1U] = length + 3U;
    reply[2U] = MMDVM_DEBUG_DUMP;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 3U] = data[i];

    writeInt(1U, reply, length + 3U);
  }
}
