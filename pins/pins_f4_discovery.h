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

#ifndef _PINS_F4_DISCOVERY_H
#define _PINS_F4_DISCOVERY_H

/*
Pin definitions for STM32F4 Discovery Board:

PTT      PB13   output           P1 Pin37
COSLED   PA7    output           P1 Pin17
LED      PD15   output           P1 Pin47
COS      PA5    input            P1 Pin15

DSTAR    PD12   output           P1 Pin44
DMR      PD13   output           P1 Pin45
YSF      PD14   output           P1 Pin46
P25      PD11   output           P1 Pin43
NXDN     PD10   output           P1 Pin42

RX       PA0    analog input     P1 Pin12
RSSI     PA1    analog input     P1 Pin11
TX       PA4    analog output    P1 Pin16

EXT_CLK  PA15   input            P2 Pin40
*/

#define PIN_COS           GPIO_Pin_5
#define PORT_COS          GPIOA
#define RCC_Per_COS       RCC_AHB1Periph_GPIOA

#define PIN_PTT           GPIO_Pin_13
#define PORT_PTT          GPIOB
#define RCC_Per_PTT       RCC_AHB1Periph_GPIOB

#define PIN_COSLED        GPIO_Pin_7
#define PORT_COSLED       GPIOA
#define RCC_Per_COSLED    RCC_AHB1Periph_GPIOA

#define PIN_LED           GPIO_Pin_15
#define PORT_LED          GPIOD
#define RCC_Per_LED       RCC_AHB1Periph_GPIOD

#define PIN_P25           GPIO_Pin_11
#define PORT_P25          GPIOD
#define RCC_Per_P25       RCC_AHB1Periph_GPIOD

#define PIN_NXDN          GPIO_Pin_10
#define PORT_NXDN         GPIOD
#define RCC_Per_NXDN      RCC_AHB1Periph_GPIOD

#define PIN_DSTAR         GPIO_Pin_12
#define PORT_DSTAR        GPIOD
#define RCC_Per_DSTAR     RCC_AHB1Periph_GPIOD

#define PIN_DMR           GPIO_Pin_13
#define PORT_DMR          GPIOD
#define RCC_Per_DMR       RCC_AHB1Periph_GPIOD

#define PIN_YSF           GPIO_Pin_14
#define PORT_YSF          GPIOD
#define RCC_Per_YSF       RCC_AHB1Periph_GPIOD

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

#endif