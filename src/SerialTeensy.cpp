/*
 *   Copyright (C) 2017 by Chris Huitema
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
 *
 *   28-4-2017 Created for MMDVM Pi shield for Teensy by Chris Huitema
 */

#include "Config.h"
#include "Globals.h"

#if defined(VK6MST_TEENSY_PI3_SHIELD_I2C)

#include <i2c_t3.h>   //available here https://github.com/nox771/i2c_t3 or maybe the normal wire will work #include <wire.h> justs need to test i guess

#define I2C_ADDRESS   0x22
#define I2C_SPEED     100000

// Function prototypes
void receiveEvent(size_t count);
void requestEvent(void);

#define TX_FIFO_SIZE 512U
#define RX_FIFO_SIZE 512U

volatile uint8_t  TXfifo[TX_FIFO_SIZE];
volatile uint8_t  RXfifo[RX_FIFO_SIZE];
volatile uint16_t TXfifohead, TXfifotail;
volatile uint16_t RXfifohead, RXfifotail;

// Init queues
void TXfifoinit(void)
{
   TXfifohead = 0U;
   TXfifotail = 0U;
}

void RXfifoinit()
{
   RXfifohead = 0U;
   RXfifotail = 0U;
}

// How full is queue

uint16_t TXfifolevel(void)
{
   uint32_t tail = TXfifotail;
   uint32_t head = TXfifohead;

   if (tail > head)
      return TX_FIFO_SIZE + head - tail;
   else
      return head - tail;
}

uint16_t RXfifolevel(void)
{
   uint32_t tail = RXfifotail;
   uint32_t head = RXfifohead;

   if (tail > head)
      return RX_FIFO_SIZE + head - tail;
   else
      return head - tail;
}


uint8_t TXfifoput(uint8_t next)
{
   if (TXfifolevel() < TX_FIFO_SIZE) {
      TXfifo[TXfifohead] = next;

      TXfifohead++;
      if (TXfifohead >= TX_FIFO_SIZE)
         TXfifohead = 0U;
      return 1U;
   } else {
      return 0U; // signal an overflow occurred by returning a zero count
   }
}

void I2Cbegin(void)
{
  // Setup for Slave mode, address 0x22, pins 18/19, external pullups, speed in hz
  Wire.begin(I2C_SLAVE, I2C_ADDRESS, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_SPEED);

    // register events
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    // initialize the fifos
    TXfifoinit();
    RXfifoinit();
}


int I2Cavailable(void)
{
  if (RXfifolevel() > 0U)
      return 1U;
    else
      return 0U;
}


uint8_t I2Cread(void)
{
  uint8_t data_c = RXfifo[RXfifotail];

   RXfifotail++;
   if (RXfifotail >= RX_FIFO_SIZE)
      RXfifotail = 0U;

   return data_c;
}


void I2Cwrite(const uint8_t* data, uint16_t length)
{
   for (uint16_t i = 0U; i < length; i++)
      TXfifoput(data[i]);   //puts it in the fifo
}

//
// handle Rx Event (incoming I2C data)
//
void receiveEvent(size_t count)
{
  for (uint16_t i = 0U; i < count; i++)
  {
    if (RXfifolevel() < RX_FIFO_SIZE) {
      RXfifo[RXfifohead] = Wire.readByte();
      if (RXfifo[RXfifohead] != -1){
        RXfifohead++;
        if (RXfifohead >= RX_FIFO_SIZE) RXfifohead = 0U;
      }
    } else {
      Wire.readByte();                  // drop data if mem full.
    }
    
  }
}

//
// handle Tx Event (outgoing I2C data)
//
void requestEvent(void)
{
    if (TXfifolevel() > 0) {
      if (Wire.write(TXfifo[TXfifotail])){  //write to i2c
        TXfifotail++;
        if (TXfifotail >= TX_FIFO_SIZE) TXfifotail = 0U;
      }  
    }
}



/************************************/

void CSerialPort::beginInt(uint8_t n, int speed)
{
  switch (n) {
    case 1U:
      return I2Cbegin();
    case 3U:
      Serial3.begin(speed);
      break;
    default:
      break;
  }
}

int CSerialPort::availableForReadInt(uint8_t n)
{
  switch (n) {
    case 1U:
      return I2Cavailable();
    case 3U:
      return Serial3.available();
    default:
      return false;
  }
}

int CSerialPort::availableForWriteInt(uint8_t n)
{
  switch (n) {
    case 3U:
      return Serial3.availableForWrite();
    default:
      return false;
  }
}

uint8_t CSerialPort::readInt(uint8_t n)
{
  switch (n) {
    case 1U:
      return I2Cread();
    case 3U:
      return Serial3.read();
    default:
      return 0U;
  }
}

void CSerialPort::writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush)
{
  switch (n) {
    case 1U:
      I2Cwrite(data, length);
      break;
    case 3U:
      Serial3.write(data, length);
      if (flush)
        Serial3.flush();
      break;
    default:
      break;
  }
}

#endif
