/*
 *   Copyright (C) 2019,2020 by BG5HHP
 *   Copyright (C) 2024 by Jonathan Naylor, G4KLX
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

#if !defined(_PINS_ZUM_V10_PI_H)
#define _PINS_ZUM_V10_PI_H

/*
Pin definitions for KI6ZUM v1.0 Pi Board:

PTT      PB13   output
COSLED   PB14   output
LED      PB15   output
COS      PC0    input

Mode LEDs
DSTAR    PC7    output
DMR      PC8    output
YSF      PA8    output
P25      PC9    output
NXDN     PB1    output
POCSAG   PB12   output

Analogue input/output
RX       PA0    analogue input
RSSI     PA7    analogue input
TX       PA4    analogue output

Host communication
TXD      PA9
RXD      PA10

Serial repeater
TXD      PC12
RXD      PD2
*/

#define PIN_PTT           PB13

#define PIN_COSLED        PB14

#define PIN_LED           PB15

#define PIN_COS           PC0

#define PIN_DSTAR         PC7

#define PIN_DMR           PC8

#define PIN_YSF           PA8

#define PIN_P25           PC9

#define PIN_NXDN          PB1

#define PIN_POCSAG        PB12

#define PIN_RX            PA0

#define PIN_RSSI          PA7

#define PIN_TX            PA4

#define PIN_HOST_TXD      PA9

#define PIN_HOST_RXD      PA10

#define PIN_RPT_TXD       PC12

#define PIN_RPT_RXD       PD2

#endif
