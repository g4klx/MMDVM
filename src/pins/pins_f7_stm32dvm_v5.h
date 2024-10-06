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

#ifndef _PINS_F7_STM32DVM_V5_H
#define _PINS_F7_STM32DVM_V5_H

/*
Pin definitions for STM32F7 STM32-DVM rev 5 Board:

COS      PB13   input
PTT      PB12   output
COSLED   PB4    output
LED      PB3    output

P25      PB8    output
NXDN     PB9    output
DSTAR    PB6    output
DMR      PB5    output
YSF      PB7    output
POCSAG   PC10   output (Not Valid)
FM       PB14   output

RX       PB0    analog input
RSSI     PB1    analog input
TX       PA4    analog output

EXT_CLK  PA15   input
*/

#define PIN_COS           GPIO_Pin_13
#define PORT_COS          GPIOB
#define RCC_Per_COS       RCC_AHB1Periph_GPIOB

#define PIN_PTT           GPIO_Pin_12
#define PORT_PTT          GPIOB
#define RCC_Per_PTT       RCC_AHB1Periph_GPIOB

#define PIN_COSLED        GPIO_Pin_4
#define PORT_COSLED       GPIOB
#define RCC_Per_COSLED    RCC_AHB1Periph_GPIOB

#define PIN_LED           GPIO_Pin_3
#define PORT_LED          GPIOB
#define RCC_Per_LED       RCC_AHB1Periph_GPIOB

#define PIN_P25           GPIO_Pin_8
#define PORT_P25          GPIOB
#define RCC_Per_P25       RCC_AHB1Periph_GPIOB

#define PIN_NXDN          GPIO_Pin_9
#define PORT_NXDN         GPIOB
#define RCC_Per_NXDN      RCC_AHB1Periph_GPIOB

#define PIN_DSTAR         GPIO_Pin_6
#define PORT_DSTAR        GPIOB
#define RCC_Per_DSTAR     RCC_AHB1Periph_GPIOB

#define PIN_DMR           GPIO_Pin_5
#define PORT_DMR          GPIOB
#define RCC_Per_DMR       RCC_AHB1Periph_GPIOB

#define PIN_YSF           GPIO_Pin_7
#define PORT_YSF          GPIOB
#define RCC_Per_YSF       RCC_AHB1Periph_GPIOB

/*
#define PIN_POCSAG        GPIO_Pin_10
#define PORT_POCSAG       GPIOC
#define RCC_Per_POCSAG    RCC_AHB1Periph_GPIOC
*/

#define PIN_FM            GPIO_Pin_14
#define PORT_FM           GPIOB
#define RCC_Per_FM        RCC_AHB1Periph_GPIOB

#define PIN_EXT_CLK       GPIO_Pin_15
#define SRC_EXT_CLK       GPIO_PinSource15
#define PORT_EXT_CLK      GPIOA

#define PIN_RX            GPIO_Pin_0
#define PIN_RX_CH         ADC_Channel_8
#define PORT_RX           GPIOB
#define RCC_Per_RX        RCC_AHB1Periph_GPIOB

#define PIN_RSSI          GPIO_Pin_1
#define PIN_RSSI_CH       ADC_Channel_9
#define PORT_RSSI         GPIOB
#define RCC_Per_RSSI      RCC_AHB1Periph_GPIOB

#define PIN_TX            GPIO_Pin_4
#define PIN_TX_CH         DAC_Channel_1

#endif
