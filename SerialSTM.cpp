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
USART1 - TXD PA9  - RXD PA10 (MMDVM-Pi board, MMDVM-Pi F722 board, MMDVM-F4M board, STM32F722-F7M board)
USART2 - TXD PA2  - RXD PA3  (Nucleo64 F446RE board, Morpho or Arduino header)
USART3 - TXD PC10 - RXD PC11 (Discovery board)
USART3 - TXD PD8  - RXD PD9  (Nucleo144 F767ZI board)

- Serial repeater:
USART1 - TXD PA9  - RXD PA10 (Nucleo with Arduino header)
UART5  - TXD PC12 - RXD PD2 (Discovery, MMDVM-Pi, MMDVM-Pi F722 board, MMDVM-F4M board, STM32F722-F7M board, Nucleo64 with Morpho header and Nucleo144 F767ZI)
*/

#if defined(STM32F4XX) || defined(STM32F7XX)

#define TX_SERIAL_FIFO_SIZE 512U
#define RX_SERIAL_FIFO_SIZE 512U

extern "C" {
   void USART1_IRQHandler();
   void USART2_IRQHandler();
   void USART3_IRQHandler();
   void UART5_IRQHandler();
}

/* ************* USART1 ***************** */
#if defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_F7M) || defined(STM32F722_PI) || (defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER))

volatile uint8_t  TXSerialfifo1[TX_SERIAL_FIFO_SIZE];
volatile uint8_t  RXSerialfifo1[RX_SERIAL_FIFO_SIZE];
volatile uint16_t TXSerialfifohead1, TXSerialfifotail1;
volatile uint16_t RXSerialfifohead1, RXSerialfifotail1;

// Init queues
void TXSerialfifoinit1()
{
   TXSerialfifohead1 = 0U;
   TXSerialfifotail1 = 0U;
}

void RXSerialfifoinit1()
{
   RXSerialfifohead1 = 0U;
   RXSerialfifotail1 = 0U;
}

// How full is queue
// TODO decide if how full or how empty is preferred info to return
uint16_t TXSerialfifolevel1()
{
   uint32_t tail = TXSerialfifotail1;
   uint32_t head = TXSerialfifohead1;

   if (tail > head)
      return TX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

uint16_t RXSerialfifolevel1()
{
   uint32_t tail = RXSerialfifotail1;
   uint32_t head = RXSerialfifohead1;

   if (tail > head)
      return RX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

// Flushes the transmit shift register
// warning: this call is blocking
void TXSerialFlush1()
{
   // wait until the TXE shows the shift register is empty
   while (USART_GetITStatus(USART1, USART_FLAG_TXE))
      ;
}

uint8_t TXSerialfifoput1(uint8_t next)
{
   if (TXSerialfifolevel1() < TX_SERIAL_FIFO_SIZE) {
      TXSerialfifo1[TXSerialfifohead1] = next;

      TXSerialfifohead1++;
      if (TXSerialfifohead1 >= TX_SERIAL_FIFO_SIZE)
         TXSerialfifohead1 = 0U;

      // make sure transmit interrupts are enabled as long as there is data to send
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
      return 1U;
   } else {
      return 0U; // signal an overflow occurred by returning a zero count
   }
}

void USART1_IRQHandler()
{
   uint8_t c;

   if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
      c = (uint8_t) USART_ReceiveData(USART1);

      if (RXSerialfifolevel1() < RX_SERIAL_FIFO_SIZE) {
         RXSerialfifo1[RXSerialfifohead1] = c;

         RXSerialfifohead1++;
         if (RXSerialfifohead1 >= RX_SERIAL_FIFO_SIZE)
            RXSerialfifohead1 = 0U;
      } else {
         // TODO - do something if rx fifo is full?
      }

      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
   }

   if (USART_GetITStatus(USART1, USART_IT_TXE)) {
      c = 0U;

      if (TXSerialfifohead1 != TXSerialfifotail1) { // if the fifo is not empty
         c = TXSerialfifo1[TXSerialfifotail1];

         TXSerialfifotail1++;
         if (TXSerialfifotail1 >= TX_SERIAL_FIFO_SIZE)
            TXSerialfifotail1 = 0U;

         USART_SendData(USART1, c);
      } else { // if there's no more data to transmit then turn off TX interrupts
         USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      }

      USART_ClearITPendingBit(USART1, USART_IT_TXE);
   }
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

   // initialize the fifos
   TXSerialfifoinit1();
   RXSerialfifoinit1();
}

uint8_t AvailUSART1()
{
   if (RXSerialfifolevel1() > 0U)
      return 1U;
   else
      return 0U;
}

int AvailForWriteUSART1()
{
   return TX_SERIAL_FIFO_SIZE - TXSerialfifolevel1();
}

uint8_t ReadUSART1()
{
   uint8_t data_c = RXSerialfifo1[RXSerialfifotail1];

   RXSerialfifotail1++;
   if (RXSerialfifotail1 >= RX_SERIAL_FIFO_SIZE)
      RXSerialfifotail1 = 0U;

   return data_c;
}

void WriteUSART1(const uint8_t* data, uint16_t length)
{
   for (uint16_t i = 0U; i < length; i++)
      TXSerialfifoput1(data[i]);

   USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

#endif

/* ************* USART2 ***************** */
#if defined(STM32F4_NUCLEO)

volatile uint8_t  TXSerialfifo2[TX_SERIAL_FIFO_SIZE];
volatile uint8_t  RXSerialfifo2[RX_SERIAL_FIFO_SIZE];
volatile uint16_t TXSerialfifohead2, TXSerialfifotail2;
volatile uint16_t RXSerialfifohead2, RXSerialfifotail2;

// Init queues
void TXSerialfifoinit2()
{
   TXSerialfifohead2 = 0U;
   TXSerialfifotail2 = 0U;
}

void RXSerialfifoinit2()
{
   RXSerialfifohead2 = 0U;
   RXSerialfifotail2 = 0U;
}

// How full is queue
// TODO decide if how full or how empty is preferred info to return
uint16_t TXSerialfifolevel2()
{
   uint32_t tail = TXSerialfifotail2;
   uint32_t head = TXSerialfifohead2;

   if (tail > head)
      return TX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

uint16_t RXSerialfifolevel2()
{
   uint32_t tail = RXSerialfifotail2;
   uint32_t head = RXSerialfifohead2;

   if (tail > head)
      return RX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

// Flushes the transmit shift register
// warning: this call is blocking
void TXSerialFlush2()
{
   // wait until the TXE shows the shift register is empty
   while (USART_GetITStatus(USART2, USART_FLAG_TXE))
      ;
}

uint8_t TXSerialfifoput2(uint8_t next)
{
   if (TXSerialfifolevel2() < TX_SERIAL_FIFO_SIZE) {
      TXSerialfifo2[TXSerialfifohead2] = next;

      TXSerialfifohead2++;
      if (TXSerialfifohead2 >= TX_SERIAL_FIFO_SIZE)
         TXSerialfifohead2 = 0U;

      // make sure transmit interrupts are enabled as long as there is data to send
      USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
      return 1U;
   } else {
      return 0U; // signal an overflow occurred by returning a zero count
   }
}

void USART2_IRQHandler()
{
   uint8_t c;

   if (USART_GetITStatus(USART2, USART_IT_RXNE)) {
      c = (uint8_t) USART_ReceiveData(USART2);

      if (RXSerialfifolevel2() < RX_SERIAL_FIFO_SIZE) {
         RXSerialfifo2[RXSerialfifohead2] = c;

         RXSerialfifohead2++;
         if (RXSerialfifohead2 >= RX_SERIAL_FIFO_SIZE)
            RXSerialfifohead2 = 0U;
      } else {
         // TODO - do something if rx fifo is full?
      }

      USART_ClearITPendingBit(USART2, USART_IT_RXNE);
   }

   if (USART_GetITStatus(USART2, USART_IT_TXE)) {
      c = 0U;

      if (TXSerialfifohead2 != TXSerialfifotail2) { // if the fifo is not empty
         c = TXSerialfifo2[TXSerialfifotail2];

         TXSerialfifotail2++;
         if (TXSerialfifotail2 >= TX_SERIAL_FIFO_SIZE)
            TXSerialfifotail2 = 0U;

         USART_SendData(USART2, c);
      } else { // if there's no more data to transmit then turn off TX interrupts
         USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
      }

      USART_ClearITPendingBit(USART2, USART_IT_TXE);
   }
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

   // initialize the fifos
   TXSerialfifoinit2();
   RXSerialfifoinit2();
}

uint8_t AvailUSART2()
{
   if (RXSerialfifolevel2() > 0U)
      return 1U;
   else
      return 0U;
}

int AvailForWriteUSART2()
{
   return TX_SERIAL_FIFO_SIZE - TXSerialfifolevel2();
}

uint8_t ReadUSART2()
{
   uint8_t data_c = RXSerialfifo2[RXSerialfifotail2];

   RXSerialfifotail2++;
   if (RXSerialfifotail2 >= RX_SERIAL_FIFO_SIZE)
      RXSerialfifotail2 = 0U;

   return data_c;
}

void WriteUSART2(const uint8_t* data, uint16_t length)
{
   for (uint16_t i = 0U; i < length; i++)
      TXSerialfifoput2(data[i]);

   USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

#endif

/* ************* USART3 ***************** */
#if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)

volatile uint8_t  TXSerialfifo3[TX_SERIAL_FIFO_SIZE];
volatile uint8_t  RXSerialfifo3[RX_SERIAL_FIFO_SIZE];
volatile uint16_t TXSerialfifohead3, TXSerialfifotail3;
volatile uint16_t RXSerialfifohead3, RXSerialfifotail3;

// Init queues
void TXSerialfifoinit3()
{
   TXSerialfifohead3 = 0U;
   TXSerialfifotail3 = 0U;
}

void RXSerialfifoinit3()
{
   RXSerialfifohead3 = 0U;
   RXSerialfifotail3 = 0U;
}

// How full is queue
// TODO decide if how full or how empty is preferred info to return
uint16_t TXSerialfifolevel3()
{
   uint32_t tail = TXSerialfifotail3;
   uint32_t head = TXSerialfifohead3;

   if (tail > head)
      return TX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

uint16_t RXSerialfifolevel3()
{
   uint32_t tail = RXSerialfifotail3;
   uint32_t head = RXSerialfifohead3;

   if (tail > head)
      return RX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

// Flushes the transmit shift register
// warning: this call is blocking
void TXSerialFlush3()
{
   // wait until the TXE shows the shift register is empty
   while (USART_GetITStatus(USART3, USART_FLAG_TXE))
      ;
}

uint8_t TXSerialfifoput3(uint8_t next)
{
   if (TXSerialfifolevel3() < TX_SERIAL_FIFO_SIZE) {
      TXSerialfifo3[TXSerialfifohead3] = next;

      TXSerialfifohead3++;
      if (TXSerialfifohead3 >= TX_SERIAL_FIFO_SIZE)
         TXSerialfifohead3 = 0U;

      // make sure transmit interrupts are enabled as long as there is data to send
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
      return 1U;
   } else {
      return 0U; // signal an overflow occurred by returning a zero count
   }
}

void USART3_IRQHandler()
{
   uint8_t c;

   if (USART_GetITStatus(USART3, USART_IT_RXNE)) {
      c = (uint8_t) USART_ReceiveData(USART3);

      if (RXSerialfifolevel3() < RX_SERIAL_FIFO_SIZE) {
         RXSerialfifo3[RXSerialfifohead3] = c;

         RXSerialfifohead3++;
         if (RXSerialfifohead3 >= RX_SERIAL_FIFO_SIZE)
            RXSerialfifohead3 = 0U;
      } else {
         // TODO - do something if rx fifo is full?
      }

      USART_ClearITPendingBit(USART3, USART_IT_RXNE);
   }

   if (USART_GetITStatus(USART3, USART_IT_TXE)) {
      c = 0U;

      if (TXSerialfifohead3 != TXSerialfifotail3) { // if the fifo is not empty
         c = TXSerialfifo3[TXSerialfifotail3];

         TXSerialfifotail3++;
         if (TXSerialfifotail3 >= TX_SERIAL_FIFO_SIZE)
            TXSerialfifotail3 = 0U;

         USART_SendData(USART3, c);
      } else { // if there's no more data to transmit then turn off TX interrupts
         USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      }

      USART_ClearITPendingBit(USART3, USART_IT_TXE);
   }
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

   // initialize the fifos
   TXSerialfifoinit3();
   RXSerialfifoinit3();
}

uint8_t AvailUSART3()
{
   if (RXSerialfifolevel3() > 0U)
      return 1U;
   else
      return 0U;
}

int AvailForWriteUSART3()
{
   return TX_SERIAL_FIFO_SIZE - TXSerialfifolevel3();
}

uint8_t ReadUSART3()
{
   uint8_t data_c = RXSerialfifo3[RXSerialfifotail3];

   RXSerialfifotail3++;
   if (RXSerialfifotail3 >= RX_SERIAL_FIFO_SIZE)
      RXSerialfifotail3 = 0U;

   return data_c;
}

void WriteUSART3(const uint8_t* data, uint16_t length)
{
   for (uint16_t i = 0U; i < length; i++)
      TXSerialfifoput3(data[i]);

   USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

#endif

/* ************* UART5 ***************** */
#if !(defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER))

volatile uint8_t  TXSerialfifo5[TX_SERIAL_FIFO_SIZE];
volatile uint8_t  RXSerialfifo5[RX_SERIAL_FIFO_SIZE];
volatile uint16_t TXSerialfifohead5, TXSerialfifotail5;
volatile uint16_t RXSerialfifohead5, RXSerialfifotail5;

// Init queues
void TXSerialfifoinit5()
{
   TXSerialfifohead5 = 0U;
   TXSerialfifotail5 = 0U;
}

void RXSerialfifoinit5()
{
   RXSerialfifohead5 = 0U;
   RXSerialfifotail5 = 0U;
}

// How full is queue
// TODO decide if how full or how empty is preferred info to return
uint16_t TXSerialfifolevel5()
{
   uint32_t tail = TXSerialfifotail5;
   uint32_t head = TXSerialfifohead5;

   if (tail > head)
      return TX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

uint16_t RXSerialfifolevel5()
{
   uint32_t tail = RXSerialfifotail5;
   uint32_t head = RXSerialfifohead5;

   if (tail > head)
      return RX_SERIAL_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

// Flushes the transmit shift register
// warning: this call is blocking
void TXSerialFlush5()
{
   // wait until the TXE shows the shift register is empty
   while (USART_GetITStatus(UART5, USART_FLAG_TXE))
      ;
}

uint8_t TXSerialfifoput5(uint8_t next)
{
   if (TXSerialfifolevel5() < TX_SERIAL_FIFO_SIZE) {
      TXSerialfifo5[TXSerialfifohead5] = next;

      TXSerialfifohead5++;
      if (TXSerialfifohead5 >= TX_SERIAL_FIFO_SIZE)
         TXSerialfifohead5 = 0U;

      // make sure transmit interrupts are enabled as long as there is data to send
      USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
      return 1U;
   } else {
      return 0U; // signal an overflow occurred by returning a zero count
   }
}

void UART5_IRQHandler()
{
   uint8_t c;

   if (USART_GetITStatus(UART5, USART_IT_RXNE)) {
      c = (uint8_t) USART_ReceiveData(UART5);

      if (RXSerialfifolevel5() < RX_SERIAL_FIFO_SIZE) {
         RXSerialfifo5[RXSerialfifohead5] = c;

         RXSerialfifohead5++;
         if (RXSerialfifohead5 >= RX_SERIAL_FIFO_SIZE)
            RXSerialfifohead5 = 0U;
      } else {
         // TODO - do something if rx fifo is full?
      }

      USART_ClearITPendingBit(UART5, USART_IT_RXNE);
   }

   if (USART_GetITStatus(UART5, USART_IT_TXE)) {
      c = 0U;

      if (TXSerialfifohead5 != TXSerialfifotail5) { // if the fifo is not empty
         c = TXSerialfifo5[TXSerialfifotail5];

         TXSerialfifotail5++;
         if (TXSerialfifotail5 >= TX_SERIAL_FIFO_SIZE)
            TXSerialfifotail5 = 0U;

         USART_SendData(UART5, c);
      } else { // if there's no more data to transmit then turn off TX interrupts
      USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
      }

      USART_ClearITPendingBit(UART5, USART_IT_TXE);
   }
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

   // initialize the fifos
   TXSerialfifoinit5();
   RXSerialfifoinit5();
}

uint8_t AvailUART5()
{
   if (RXSerialfifolevel5() > 0U)
      return 1U;
   else
      return 0U;
}

int AvailForWriteUART5()
{
   return TX_SERIAL_FIFO_SIZE - TXSerialfifolevel5();
}

uint8_t ReadUART5()
{
   uint8_t data_c = RXSerialfifo5[RXSerialfifotail5];

   RXSerialfifotail5++;
   if (RXSerialfifotail5 >= RX_SERIAL_FIFO_SIZE)
      RXSerialfifotail5 = 0U;

   return data_c;
}

void WriteUART5(const uint8_t* data, uint16_t length)
{
   for (uint16_t i = 0U; i < length; i++)
      TXSerialfifoput5(data[i]);

   USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
}

#endif
/////////////////////////////////////////////////////////////////

void CSerialPort::beginInt(uint8_t n, int speed)
{
   switch (n) {
      case 1U:
         #if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)
         InitUSART3(speed);
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M)
         InitUSART1(speed);
         #elif defined(STM32F4_NUCLEO)
         InitUSART2(speed);
         #endif
         break;
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         InitUSART1(speed);
         #else
         InitUART5(speed);
         #endif
         break;
      default:
         break;
   }
}

int CSerialPort::availableInt(uint8_t n)
{
   switch (n) {
      case 1U:
         #if defined(STM32F4_DISCOVERY) || defined(STM32F7_NUCLEO)
         return AvailUSART3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M)
         return AvailUSART1();
         #elif defined(STM32F4_NUCLEO)
         return AvailUSART2();
         #endif
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         return AvailUSART1();
         #else
         return AvailUART5();
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
         return AvailForWriteUSART3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M)
         return AvailForWriteUSART1();
         #elif defined(STM32F4_NUCLEO)
         return AvailForWriteUSART2();
         #endif
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         return AvailForWriteUSART1();
         #else
         return AvailForWriteUART5();
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
         return ReadUSART3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M)
         return ReadUSART1();
         #elif defined(STM32F4_NUCLEO)
         return ReadUSART2();
         #endif
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         return ReadUSART1();
         #else
         return ReadUART5();
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
         WriteUSART3(data, length);
         if (flush)
            TXSerialFlush3();
         #elif defined(STM32F4_PI) || defined(STM32F4_F4M) || defined(STM32F722_PI) || defined(STM32F722_F7M)
         WriteUSART1(data, length);
         if (flush)
            TXSerialFlush1();
         #elif defined(STM32F4_NUCLEO)
         WriteUSART2(data, length);
         if (flush)
            TXSerialFlush2();
         #endif
         break;
      case 3U:
         #if defined(STM32F4_NUCLEO) && defined(STM32F4_NUCLEO_ARDUINO_HEADER)
         WriteUSART1(data, length);
         if (flush)
            TXSerialFlush1();
         #else
         WriteUART5(data, length);
         if (flush)
            TXSerialFlush5();
         #endif
         break;
      default:
         break;
   }
}

#endif
