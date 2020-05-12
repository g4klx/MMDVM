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

#ifndef _PINS_F4_NUCLEO_MORPHO_H
#define _PINS_F4_NUCLEO_MORPHO_H

/*
Pin definitions for STM32F4 Nucleo boards (ST Morpho header):

PTT      PB13   output           CN10 Pin30
COSLED   PB14   output           CN10 Pin28
LED      PA5    output           CN10 Pin11
COS      PB15   input            CN10 Pin26

DSTAR    PB10   output           CN10 Pin25
DMR      PB4    output           CN10 Pin27
YSF      PB5    output           CN10 Pin29
P25      PB3    output           CN10 Pin31
NXDN     PA10   output           CN10 Pin33
POCSAG   PB12   output           CN10 Pin16

MDSTAR   PC4    output           CN10 Pin34
MDMR     PC5    output           CN10 Pin6
MYSF     PC2    output           CN7 Pin35
MP25     PC3    output           CN7 Pin37
MNXDN    PC6    output           CN10 Pin4
MPOCSAG  PC8    output           CN10 Pin2

RX       PA0    analog input     CN7 Pin28
RSSI     PA1    analog input     CN7 Pin30
TX       PA4    analog output    CN7 Pin32

EXT_CLK  PA15   input            CN7 Pin17
*/

#define PIN_COS           GPIO_Pin_15
#define PORT_COS          GPIOB
#define RCC_Per_COS       RCC_AHB1Periph_GPIOB

#define PIN_PTT           GPIO_Pin_13
#define PORT_PTT          GPIOB
#define RCC_Per_PTT       RCC_AHB1Periph_GPIOB

#define PIN_COSLED        GPIO_Pin_14
#define PORT_COSLED       GPIOB
#define RCC_Per_COSLED    RCC_AHB1Periph_GPIOB

#define PIN_LED           GPIO_Pin_5
#define PORT_LED          GPIOA
#define RCC_Per_LED       RCC_AHB1Periph_GPIOA

#define PIN_P25           GPIO_Pin_3
#define PORT_P25          GPIOB
#define RCC_Per_P25       RCC_AHB1Periph_GPIOB

#define PIN_NXDN          GPIO_Pin_10
#define PORT_NXDN         GPIOA
#define RCC_Per_NXDN      RCC_AHB1Periph_GPIOA

#define PIN_POCSAG        GPIO_Pin_12
#define PORT_POCSAG       GPIOB
#define RCC_Per_POCSAG    RCC_AHB1Periph_GPIOB

#define PIN_DSTAR         GPIO_Pin_10
#define PORT_DSTAR        GPIOB
#define RCC_Per_DSTAR     RCC_AHB1Periph_GPIOB

#define PIN_DMR           GPIO_Pin_4
#define PORT_DMR          GPIOB
#define RCC_Per_DMR       RCC_AHB1Periph_GPIOB

#define PIN_YSF           GPIO_Pin_5
#define PORT_YSF          GPIOB
#define RCC_Per_YSF       RCC_AHB1Periph_GPIOB

#if defined(MODE_PINS)
#define PIN_MP25          GPIO_Pin_3
#define PORT_MP25         GPIOC
#define RCC_Per_MP25      RCC_AHB1Periph_GPIOC

#define PIN_MNXDN         GPIO_Pin_6
#define PORT_MNXDN        GPIOC
#define RCC_Per_MNXDN     RCC_AHB1Periph_GPIOC

#define PIN_MDSTAR        GPIO_Pin_4
#define PORT_MDSTAR       GPIOC
#define RCC_Per_MDSTAR    RCC_AHB1Periph_GPIOC

#define PIN_MDMR          GPIO_Pin_5
#define PORT_MDMR         GPIOC
#define RCC_Per_MDMR      RCC_AHB1Periph_GPIOC

#define PIN_MYSF          GPIO_Pin_2
#define PORT_MYSF         GPIOC
#define RCC_Per_MYSF      RCC_AHB1Periph_GPIOC

#define PIN_MPOCSAG       GPIO_Pin_8
#define PORT_MPOCSAG      GPIOC
#define RCC_Per_MPOCSAG   RCC_AHB1Periph_GPIOC
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

#endif