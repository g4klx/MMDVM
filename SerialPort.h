/*
 *   Copyright (C) 2015,2016,2017,2018 by Jonathan Naylor G4KLX
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
#include "SerialRB.h"


class CSerialPort {
public:
  CSerialPort();

  void start();

  void process();

  void writeDStarHeader(const uint8_t* header, uint8_t length);
  void writeDStarData(const uint8_t* data, uint8_t length);
  void writeDStarLost();
  void writeDStarEOT();

  void writeDMRData(bool slot, const uint8_t* data, uint8_t length);
  void writeDMRLost(bool slot);

  void writeYSFData(const uint8_t* data, uint8_t length);
  void writeYSFLost();

  void writeP25Hdr(const uint8_t* data, uint8_t length);
  void writeP25Ldu(const uint8_t* data, uint8_t length);
  void writeP25Lost();

  void writeNXDNData(const uint8_t* data, uint8_t length);
  void writeNXDNLost();

  void writeCalData(const uint8_t* data, uint8_t length);
  void writeRSSIData(const uint8_t* data, uint8_t length);

  void writeDebug(const char* text);
  void writeDebug(const char* text, int16_t n1);
  void writeDebug(const char* text, int16_t n1, int16_t n2);
  void writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3);
  void writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4);

private:
  uint8_t   m_buffer[256U];
  uint8_t   m_ptr;
  uint8_t   m_len;
  bool      m_debug;
  CSerialRB m_repeat;

  void    sendACK();
  void    sendNAK(uint8_t err);
  void    getStatus();
  void    getVersion();
  uint8_t setConfig(const uint8_t* data, uint8_t length);
  uint8_t setMode(const uint8_t* data, uint8_t length);
  void    setMode(MMDVM_STATE modemState);

  // Hardware versions
  void    beginInt(uint8_t n, int speed);
  int     availableInt(uint8_t n);
  int     availableForWriteInt(uint8_t n);
  uint8_t readInt(uint8_t n);
  void    writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush = false);
};

#endif

