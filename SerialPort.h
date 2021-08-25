/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021 by Jonathan Naylor G4KLX
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

#if !defined(SERIALPORT_H)
#define  SERIALPORT_H

#include "Config.h"
#include "Globals.h"
#include "RingBuffer.h"

#if !defined(SERIAL_SPEED)
#define SERIAL_SPEED 115200
#endif


class CSerialPort {
public:
  CSerialPort();

  void start();

  void process();

#if defined(MODE_DSTAR)
  void writeDStarHeader(const uint8_t* header, uint8_t length);
  void writeDStarData(const uint8_t* data, uint8_t length);
  void writeDStarLost();
  void writeDStarEOT();
#endif

#if defined(MODE_DMR)
  void writeDMRData(bool slot, const uint8_t* data, uint8_t length);
  void writeDMRLost(bool slot);
#endif

#if defined(MODE_YSF)
  void writeYSFData(const uint8_t* data, uint8_t length);
  void writeYSFLost();
#endif

#if defined(MODE_P25)
  void writeP25Hdr(const uint8_t* data, uint8_t length);
  void writeP25Ldu(const uint8_t* data, uint8_t length);
  void writeP25Lost();
#endif

#if defined(MODE_NXDN)
  void writeNXDNData(const uint8_t* data, uint8_t length);
  void writeNXDNLost();
#endif

#if defined(MODE_M17)
  void writeM17LinkSetup(const uint8_t* data, uint8_t length);
  void writeM17Stream(const uint8_t* data, uint8_t length);
  void writeM17Lost();
  void writeM17EOT();
#endif

#if defined(MODE_AX25)
  void writeAX25Data(const uint8_t* data, uint16_t length);
#endif

#if defined(MODE_FM)
  void writeFMData(const uint8_t* data, uint16_t length);
  void writeFMStatus(uint8_t status);
  void writeFMEOT();
#endif

#if defined(SERIAL_REPEATER)
  void writeSerialData(const uint8_t* data, uint8_t length);
#endif

#if defined(I2C_REPEATER)
  void writeI2CData(const uint8_t* data, uint8_t length);
#endif

  void writeCalData(const uint8_t* data, uint8_t length);
  void writeRSSIData(const uint8_t* data, uint8_t length);

  void writeDebug(const char* text);
  void writeDebug(const char* text, int16_t n1);
  void writeDebug(const char* text, int16_t n1, int16_t n2);
  void writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3);
  void writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4);
  void writeDebugDump(const uint8_t* data, uint16_t length);

private:
  uint8_t   m_buffer[512U];
  uint16_t  m_ptr;
  uint16_t  m_len;
  bool      m_debug;
  CRingBuffer<uint8_t> m_serialData;
  int       m_lastSerialAvail;
  uint16_t  m_lastSerialAvailCount;
  CRingBuffer<uint8_t> m_i2CData;

  void    sendACK(uint8_t type);
  void    sendNAK(uint8_t type, uint8_t err);
  void    getStatus();
  void    getVersion();
  uint8_t setConfig(const uint8_t* data, uint16_t length);
  uint8_t setMode(const uint8_t* data, uint16_t length);
  void    setMode(MMDVM_STATE modemState);
  void    processMessage(uint8_t type, const uint8_t* data, uint16_t length);

#if defined(MODE_FM)
  uint8_t setFMParams1(const uint8_t* data, uint16_t length);
  uint8_t setFMParams2(const uint8_t* data, uint16_t length);
  uint8_t setFMParams3(const uint8_t* data, uint16_t length);
  uint8_t setFMParams4(const uint8_t* data, uint16_t length);
#endif

  // Hardware versions
  void    beginInt(uint8_t n, int speed);
  int     availableForReadInt(uint8_t n);
  int     availableForWriteInt(uint8_t n);
  uint8_t readInt(uint8_t n);
  void    writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush = false);
};

#endif
