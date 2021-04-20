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

#ifndef _IO_PINS_H
#define _IO_PINS_H

#if !defined(CONFIG_H)
#error "Requires Config.h to be included first!"
#endif

#if defined(STM32F4_RPT_HAT_TGO)
    #include "pins/pins_f4_rpt_tgo.h"

#elif defined(STM32F4_DISCOVERY)
    #include "pins/pins_f4_discovery.h"

#elif defined(STM32F4_PI)
    #include "pins/pins_f4_pi.h"

#elif defined(STM32F722_PI)
    #include "pins/pins_f7_pi.h"

#elif defined(STM32F4_F4M)
    #include "pins/pins_f4_f4m.h"

#elif defined(STM32F722_F7M)
    #include "pins/pins_f7_f7m.h"

#elif defined(STM32F722_RPT_HAT)
    #include "pins/pins_f7_rpt_hat.h"

#elif defined(STM32F4_NUCLEO)
    #if defined(STM32F4_NUCLEO_MORPHO_HEADER)
        #include "pins/pins_f4_nucleo_morpho.h"
    #elif defined(STM32F4_NUCLEO_ARDUINO_HEADER)
        #include "pins/pins_f4_nucleo_arduino.h"
    #else
        #error "Either STM32F4_NUCLEO_MORPHO_HEADER or STM32F4_NUCLEO_ARDUINO_HEADER need to be defined"
    #endif

#elif defined(STM32F7_NUCLEO)
    #include "pins/pins_f7_nucleo.h"

#elif defined(STM32F4_DVM)
    #include "pins/pins_f4_stm32dvm_v3.h"

#elif defined(STM32F7_DVM)
    #include "pins/pins_f7_stm32dvm_v5.h"

#elif defined(DRCC_DVM_NQF)
    #include "pins/pins_f4_drcc_nqf.h"

#elif defined(DRCC_DVM_HHP446)
    #include "pins/pins_f4_drcc_hhp446.h"

#elif defined(STM32F4_EDA_405) || defined(STM32F4_EDA_446)
    #include "pins/pins_f4_stm32eda.h"

#else
    #error "A valid board type macro need to be defined."

#endif

#endif  //#ifndef _IO_PINS_H
