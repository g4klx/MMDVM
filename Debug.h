/*
 *   Copyright (C) 2015 by Jonathan Naylor G4KLX
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

#if !defined(DEBUG_H)
#define  DEBUG_H

#include "Config.h"

#if defined(WANT_DEBUG)

#include "Globals.h"

#define  DEBUG1(a)          serial.writeDebug((a))
#define  DEBUG2(a,b)        serial.writeDebug((a),(b))
#define  DEBUG3(a,b,c)      serial.writeDebug((a),(b),(c))
#define  DEBUG4(a,b,c,d)    serial.writeDebug((a),(b),(c),(d))
#define  DEBUG5(a,b,c,d,e)  serial.writeDebug((a),(b),(c),(d),(e))
#define  ASSERT(a)          serial.writeAssert((a),#a,__FILE__,__LINE__)
#define  DUMP(a,b)          serial.writeDump((a),(b))
#define  SAMPLES(a,b)       serial.writeSamples((a),(b))

#else

#define  DEBUG1(a)          
#define  DEBUG2(a,b)        
#define  DEBUG3(a,b,c)      
#define  DEBUG4(a,b,c,d)    
#define  DEBUG5(a,b,c,d,e)  
#define  ASSERT(a)          
#define  DUMP(a,b)          
#define  SAMPLES(a,b)       

#endif

#endif

