/*
 *   Copyright (C) 2016 by Jim McLaughlin KI6ZUM
 *   Copyright (C) 2016, 2017 by Andy Uribe CA6JAU
 *   Copyright (C) 2017 by Jonathan Naylor G4KLX
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
#include "IO.h"

#if defined(STM32F4XX) || defined(STM32F4)

#if defined(STM32F4_DISCOVERY)
/*
Pin definitions for STM32F4 Discovery Board:

PTT      PB13   output
COSLED   PA7    output
LED      PD15   output
COS      PA5    input

DSTAR    PD12   output
DMR      PD13   output
YSF      PD14   output
P25      PD11   output

RX       PA0    analog input
RSSI     PA1    analog input
TX       PA4    analog output

EXT_CLK  PA15   input
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

#elif defined(STM32F4_PI)
/*
Pin definitions for STM32F4 Pi Board:

PTT      PB13   output
COSLED   PB14   output
LED      PB15   output
COS      PC0    input

DSTAR    PC7    output
DMR      PC8    output
YSF      PA8    output
P25      PC9    output

RX       PA0    analog input
RSSI     PA7    analog input
TX       PA4    analog output

EXT_CLK  PA15   input
*/

#define PIN_COS           GPIO_Pin_0
#define PORT_COS          GPIOC
#define RCC_Per_COS       RCC_AHB1Periph_GPIOC

#define PIN_PTT           GPIO_Pin_13
#define PORT_PTT          GPIOB
#define RCC_Per_PTT       RCC_AHB1Periph_GPIOB

#define PIN_COSLED        GPIO_Pin_14
#define PORT_COSLED       GPIOB
#define RCC_Per_COSLED    RCC_AHB1Periph_GPIOB

#define PIN_LED           GPIO_Pin_15
#define PORT_LED          GPIOB
#define RCC_Per_LED       RCC_AHB1Periph_GPIOB

#define PIN_P25           GPIO_Pin_9
#define PORT_P25          GPIOC
#define RCC_Per_P25       RCC_AHB1Periph_GPIOC

#define PIN_DSTAR         GPIO_Pin_7
#define PORT_DSTAR        GPIOC
#define RCC_Per_DSTAR     RCC_AHB1Periph_GPIOC

#define PIN_DMR           GPIO_Pin_8
#define PORT_DMR          GPIOC
#define RCC_Per_DMR       RCC_AHB1Periph_GPIOC

#define PIN_YSF           GPIO_Pin_8
#define PORT_YSF          GPIOA
#define RCC_Per_YSF       RCC_AHB1Periph_GPIOA

#define PIN_EXT_CLK       GPIO_Pin_15
#define SRC_EXT_CLK       GPIO_PinSource15
#define PORT_EXT_CLK      GPIOA

#define PIN_RX            GPIO_Pin_0
#define PIN_RX_CH         ADC_Channel_0
#define PORT_RX           GPIOA
#define RCC_Per_RX        RCC_AHB1Periph_GPIOA

#define PIN_RSSI          GPIO_Pin_7
#define PIN_RSSI_CH       ADC_Channel_7
#define PORT_RSSI         GPIOA
#define RCC_Per_RSSI      RCC_AHB1Periph_GPIOA

#define PIN_TX            GPIO_Pin_4
#define PIN_TX_CH         DAC_Channel_1

#elif defined(STM32F4_NUCLEO)

#if defined(STM32F4_NUCLEO_MORPHO_HEADER)
/*
Pin definitions for STM32F4 Nucleo boards (ST Morpho header):

PTT      PB13   output
COSLED   PB14   output
LED      PA5    output
COS      PB15   input

DSTAR    PB10   output
DMR      PB4    output
YSF      PB5    output
P25      PB3    output

MDSTAR   PC14   output
MDMR     PC15   outout
MYSF     PC2    output
MP25     PC3    output

RX       PA0    analog input
RSSI     PA1    analog input
TX       PA4    analog output

EXT_CLK  PA15   input
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

#define PIN_DSTAR         GPIO_Pin_10
#define PORT_DSTAR        GPIOB
#define RCC_Per_DSTAR     RCC_AHB1Periph_GPIOB

#define PIN_DMR           GPIO_Pin_4
#define PORT_DMR          GPIOB
#define RCC_Per_DMR       RCC_AHB1Periph_GPIOB

#define PIN_YSF           GPIO_Pin_5
#define PORT_YSF          GPIOB
#define RCC_Per_YSF       RCC_AHB1Periph_GPIOB

#if defined(STM32F4_NUCLEO_MODE_PINS)
#define PIN_MP25          GPIO_Pin_3
#define PORT_MP25         GPIOC
#define RCC_Per_MP25      RCC_AHB1Periph_GPIOC

#define PIN_MDSTAR        GPIO_Pin_14
#define PORT_MDSTAR       GPIOC
#define RCC_Per_MDSTAR    RCC_AHB1Periph_GPIOC

#define PIN_MDMR          GPIO_Pin_15
#define PORT_MDMR         GPIOC
#define RCC_Per_MDMR      RCC_AHB1Periph_GPIOC

#define PIN_MYSF          GPIO_Pin_2
#define PORT_MYSF         GPIOC
#define RCC_Per_MYSF      RCC_AHB1Periph_GPIOC
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

#elif defined(STM32F4_NUCLEO_ARDUINO_HEADER)
/*
Pin definitions for STM32F4 Nucleo boards (Arduino header):

PTT      PB10   output
COSLED   PB3    output
LED      PB5    output
COS      PB4    input

DSTAR    PA1    output
DMR      PA4    output
YSF      PB0    output
P25      PC1    output

RX       PA0    analog input
RSSI     PC0    analog input
TX       PA5    analog output

EXT_CLK  PB8    input
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

#else
#error "Either STM32F4_NUCLEO_MORPHO_HEADER or STM32F4_NUCLEO_ARDUINO_HEADER need to be defined in Config.h"
#endif

#else
#error "Either STM32F4_DISCOVERY, STM32F4_PI or STM32F4_NUCLEO need to be defined"
#endif

const uint16_t DC_OFFSET = 2048U;

// Sampling frequency
#define SAMP_FREQ   24000

extern "C" {
   void TIM2_IRQHandler() {
      if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
         io.interrupt();
      }
   }
}

void CIO::initInt()
{
   GPIO_InitTypeDef GPIO_InitStruct;
   GPIO_StructInit(&GPIO_InitStruct);
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;

   // PTT pin
   RCC_AHB1PeriphClockCmd(RCC_Per_PTT, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_PTT;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_PTT, &GPIO_InitStruct);

   // COSLED pin
   RCC_AHB1PeriphClockCmd(RCC_Per_COSLED, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_COSLED;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_COSLED, &GPIO_InitStruct);

   // LED pin
   RCC_AHB1PeriphClockCmd(RCC_Per_LED, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_LED;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_LED, &GPIO_InitStruct);

   // Init the input pins PIN_COS
   RCC_AHB1PeriphClockCmd(RCC_Per_COS, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_COS;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
   GPIO_Init(PORT_COS, &GPIO_InitStruct);

#if defined(ARDUINO_MODE_PINS)
   // DSTAR pin
   RCC_AHB1PeriphClockCmd(RCC_Per_DSTAR, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_DSTAR;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_DSTAR, &GPIO_InitStruct);

   // DMR pin
   RCC_AHB1PeriphClockCmd(RCC_Per_DMR, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_DMR;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_DMR, &GPIO_InitStruct);

   // YSF pin
   RCC_AHB1PeriphClockCmd(RCC_Per_YSF, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_YSF;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_YSF, &GPIO_InitStruct);

   // P25 pin
   RCC_AHB1PeriphClockCmd(RCC_Per_P25, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_P25;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_P25, &GPIO_InitStruct);
#endif

#if defined(STM32F4_NUCLEO_MODE_PINS)
   // DSTAR mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MDSTAR, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MDSTAR;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MDSTAR, &GPIO_InitStruct);

   // DMR mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MDMR, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MDMR;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MDMR, &GPIO_InitStruct);

   // YSF mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MYSF, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MYSF;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MYSF, &GPIO_InitStruct);

   // P25 mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MP25, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MP25;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MP25, &GPIO_InitStruct);
#endif
}

void CIO::startInt()
{
   if ((ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET))
      io.interrupt();

   // Init the ADC
   GPIO_InitTypeDef        GPIO_InitStruct;
   ADC_InitTypeDef         ADC_InitStructure;
   ADC_CommonInitTypeDef   ADC_CommonInitStructure;

   GPIO_StructInit(&GPIO_InitStruct);
   ADC_CommonStructInit(&ADC_CommonInitStructure);
   ADC_StructInit(&ADC_InitStructure);

   // Enable ADC1 clock
   RCC_AHB1PeriphClockCmd(RCC_Per_RX, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
   // Enable ADC1 GPIO
   GPIO_InitStruct.GPIO_Pin  = PIN_RX;
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
   GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
   GPIO_Init(PORT_RX, &GPIO_InitStruct);

#if defined(SEND_RSSI_DATA)
   // Enable ADC2 clock
   RCC_AHB1PeriphClockCmd(RCC_Per_RSSI, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
   // Enable ADC2 GPIO
   GPIO_InitStruct.GPIO_Pin  = PIN_RSSI;
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
   GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
   GPIO_Init(PORT_RSSI, &GPIO_InitStruct);
#endif

   // Init ADCs in dual mode (RSSI), div clock by two
#if defined(SEND_RSSI_DATA)
   ADC_CommonInitStructure.ADC_Mode             = ADC_DualMode_RegSimult;
#else
   ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
#endif
   ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div2;
   ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
   ADC_CommonInit(&ADC_CommonInitStructure);

   // Init ADC1 and ADC2: 12bit, single-conversion
   ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
   ADC_InitStructure.ADC_ScanConvMode         = DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
   ADC_InitStructure.ADC_ExternalTrigConv     = 0;
   ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion      = 1;

   ADC_Init(ADC1, &ADC_InitStructure);

   ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
   ADC_RegularChannelConfig(ADC1, PIN_RX_CH, 1, ADC_SampleTime_3Cycles);

   // Enable ADC1
   ADC_Cmd(ADC1, ENABLE);

#if defined(SEND_RSSI_DATA)
   ADC_Init(ADC2, &ADC_InitStructure);

   ADC_EOCOnEachRegularChannelCmd(ADC2, ENABLE);
   ADC_RegularChannelConfig(ADC2, PIN_RSSI_CH, 1, ADC_SampleTime_3Cycles);

   // Enable ADC2
   ADC_Cmd(ADC2, ENABLE);
#endif

   // Init the DAC
   DAC_InitTypeDef DAC_InitStructure;

   GPIO_StructInit(&GPIO_InitStruct);
   DAC_StructInit(&DAC_InitStructure);

   // GPIOA clock enable
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

   // DAC Periph clock enable
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

   // GPIO CONFIGURATION of DAC Pin
   GPIO_InitStruct.GPIO_Pin   = PIN_TX;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AN;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStruct);

   DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
   DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
   DAC_Init(PIN_TX_CH, &DAC_InitStructure);
   DAC_Cmd(PIN_TX_CH, ENABLE);

   // Init the timer
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

#if defined(EXTERNAL_OSC)
   // Configure a GPIO as external TIM2 clock source
   GPIO_PinAFConfig(PORT_EXT_CLK, SRC_EXT_CLK, GPIO_AF_TIM2);
   GPIO_InitStruct.GPIO_Pin = PIN_EXT_CLK;
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
   GPIO_Init(PORT_EXT_CLK, &GPIO_InitStruct);
#endif

   TIM_TimeBaseInitTypeDef timerInitStructure;
   TIM_TimeBaseStructInit (&timerInitStructure);

   // TIM2 output frequency
#if defined(EXTERNAL_OSC)
   timerInitStructure.TIM_Prescaler = (uint16_t) ((EXTERNAL_OSC/(2*SAMP_FREQ)) - 1);
#else
   timerInitStructure.TIM_Prescaler = (uint16_t) ((SystemCoreClock/(4*SAMP_FREQ)) - 1);
#endif

   timerInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
   timerInitStructure.TIM_Period            = 1;
   timerInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
   timerInitStructure.TIM_RepetitionCounter = 0;
   TIM_TimeBaseInit(TIM2, &timerInitStructure);

#if defined(EXTERNAL_OSC)
   // Enable external clock
   TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x00);
#else
   // Enable internal clock
   TIM_InternalClockConfig(TIM2);
#endif

   // Enable TIM2
   TIM_Cmd(TIM2, ENABLE);
   // Enable TIM2 interrupt
   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

   NVIC_InitTypeDef nvicStructure;
   nvicStructure.NVIC_IRQChannel                   = TIM2_IRQn;
   nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
   nvicStructure.NVIC_IRQChannelSubPriority        = 1;
   nvicStructure.NVIC_IRQChannelCmd                = ENABLE;
   NVIC_Init(&nvicStructure);

   GPIO_ResetBits(PORT_COSLED, PIN_COSLED);
   GPIO_SetBits(PORT_LED, PIN_LED);
}

void CIO::interrupt()
{
   uint8_t control  = MARK_NONE;
   uint16_t sample  = DC_OFFSET;
   uint16_t rawRSSI = 0U;

   m_txBuffer.get(sample, control);

   // Send the value to the DAC
#if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
   DAC_SetChannel2Data(DAC_Align_12b_R, sample);
#else
   DAC_SetChannel1Data(DAC_Align_12b_R, sample);
#endif

   // Read value from ADC1 and ADC2
   if ((ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)) {
      // shouldn't be still in reset at this point so null the sample value?
      sample  = 0U;
   } else {
      sample  = ADC_GetConversionValue(ADC1);
#if defined(SEND_RSSI_DATA)
      rawRSSI = ADC_GetConversionValue(ADC2);
#endif
   }

   // trigger next ADC1
   ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
   ADC_SoftwareStartConv(ADC1);

   m_rxBuffer.put(sample, control);
   m_rssiBuffer.put(rawRSSI);

   m_watchdog++;
}

bool CIO::getCOSInt()
{
   return GPIO_ReadOutputDataBit(PORT_COS, PIN_COS) == Bit_SET;
}

void CIO::setLEDInt(bool on)
{
   GPIO_WriteBit(PORT_LED, PIN_LED, on ? Bit_SET : Bit_RESET);
}

void CIO::setPTTInt(bool on)
{
   GPIO_WriteBit(PORT_PTT, PIN_PTT, on ? Bit_SET : Bit_RESET);
}

void CIO::setCOSInt(bool on)
{
   GPIO_WriteBit(PORT_COSLED, PIN_COSLED, on ? Bit_SET : Bit_RESET);
}

void CIO::setDStarInt(bool on)
{
   GPIO_WriteBit(PORT_DSTAR, PIN_DSTAR, on ? Bit_SET : Bit_RESET);
#if defined(STM32F4_NUCLEO_MODE_PINS)
   GPIO_WriteBit(PORT_MDSTAR, PIN_MDSTAR, on ? Bit_SET : Bit_RESET);
#endif
}

void CIO::setDMRInt(bool on)
{
   GPIO_WriteBit(PORT_DMR, PIN_DMR, on ? Bit_SET : Bit_RESET);
#if defined(STM32F4_NUCLEO_MODE_PINS)
   GPIO_WriteBit(PORT_MDMR, PIN_MDMR, on ? Bit_SET : Bit_RESET);
#endif
}

void CIO::setYSFInt(bool on)
{
   GPIO_WriteBit(PORT_YSF, PIN_YSF, on ? Bit_SET : Bit_RESET);
#if defined(STM32F4_NUCLEO_MODE_PINS)
   GPIO_WriteBit(PORT_MYSF, PIN_MYSF, on ? Bit_SET : Bit_RESET);
#endif
}

void CIO::setP25Int(bool on)
{
   GPIO_WriteBit(PORT_P25, PIN_P25, on ? Bit_SET : Bit_RESET);
#if defined(STM32F4_NUCLEO_MODE_PINS)
   GPIO_WriteBit(PORT_MP25, PIN_MP25, on ? Bit_SET : Bit_RESET);
#endif
}

#endif
