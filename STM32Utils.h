/*
 *   Copyright (C) 2017 by Wojciech Krutnik N0CALL
 *
 *   Source: http://mightydevices.com/?p=144
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

#if !defined(STM32UTILS_H)
#define STM32UTILS_H

#include <stdint.h>

/* ram function */
#define RAMFUNC __attribute__ ((long_call, section (".data")))
/* eeprom data */
/* for placing variables in eeprom memory */
#define EEMEM 	__attribute__((section(".eeprom")))

/* bitband type */
typedef volatile uint32_t * const bitband_t;

/* base address for bit banding */
#define BITBAND_SRAM_REF               		(0x20000000)
/* base address for bit banding */
#define BITBAND_SRAM_BASE              		(0x22000000)
/* base address for bit banding */
#define BITBAND_PERIPH_REF               	(0x40000000)
/* base address for bit banding */
#define BITBAND_PERIPH_BASE              	(0x42000000)

/* sram bit band */
#define BITBAND_SRAM(address, bit)     ((void*)(BITBAND_SRAM_BASE +   \
		(((uint32_t)address) - BITBAND_SRAM_REF) * 32 + (bit) * 4))

/* periph bit band */
#define BITBAND_PERIPH(address, bit)   ((void *)(BITBAND_PERIPH_BASE + \
		(((uint32_t)address) - BITBAND_PERIPH_REF) * 32 + (bit) * 4))

#endif
