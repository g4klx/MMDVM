/*
 *   Copyright (C) 2020,2021 by Jonathan Naylor G4KLX
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

#if defined(MODE_OLED)

#include "I2C3.h"

#include "Globals.h"


#define	I2C_WRITE_REG(REG, VAL)			((REG) = (VAL))
#define	I2C_READ_REG(REG)				((REG))

#define	I2C_CLEAR_FLAG(__FLAG__)			(((__FLAG__) == I2C_FLAG_TXE) ? (I2C3->ISR |= (__FLAG__)) : (I2C3->ICR = (__FLAG__)))
#define	I2C_GET_FLAG(__FLAG__)				((((I2C3->ISR) & (__FLAG__)) == (__FLAG__)) ? SET : RESET)

#define	I2C_RESET_CR2()				(I2C3->CR2 &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)))

#define	I2C_MODIFY_REG(REG, CLEARMASK, SETMASK)	I2C_WRITE_REG((REG), (((I2C_READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define	TIMING_CLEAR_MASK				(0xF0FFFFFFU)

#define	I2C_DUALADDRESS_DISABLE			(0x00000000U)
#define	I2C_DUALADDRESS_ENABLE				I2C_OAR2_OA2EN

#define	I2C_GENERALCALL_DISABLE			(0x00000000U)

#define	I2C_NOSTRETCH_DISABLE				(0x00000000U)

#define	I2C_ANALOGFILTER_ENABLE			0x00000000U

#define	RCC_I2C3CLKSOURCE_PCLK1			((uint32_t)0x00000000U)

#define	I2C_OA2_NOMASK					((uint8_t)0x00U)

#define	I2C_ENABLE()					(SET_BIT(I2C3->CR1,  I2C_CR1_PE))

#define	I2C_DISABLE()					(CLEAR_BIT(I2C3->CR1, I2C_CR1_PE))

#define	RCC_GPIOA_CLK_ENABLE()				do { \
									__IO uint32_t tmpreg; \
									SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);\
									tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);\
								} while(0)

#define	RCC_GPIOC_CLK_ENABLE()				do { \
									__IO uint32_t tmpreg; \
									SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);\
									tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);\
								} while(0)

CI2C3::CI2C3()
{
}

void CI2C3::init()
{
  // Enable the I2C3 clock
  I2C_MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C3SEL, RCC_I2C3CLKSOURCE_PCLK1);

  // Enable the GPIOs for the SCL/SDA Pins
  RCC_GPIOC_CLK_ENABLE();
  RCC_GPIOA_CLK_ENABLE();

  initI2C3();

  // Configure Analogue filter
  configAnalogFilter(I2C_ANALOGFILTER_ENABLE);

  // Configure Digital filter
  configDigitalFilter(0U);
}

void CI2C3::write(uint8_t addr, const uint8_t* data, uint8_t length)
{
  DEBUG2("OLED Data", addr);
  DEBUG_DUMP(data, length);

  // Wait for the I2C transmitter to become free
  while (I2C_GET_FLAG(I2C_FLAG_BUSY) == SET)
    ;

  // Configure the data transfer
  transferConfig(addr, length, I2C_CR2_AUTOEND, I2C_Generate_Start_Write);

  // Start Writing Data
  for (uint16_t i = 0U; i < length; i++) {
    // Wait for the TXIS flag to be set
    while (I2C_GET_FLAG(I2C_FLAG_TXIS) == RESET)
	  ;

    // Write the byte to the TXDR
    I2C3->TXDR = data[i];
  }

  // No need to Check TC flag, with AUTOEND mode the stop is automatically generated Wait until STOPF flag is set
  while (I2C_GET_FLAG(I2C_FLAG_STOPF) == RESET)
    ;

  // Clear STOP flag
  I2C_CLEAR_FLAG(I2C_FLAG_STOPF);

  // Clear Configuration Register 2
  I2C_RESET_CR2();
}

void CI2C3::transferConfig(uint16_t addr, uint8_t length, uint32_t mode, uint32_t request)
{
  // Update CR2 register
  I2C_MODIFY_REG(I2C3->CR2, ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | \
                (I2C_CR2_RD_WRN & (uint32_t)(request >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)), \
                (uint32_t)(((uint32_t)addr & I2C_CR2_SADD) |
                        (((uint32_t)length << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)mode | (uint32_t)request));
}

void CI2C3::configAnalogFilter(uint32_t AnalogFilter)
{
  // Disable the selected I2C peripheral
  I2C_DISABLE();

  // Reset I2Cx ANOFF bit
  I2C3->CR1 &= ~(I2C_CR1_ANFOFF);

  // Set analog filter bit
  I2C3->CR1 |= AnalogFilter;

  I2C_ENABLE();
}

void CI2C3::configDigitalFilter(uint32_t DigitalFilter)
{
  // Disable the selected I2C peripheral
  I2C_DISABLE();

  // Get the old register value
  uint32_t tmpreg = I2C3->CR1;

  // Reset I2Cx DNF bits [11:8]
  tmpreg &= ~(I2C_CR1_DNF);

  // Set I2Cx DNF coefficient
  tmpreg |= DigitalFilter << 8U;

  // Store the new register value
  I2C3->CR1 = tmpreg;

  I2C_ENABLE();
}

void CI2C3::initI2C3()
{
  /* Disable the selected I2C peripheral */
  I2C_DISABLE();

  /*---------------------------- I2Cx TIMINGR Configuration ------------------*/
  /* Configure I2Cx: Frequency range */
  I2C3->TIMINGR = 0x0010061AU & TIMING_CLEAR_MASK;

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Disable Own Address1 before set the Own Address1 configuration */
  I2C3->OAR1 &= ~I2C_OAR1_OA1EN;

  /* Configure I2Cx: Own Address1 and ack own address1 mode */
  I2C3->OAR1 = (I2C_OAR1_OA1EN | 0U);

  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
  I2C3->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Disable Own Address2 before set the Own Address2 configuration */
  I2C3->OAR2 &= ~I2C_DUALADDRESS_ENABLE;

  /* Configure I2Cx: Dual mode and Own Address2 */
  I2C3->OAR2 = (I2C_DUALADDRESS_DISABLE | 0U | (I2C_OA2_NOMASK << 8));

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  I2C3->CR1 = (I2C_GENERALCALL_DISABLE | I2C_NOSTRETCH_DISABLE);

  /* Enable the selected I2C peripheral */
  I2C_ENABLE();
}

#endif

