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

#ifndef _PINS_F4_NUCLEO_ARDUINO_H
#define _PINS_F4_NUCLEO_ARDUINO_H

/*
Pin definitions for STM32F4 Nucleo boards (Arduino header):

PTT      PB10   output           CN9 Pin7
COSLED   PB3    output           CN9 Pin4
LED      PB5    output           CN9 Pin5
COS      PB4    input            CN9 Pin6

DSTAR    PA1    output           CN8 Pin2
DMR      PA4    output           CN8 Pin3
YSF      PB0    output           CN8 Pin4
P25      PC1    output           CN8 Pin5
NXDN     PA3    output           CN9 Pin1
POCSAG   PB12   output

RX       PA0    analog input     CN8 Pin1
RSSI     PC0    analog input     CN8 Pin6
TX       PA5    analog output    CN5 Pin6

EXT_CLK  PB8    input            CN5 Pin10
*/

#define PIN_COS           GPIO_Pin_4
#define PORT_COS          GPIOB
#define RCC_Per_COS       RCC_AHB1Periph_GPIOB

#define PIN_PTT           GPIO_Pin_10
#define PORT_PTT          GPIOB
#define RCC_Per_PTT       RCC_AHB1Periph_GPIOB

#define PIN_COSLED        GPIO_Pin_3
#define PORT_COSLED       GPIOB
#define RCC_Per_COSLED    RCC_AHB1Periph_GPIOB

#define PIN_LED           GPIO_Pin_5
#define PORT_LED          GPIOB
#define RCC_Per_LED       RCC_AHB1Periph_GPIOB

#define PIN_P25           GPIO_Pin_1
#define PORT_P25          GPIOC
#define RCC_Per_P25       RCC_AHB1Periph_GPIOC

#define PIN_NXDN          GPIO_Pin_3
#define PORT_NXDN         GPIOA
#define RCC_Per_NXDN      RCC_AHB1Periph_GPIOA

#define PIN_POCSAG        GPIO_Pin_12
#define PORT_POCSAG       GPIOB
#define RCC_Per_POCSAG    RCC_AHB1Periph_GPIOB

#define PIN_DSTAR         GPIO_Pin_1
#define PORT_DSTAR        GPIOA
#define RCC_Per_DSTAR     RCC_AHB1Periph_GPIOA

#define PIN_DMR           GPIO_Pin_4
#define PORT_DMR          GPIOA
#define RCC_Per_DMR       RCC_AHB1Periph_GPIOA

#define PIN_YSF           GPIO_Pin_0
#define PORT_YSF          GPIOB
#define RCC_Per_YSF       RCC_AHB1Periph_GPIOB

#define PIN_EXT_CLK       GPIO_Pin_8
#define SRC_EXT_CLK       GPIO_PinSource8
#define PORT_EXT_CLK      GPIOB

#define PIN_RX            GPIO_Pin_0
#define PIN_RX_CH         ADC_Channel_0
#define PORT_RX           GPIOA
#define RCC_Per_RX        RCC_AHB1Periph_GPIOA

#define PIN_RSSI          GPIO_Pin_0
#define PIN_RSSI_CH       ADC_Channel_10
#define PORT_RSSI         GPIOC
#define RCC_Per_RSSI      RCC_AHB1Periph_GPIOC

#define PIN_TX            GPIO_Pin_5
#define PIN_TX_CH         DAC_Channel_2

#endif