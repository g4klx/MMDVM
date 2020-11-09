/*
 *   Copyright (C) 2016 by Jim McLaughlin KI6ZUM
 *   Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
 *   Copyright (C) 2017,2018,2020 by Jonathan Naylor G4KLX
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

#include "Config.h"
#include "Globals.h"
#include "IO.h"

#if defined(STM32F4XX) || defined(STM32F7XX)
#include "IOPins.h"


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
   GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
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

#if defined(MODE_LEDS)
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

#if !defined(USE_ALTERNATE_NXDN_LEDS)
   // NXDN pin
   RCC_AHB1PeriphClockCmd(RCC_Per_NXDN, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_NXDN;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_NXDN, &GPIO_InitStruct);
#endif

#if !defined(USE_ALTERNATE_M17_LEDS)
   // M17 pin
   RCC_AHB1PeriphClockCmd(RCC_Per_M17, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_M17;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_M17, &GPIO_InitStruct);
#endif

#if !defined(USE_ALTERNATE_POCSAG_LEDS)
   // POCSAG pin
   RCC_AHB1PeriphClockCmd(RCC_Per_POCSAG, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_POCSAG;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_POCSAG, &GPIO_InitStruct);
#endif

#if !defined(USE_ALTERNATE_FM_LEDS)
   // FM pin
   RCC_AHB1PeriphClockCmd(RCC_Per_FM, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_FM;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_FM, &GPIO_InitStruct);
#endif

#endif

#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
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

#if !defined(USE_ALTERNATE_NXDN_LEDS)
   // NXDN mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MNXDN, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MNXDN;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MNXDN, &GPIO_InitStruct);
#endif

#if !defined(USE_ALTERNATE_M17_LEDS)
   // M17 mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MM17, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MM17;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MM17, &GPIO_InitStruct);
#endif

#if !defined(USE_ALTERNATE_POCSAG_LEDS)
   // POCSAG mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MPOCSAG, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MPOCSAG;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MPOCSAG, &GPIO_InitStruct);
#endif

#if !defined(USE_ALTERNATE_FM_LEDS)
   // FM mode pin
   RCC_AHB1PeriphClockCmd(RCC_Per_MFM, ENABLE);
   GPIO_InitStruct.GPIO_Pin   = PIN_MFM;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_Init(PORT_MFM, &GPIO_InitStruct);
#endif

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

#if defined(EXTERNAL_OSC) && !(defined(STM32F4_PI) || defined(STM32F722_PI))
   // Configure a GPIO as external TIM2 clock source
   GPIO_PinAFConfig(PORT_EXT_CLK, SRC_EXT_CLK, GPIO_AF_TIM2);
   GPIO_InitStruct.GPIO_Pin = PIN_EXT_CLK;
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
   GPIO_Init(PORT_EXT_CLK, &GPIO_InitStruct);
#endif

   TIM_TimeBaseInitTypeDef timerInitStructure;
   TIM_TimeBaseStructInit (&timerInitStructure);

   // TIM2 output frequency
#if defined(EXTERNAL_OSC) && !(defined(STM32F4_PI) || defined(STM32F722_PI))
   timerInitStructure.TIM_Prescaler = (uint16_t) ((EXTERNAL_OSC/(2*SAMP_FREQ)) - 1);
   timerInitStructure.TIM_Period = 1;
#else
   timerInitStructure.TIM_Prescaler = (uint16_t) ((SystemCoreClock/(6*SAMP_FREQ)) - 1);
   timerInitStructure.TIM_Period = 2;
#endif

   timerInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
   timerInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
   timerInitStructure.TIM_RepetitionCounter = 0;
   TIM_TimeBaseInit(TIM2, &timerInitStructure);

#if defined(EXTERNAL_OSC) && !(defined(STM32F4_PI) || defined(STM32F722_PI))
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
   TSample sample = {DC_OFFSET, MARK_NONE};
   uint16_t rawRSSI = 0U;

   m_txBuffer.get(sample);

   // Send the value to the DAC
#if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
   DAC_SetChannel2Data(DAC_Align_12b_R, sample.sample);
#else
   DAC_SetChannel1Data(DAC_Align_12b_R, sample.sample);
#endif

   // Read value from ADC1 and ADC2
   if ((ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)) {
      // shouldn't be still in reset at this point so null the sample value?
      sample.sample  = 0U;
   } else {
      sample.sample  = ADC_GetConversionValue(ADC1);
#if defined(SEND_RSSI_DATA)
      rawRSSI = ADC_GetConversionValue(ADC2);
#endif
   }

   // trigger next ADC1
   ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
   ADC_SoftwareStartConv(ADC1);

   m_rxBuffer.put(sample);
   m_rssiBuffer.put(rawRSSI);

   m_watchdog++;
}

bool CIO::getCOSInt()
{
   return GPIO_ReadInputDataBit(PORT_COS, PIN_COS) == Bit_SET;
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
#if defined(MODE_LEDS)
   GPIO_WriteBit(PORT_DSTAR, PIN_DSTAR, on ? Bit_SET : Bit_RESET);
#endif
#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
   GPIO_WriteBit(PORT_MDSTAR, PIN_MDSTAR, on ? Bit_SET : Bit_RESET);
#endif
}

void CIO::setDMRInt(bool on)
{
#if defined(MODE_LEDS)
   GPIO_WriteBit(PORT_DMR, PIN_DMR, on ? Bit_SET : Bit_RESET);
#endif
#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
   GPIO_WriteBit(PORT_MDMR, PIN_MDMR, on ? Bit_SET : Bit_RESET);
#endif
}

void CIO::setYSFInt(bool on)
{
#if defined(MODE_LEDS)
   GPIO_WriteBit(PORT_YSF, PIN_YSF, on ? Bit_SET : Bit_RESET);
#endif
#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
   GPIO_WriteBit(PORT_MYSF, PIN_MYSF, on ? Bit_SET : Bit_RESET);
#endif
}

void CIO::setP25Int(bool on)
{
#if defined(MODE_LEDS)
   GPIO_WriteBit(PORT_P25, PIN_P25, on ? Bit_SET : Bit_RESET);
#endif
#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
   GPIO_WriteBit(PORT_MP25, PIN_MP25, on ? Bit_SET : Bit_RESET);
#endif
}

void CIO::setNXDNInt(bool on)
{
#if defined(MODE_LEDS)
#if defined(USE_ALTERNATE_NXDN_LEDS)
   GPIO_WriteBit(PORT_YSF, PIN_YSF, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_P25, PIN_P25, on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_NXDN, PIN_NXDN, on ? Bit_SET : Bit_RESET);
#endif
#endif

#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
#if defined(USE_ALTERNATE_NXDN_LEDS)
   GPIO_WriteBit(PORT_MYSF, PIN_MYSF, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_MP25, PIN_MP25, on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_MNXDN, PIN_MNXDN, on ? Bit_SET : Bit_RESET);
#endif
#endif
}

void CIO::setM17Int(bool on)
{
#if defined(MODE_LEDS)
#if defined(USE_ALTERNATE_M17_LEDS)
   GPIO_WriteBit(PORT_DSTAR, PIN_DSTAR, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_P25,   PIN_P25, on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_M17, PIN_M17, on ? Bit_SET : Bit_RESET);
#endif
#endif

#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
#if defined(USE_ALTERNATE_M17_LEDS)
   GPIO_WriteBit(PORT_MDSTAR, PIN_MDSTAR, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_MP25,   PIN_MP25,   on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_MM17, PIN_MM17, on ? Bit_SET : Bit_RESET);
#endif
#endif
}

void CIO::setPOCSAGInt(bool on)
{
#if defined(MODE_LEDS)
#if defined(USE_ALTERNATE_POCSAG_LEDS)
   GPIO_WriteBit(PORT_DSTAR, PIN_DSTAR, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_DMR,   PIN_DMR,   on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_POCSAG,  PIN_POCSAG, on ? Bit_SET : Bit_RESET);
#endif
#endif

#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
#if defined(USE_ALTERNATE_POCSAG_LEDS)
   GPIO_WriteBit(PORT_MDSTAR, PIN_MDSTAR, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_MDMR,   PIN_MDMR,   on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_MPOCSAG, PIN_MPOCSAG, on ? Bit_SET : Bit_RESET);
#endif
#endif
}

void CIO::setFMInt(bool on)
{
#if defined(MODE_LEDS)
#if defined(USE_ALTERNATE_FM_LEDS)
   GPIO_WriteBit(PORT_DSTAR, PIN_DSTAR, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_YSF,   PIN_YSF,   on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_FM,  PIN_FM, on ? Bit_SET : Bit_RESET);
#endif
#endif

#if defined(MODE_PINS) && defined(STM32F4_NUCLEO_MORPHO_HEADER) && (defined(STM32F4_NUCLEO) || defined(STM32F722_RPT_HAT))
#if defined(USE_ALTERNATE_FM_LEDS)
   GPIO_WriteBit(PORT_MDSTAR, PIN_MDSTAR, on ? Bit_SET : Bit_RESET);
   GPIO_WriteBit(PORT_MYSF,   PIN_MYSF,   on ? Bit_SET : Bit_RESET);
#else
   GPIO_WriteBit(PORT_MFM, PIN_MFM, on ? Bit_SET : Bit_RESET);
#endif
#endif
}

// Simple delay function for STM32
// Example from: http://thehackerworkshop.com/?p=1209
void CIO::delayInt(unsigned int dly)
{
#if defined(STM32F7XX)
  unsigned int loopsPerMillisecond = (SystemCoreClock/1000);
#else
  unsigned int loopsPerMillisecond = (SystemCoreClock/1000) / 3;
#endif

  for (; dly > 0; dly--)
  {
    asm volatile //this routine waits (approximately) one millisecond
    (
      "mov r3, %[loopsPerMillisecond] \n\t" //load the initial loop counter
      "loop: \n\t"
        "subs r3, #1 \n\t"
        "bne loop \n\t"

      : //empty output list
      : [loopsPerMillisecond] "r" (loopsPerMillisecond) //input to the asm routine
      : "r3", "cc" //clobber list
    );
  }
}

uint8_t CIO::getCPU() const
{
  return 2U;
}

void CIO::getUDID(uint8_t* buffer)
{
#if defined(STM32F4XX)
  ::memcpy(buffer, (void *)0x1FFF7A10, 12U);
#elif defined(STM32F722xx)
  ::memcpy(buffer, (void *)0x1FF07A10, 12U);
#elif defined(STM32F767xx)
  ::memcpy(buffer, (void *)0x1FF0F420, 12U);
#endif
}

#endif

