/*
 *   Copyright (C) 2016 by Jim McLaughlin KI6ZUM
 *   Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
 *   Copyright (c) 2017 by Jonathan Naylor G4KLX
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

#include "SerialPort.h"

/*
Pin definitions:

- Host communication:
USART1 - TXD PA9  - RXD PA10 (MMDVM-Pi board, MMDVM-Pi F722 board, MMDVM-F4M board, STM32F722-F7M board, STM32F4-DVM board, STM32F7-DVM board)
USART2 - TXD PA2  - RXD PA3  (Nucleo64 F446RE board, Morpho or Arduino header, MMDVM_RPT_Hat BG4TGO/BG5HHP board)
USART3 - TXD PC10 - RXD PC11 (Discovery board)
USART3 - TXD PD8  - RXD PD9  (Nucleo144 F767ZI board)

- Serial repeater:
USART1 - TXD PA9  - RXD PA10 (Nucleo with Arduino header)
UART5  - TXD PC12 - RXD PD2 (Discovery, MMDVM-Pi, MMDVM-Pi F722 board, MMDVM-F4M board, STM32F722-F7M board, STM32F4-DVM board, STM32F7-DVM board, Nucleo64 with Morpho header and Nucleo144 F767ZI, MMDVM_RPT_Hat BG4TGO/BG5HHP board)
*/

#if defined(STM32F4XX) || defined(STM32F7XX)

#include "STMUART.h"
extern "C" {
   void USART1_IRQHandler();
   void USART2_IRQHandler();
   void USART3_IRQHandler();
   void UART5_IRQHandler();
}

/* ************* USART1 ***************** */
#if defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_F7M) || defined(STM32F722_PI) || defined(STM32F722_RPT_HAT) || defined(STM32F4_DVM) || defined(STM32F7_DVM) || (defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)) || defined(DRCC_DVM) || defined(STM32F4_EDA_405) || defined(STM32F4_EDA_446)


static CSTMUART m_USART1;

void USART1_IRQHandler()
{
  m_USART1.handleIRQ();
}

void InitUSART1(int speed)
{
   // USART1 - TXD PA9  - RXD PA10 - pins on mmdvm pi board
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

   // USART IRQ init
   NVIC_InitStructure.NVIC_IRQChannel    = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
   NVIC_Init(&NVIC_InitStructure);

   // Configure USART as alternate function
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10; //  Tx | Rx
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Configure USART baud rate
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate   = speed;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART1, &USART_InitStructure);

   USART_Cmd(USART1, ENABLE);

   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

   m_USART1.init(USART1);
}

#endif

/* ************* USART2 ***************** */
#if defined(STM32F4_NUCLEO) || defined(STM32F4_RPT_HAT_TGO) || defined(DRCC_DVM)

static CSTMUART m_USART2;


void USART2_IRQHandler()
{
   m_USART2.handleIRQ();
}

void InitUSART2(int speed)
{
   // USART2 - TXD PA2  - RXD PA3
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

   // USART IRQ init
   NVIC_InitStructure.NVIC_IRQChannel    = USART2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
   NVIC_Init(&NVIC_InitStructure);

   // Configure USART as alternate function
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3; //  Tx | Rx
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Configure USART baud rate
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate   = speed;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART2, &USART_InitStructure);

   USART_Cmd(USART2, ENABLE);

   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

   m_USART2.init(USART2);
}

#endif

/* ************* USART3 ***************** */
#if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)

static CSTMUART m_USART3;

void USART3_IRQHandler()
{
   m_USART3.handleIRQ();
}

#if defined(STM32F7_NUCLEO)
// USART3 - TXD PD8 - RXD PD9
#define USART3_GPIO_PinSource_TX   GPIO_PinSource8
#define USART3_GPIO_PinSource_RX   GPIO_PinSource9
#define USART3_GPIO_Pin_TX         GPIO_Pin_8
#define USART3_GPIO_Pin_RX         GPIO_Pin_9
#define USART3_GPIO_PORT           GPIOD
#define USART3_RCC_Periph          RCC_AHB1Periph_GPIOD
#else
// USART3 - TXD PC10 - RXD PC11
#define USART3_GPIO_PinSource_TX   GPIO_PinSource10
#define USART3_GPIO_PinSource_RX   GPIO_PinSource11
#define USART3_GPIO_Pin_TX         GPIO_Pin_10
#define USART3_GPIO_Pin_RX         GPIO_Pin_11
#define USART3_GPIO_PORT           GPIOC
#define USART3_RCC_Periph          RCC_AHB1Periph_GPIOC
#endif

void InitUSART3(int speed)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

   RCC_AHB1PeriphClockCmd(USART3_RCC_Periph, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
   GPIO_PinAFConfig(USART3_GPIO_PORT, USART3_GPIO_PinSource_TX, GPIO_AF_USART3);
   GPIO_PinAFConfig(USART3_GPIO_PORT, USART3_GPIO_PinSource_RX, GPIO_AF_USART3);

   // USART IRQ init
   NVIC_InitStructure.NVIC_IRQChannel    = USART3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
   NVIC_Init(&NVIC_InitStructure);

   // Configure USART as alternate function
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Pin   = USART3_GPIO_Pin_TX | USART3_GPIO_Pin_RX; //  Tx | Rx
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);

   // Configure USART baud rate
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate   = speed;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART3, &USART_InitStructure);

   USART_Cmd(USART3, ENABLE);

   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

   m_USART3.init(USART3);
}

#endif

/* ************* UART5 ***************** */
#if !(defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER))

static CSTMUART m_UART5;

void UART5_IRQHandler()
{
   m_UART5.handleIRQ();
}

void InitUART5(int speed)
{
   // UART5 - TXD PC12 - RXD PD2
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

   // USART IRQ init
   NVIC_InitStructure.NVIC_IRQChannel    = UART5_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
   NVIC_Init(&NVIC_InitStructure);

   // Configure USART as alternate function
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12; //  Tx
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIOC, &GPIO_InitStructure);

   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; //  Rx
   GPIO_Init(GPIOD, &GPIO_InitStructure);

   // Configure USART baud rate
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate   = speed;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(UART5, &USART_InitStructure);

   USART_Cmd(UART5, ENABLE);

   USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

   m_UART5.init(UART5);
}

#endif
/////////////////////////////////////////////////////////////////

void CSerialPort::beginInt(uint8_t n, int speed)
{
   switch (n) {
      case 1U:
         #if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)
         InitUSART3(speed);
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M) || defined(STM32F722_RPT_HAT) || defined(STM32F4_DVM) || defined(STM32F7_DVM) || defined(STM32F4_EDA_405) || defined(STM32F4_EDA_446)
         InitUSART1(speed);
         #elif defined(STM32F4_NUCLEO) || defined(STM32F4_RPT_HAT_TGO)
         InitUSART2(speed);
         #elif defined(DRCC_DVM)
         InitUSART1(speed);
         #endif
         break;
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         InitUSART1(speed);
         #elif defined(DRCC_DVM)
         InitUSART2(speed);
         #else
         InitUART5(speed);
         #endif
         break;
      default:
         break;
   }
}

int CSerialPort::availableForReadInt(uint8_t n)
{
   switch (n) {
      case 1U:
         #if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)
         return m_USART3.available();//AvailUSART3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M) || defined(STM32F722_RPT_HAT) || defined(STM32F4_DVM) || defined(STM32F7_DVM) || defined(STM32F4_EDA_405) || defined(STM32F4_EDA_446)
         return m_USART1.available();//AvailUSART1();
         #elif defined(STM32F4_NUCLEO) || defined(STM32F4_RPT_HAT_TGO)
         return m_USART2.available();//AvailUSART2();
         #elif defined(DRCC_DVM)
         return m_USART1.available();//AvailUSART1();
         #endif
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         return m_USART1.available(); //AvailUSART1();
         #elif defined(DRCC_DVM)
         return m_USART2.available(); //AvailUSART2();
         #else
         return m_UART5.available();//AvailUART5();
         #endif
      default:
         return 0;
   }
}

int CSerialPort::availableForWriteInt(uint8_t n)
{
   switch (n) {
      case 1U:
         #if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)
         return m_USART3.availableForWrite(); //AvailForWriteUSART3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M) || defined(STM32F722_RPT_HAT) || defined(STM32F4_DVM) || defined(STM32F7_DVM) || defined(STM32F4_EDA_405) || defined(STM32F4_EDA_446)
         return m_USART1.availableForWrite(); //AvailForWriteUSART1();
         #elif defined(STM32F4_NUCLEO) || defined(STM32F4_RPT_HAT_TGO)
         return m_USART2.availableForWrite();//AvailForWriteUSART2();
         #elif defined(DRCC_DVM)
         return m_USART1.availableForWrite();//AvailForWriteUSART1();
         #endif
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         return m_USART1.availableForWrite(); //AvailForWriteUSART1();
         #elif defined(DRCC_DVM)
         return m_USART2.availableForWrite();//AvailForWriteUSART2();
         #else
         return m_UART5.availableForWrite();//AvailForWriteUART5();
         #endif
      default:
         return 0;
   }
}

uint8_t CSerialPort::readInt(uint8_t n)
{
   switch (n) {
      case 1U:
         #if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)
         return m_USART3.read();//ReadUSART3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M) || defined(STM32F722_RPT_HAT) || defined(STM32F4_DVM) || defined(STM32F7_DVM) || defined(STM32F4_EDA_405) || defined(STM32F4_EDA_446)
         return m_USART1.read();//ReadUSART1();
         #elif defined(STM32F4_NUCLEO) || defined(STM32F4_RPT_HAT_TGO)
         return m_USART2.read();//ReadUSART2();
         #elif defined(DRCC_DVM)
         return m_USART1.read();//ReadUSART1();
         #endif
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         return m_USART1.read();//ReadUSART1();
         #elif defined(DRCC_DVM)
         return m_USART2.read();//ReadUSART2();
         #else
         return m_UART5.read();//ReadUART5();
         #endif
      default:
         return 0U;
   }
}

void CSerialPort::writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush)
{
   switch (n) {
      case 1U:
         #if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)
         m_USART3.write(data, length); //WriteUSART3(data, length);
         if (flush)
            m_USART3.flush();//TXSerialFlush3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M) || defined(STM32F722_RPT_HAT) || defined(STM32F4_DVM) || defined(STM32F7_DVM) || defined(STM32F4_EDA_405) || defined(STM32F4_EDA_446)
         m_USART1.write(data, length);//WriteUSART1(data, length);
         if (flush)
            m_USART1.flush();//TXSerialFlush1();
         #elif defined(STM32F4_NUCLEO) || defined(STM32F4_RPT_HAT_TGO)
         m_USART2.write(data, length);//WriteUSART2(data, length);
         if (flush)
            m_USART2.flush();//TXSerialFlush2();
         #elif defined(DRCC_DVM)
         m_USART1.write(data, length);//WriteUSART1(data, length);
         if (flush)
            m_USART1.flush();//TXSerialFlush1();
         #endif
         break;
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         m_USART1.write(data, length); //WriteUSART1(data, length);
         if (flush)
            m_USART1.flush();
         #elif defined(DRCC_DVM)
         m_USART2.write(data, length);//WriteUSART2(data, length);
         if (flush)
            m_USART2.flush();//TXSerialFlush2();
         #else
         m_UART5.write(data, length);//WriteUART5(data, length);
         if (flush)
            m_UART5.flush();//TXSerialFlush5();
         #endif
         break;
      default:
         break;
   }
}

#endif

