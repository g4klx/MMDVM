/*
 *   Copyright (C) 2019,2020 by BG5HHP
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

#ifndef _PINS_F4_DRCC_HHP446_H
#define _PINS_F4_DRCC_HHP446_H

/*
Pin definitions for DRCC_DVM BG5HHP F446 board rev1

TX/PTT_LED  PB12   output
RX/COS_LED  PB5    output
STATUS_LED  PB10   output

COS_IN      PB13   input

DSTAR       N/A
DMR         N/A
YSF         N/A
P25         N/A
NXDN        N/A
POCSAG      N/A

MDMR/BIT0   PB8    output
MYSF/BIT1   PB9    output
MDSTAR/BIT2 PB14   output
MP25/BIT3   PB15   output          Generic Mode Pins
MNXDN       N/A
MPOCSAG     N/A

RX          PA0    analog input
RSSI        PA1    analog input
TX          PA4    analog output

EXT_CLK     N/A

UART1_TX    PA9    output
UART1_RX    PA10   output          Host Data Communication

UART2_TX    PA2    output
UART2_RX    PA3    output          Nextion Data Communication

I2C1_SCL    PB6    output
I2C1_SDA    PB7    output          OLED Data Communication as master

*/

#define PIN_COS           GPIO_Pin_13
#define PORT_COS          GPIOB
#define RCC_Per_COS       RCC_AHB1Periph_GPIOB

#define PIN_PTT           GPIO_Pin_12
#define PORT_PTT          GPIOB
#define RCC_Per_PTT       RCC_AHB1Periph_GPIOB

#define PIN_COSLED        GPIO_Pin_5
#define PORT_COSLED       GPIOB
#define RCC_Per_COSLED    RCC_AHB1Periph_GPIOB

#define PIN_LED           GPIO_Pin_10
#define PORT_LED          GPIOB
#define RCC_Per_LED       RCC_AHB1Periph_GPIOB

#define PIN_TXLED         GPIO_Pin_4
#define PORT_TXLED        GPIOB
#define RCC_Per_TXLED     RCC_AHB1Periph_GPIOB

// #define PIN_P25           GPIO_Pin_3
// #define PORT_P25          GPIOB
// #define RCC_Per_P25       RCC_AHB1Periph_GPIOB

// #define PIN_NXDN          GPIO_Pin_10
// #define PORT_NXDN         GPIOA
// #define RCC_Per_NXDN      RCC_AHB1Periph_GPIOA

// #define PIN_POCSAG        GPIO_Pin_12
// #define PORT_POCSAG       GPIOB
// #define RCC_Per_POCSAG    RCC_AHB1Periph_GPIOB

// #define PIN_DSTAR         GPIO_Pin_10
// #define PORT_DSTAR        GPIOB
// #define RCC_Per_DSTAR     RCC_AHB1Periph_GPIOB

// #define PIN_DMR           GPIO_Pin_4
// #define PORT_DMR          GPIOB
// #define RCC_Per_DMR       RCC_AHB1Periph_GPIOB

// #define PIN_YSF           GPIO_Pin_5
// #define PORT_YSF          GPIOB
// #define RCC_Per_YSF       RCC_AHB1Periph_GPIOB

#if defined(MODE_PINS)
#define PIN_MP25          GPIO_Pin_15
#define PORT_MP25         GPIOB
#define RCC_Per_MP25      RCC_AHB1Periph_GPIOB

#define PIN_MDSTAR        GPIO_Pin_9
#define PORT_MDSTAR       GPIOB
#define RCC_Per_MDSTAR    RCC_AHB1Periph_GPIOB

#define PIN_MDMR          GPIO_Pin_8
#define PORT_MDMR         GPIOB
#define RCC_Per_MDMR      RCC_AHB1Periph_GPIOB

#define PIN_MYSF          GPIO_Pin_14
#define PORT_MYSF         GPIOB
#define RCC_Per_MYSF      RCC_AHB1Periph_GPIOB
#endif

#define PIN_EXT_CLK       GPIO_Pin_15
#define SRC_EXT_CLK       GPIO_PinSource15
#define PORT_EXT_CLK      GPIOA

#define PIN_RX            GPIO_Pin_0
#define PIN_RX_CH         ADC_Channel_0
#define PORT_RX           GPIOA
#define RCC_Per_RX        RCC_AHB1Periph_GPIOA

#define PIN_RSSI          GPIO_Pin_1
#define PIN_RSSI_CH       ADC_Channel_1
#define PORT_RSSI         GPIOA
#define RCC_Per_RSSI      RCC_AHB1Periph_GPIOA

#define PIN_TX            GPIO_Pin_4
#define PIN_TX_CH         DAC_Channel_1
#define PORT_TX           GPIOA
#define RCC_Per_TX        RCC_AHB1Periph_GPIOA

#endif