/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021 by Jonathan Naylor G4KLX
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

// Global variables
MMDVM_STATE m_modemState = STATE_IDLE;

bool m_dstarEnable  = true;
bool m_dmrEnable    = true;
bool m_ysfEnable    = true;
bool m_p25Enable    = true;
bool m_nxdnEnable   = true;
bool m_m17Enable    = true;
bool m_pocsagEnable = true;
bool m_fmEnable     = true;
bool m_ax25Enable   = true;

bool m_duplex = true;

bool m_tx  = false;
bool m_dcd = false;

#if defined(MODE_DSTAR)
CDStarRX dstarRX;
CDStarTX dstarTX;

CCalDStarRX calDStarRX;
CCalDStarTX calDStarTX;
#endif

#if defined(MODE_DMR)
CDMRIdleRX dmrIdleRX;
CDMRRX dmrRX;
CDMRTX dmrTX;

CDMRDMORX dmrDMORX;
CDMRDMOTX dmrDMOTX;

CCalDMR calDMR;
#endif

#if defined(MODE_YSF)
CYSFRX ysfRX;
CYSFTX ysfTX;
#endif

#if defined(MODE_P25)
CP25RX p25RX;
CP25TX p25TX;

CCalP25 calP25;
#endif

#if defined(MODE_NXDN)
CNXDNRX nxdnRX;
CNXDNTX nxdnTX;

CCalNXDN calNXDN;
#endif

#if defined(MODE_M17)
CM17RX m17RX;
CM17TX m17TX;

CCalM17 calM17;
#endif

#if defined(MODE_POCSAG)
CPOCSAGTX  pocsagTX;
CCalPOCSAG calPOCSAG;
#endif

#if defined(MODE_FM)
CFM    fm;
CCalFM calFM;
#endif

#if defined(MODE_AX25)
CAX25RX ax25RX;
CAX25TX ax25TX;
#endif

CCalRSSI calRSSI;

CCWIdTX cwIdTX;

CSerialPort serial;
CIO io;

void setup()
{
  serial.start();
}

void loop()
{
  serial.process();
  
  io.process();

  // The following is for transmitting
#if defined(MODE_DSTAR)
  if (m_dstarEnable && m_modemState == STATE_DSTAR)
    dstarTX.process();
#endif

#if defined(MODE_DMR)
  if (m_dmrEnable && m_modemState == STATE_DMR) {
    if (m_duplex)
      dmrTX.process();
    else
      dmrDMOTX.process();
  }
#endif

#if defined(MODE_YSF)
  if (m_ysfEnable && m_modemState == STATE_YSF)
    ysfTX.process();
#endif

#if defined(MODE_P25)
  if (m_p25Enable && m_modemState == STATE_P25)
    p25TX.process();
#endif

#if defined(MODE_NXDN)
  if (m_nxdnEnable && m_modemState == STATE_NXDN)
    nxdnTX.process();
#endif

#if defined(MODE_M17)
  if (m_m17Enable && m_modemState == STATE_M17)
    m17TX.process();
#endif

#if defined(MODE_POCSAG)
  if (m_pocsagEnable && (m_modemState == STATE_POCSAG || pocsagTX.busy()))
    pocsagTX.process();
#endif

#if defined(MODE_AX25)
  if (m_ax25Enable && (m_modemState == STATE_IDLE || m_modemState == STATE_FM))
    ax25TX.process();
#endif

#if defined(MODE_FM)
  if (m_fmEnable && m_modemState == STATE_FM)
    fm.process();
#endif

#if defined(MODE_DSTAR)
  if (m_modemState == STATE_DSTARCAL)
    calDStarTX.process();
#endif

#if defined(MODE_DMR)
  if (m_modemState == STATE_DMRCAL || m_modemState == STATE_LFCAL || m_modemState == STATE_DMRCAL1K || m_modemState == STATE_DMRDMO1K)
    calDMR.process();
#endif

#if defined(MODE_FM)
  if (m_modemState == STATE_FMCAL10K || m_modemState == STATE_FMCAL12K || m_modemState == STATE_FMCAL15K || m_modemState == STATE_FMCAL20K || m_modemState == STATE_FMCAL25K || m_modemState == STATE_FMCAL30K)
    calFM.process();
#endif

#if defined(MODE_P25)
  if (m_modemState == STATE_P25CAL1K)
    calP25.process();
#endif

#if defined(MODE_NXDN)
  if (m_modemState == STATE_NXDNCAL1K)
    calNXDN.process();
#endif

#if defined(MODE_M17)
  if (m_modemState == STATE_M17CAL)
    calM17.process();
#endif

#if defined(MODE_POCSAG)
  if (m_modemState == STATE_POCSAGCAL)
    calPOCSAG.process();
#endif

  if (m_modemState == STATE_IDLE)
    cwIdTX.process();
}

