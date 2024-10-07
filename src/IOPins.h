/*
 *   Copyright (C) 2019,2020 by BG5HHP
 *   Copyright (C) 2024 by Jonathan Naylor, G4KLX
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

#if !defined(_IO_PINS_H)
#define _IO_PINS_H

#if defined(ZUM_V09_PI)
    #include "pins/pins_zum_v09_pi.h"

#elif defined(ZUM_V10_PI)
    #include "pins/pins_zum_v10_pi.h"

#elif defined(RB_V3_PI)
    #include "pins/pins_rb_v3_pi.h"

#elif defined(RB_V5_PI)
    #include "pins/pins_rb_v5_pi.h"

#else
    #error "A valid board type macro need to be defined."

#endif

#endif  //#ifndef _IO_PINS_H
