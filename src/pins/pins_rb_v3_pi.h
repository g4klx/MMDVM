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

#if !defined(_PINS_RB_V3_H)
#define _PINS_RB_V3_H

/*
Pin definitions for Repeater Builder rev 3 Board:

COS      PB13   input
PTT      PB12   output
COSLED   PB4    output
LED      PB3    output

Mode LEDs
P25      PB8    output
NXDN     PB9    output
DSTAR    PB6    output
DMR      PB5    output
YSF      PB7    output
FM       PB14   output

Analogue input/output
RX       PB0    analogue input
RSSI     PB1    analogue input
TX       PA4    analogue output

Host communication
TXD      PA9
RXD      PA10

Serial repeater
TXD      PC12
RXD      PD2
*/

#define PIN_COS           PB13

#define PIN_PTT           PB12

#define PIN_COSLED        PB4

#define PIN_LED           PB3

#define PIN_P25           PB8

#define PIN_NXDN          PB9

#define PIN_DSTAR         PB6

#define PIN_DMR           PB5

#define PIN_YSF           PB7

#define PIN_FM            PB14

#define PIN_RX            PB0

#define PIN_RSSI          PB1

#define PIN_TX            PA4

#define PIN_HOST_TXD      PA9

#define PIN_HOST_RXD      PA10

#define PIN_RPT_TXD       PC12

#define PIN_RPT_RXD       PD2

#endif
