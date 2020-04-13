/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX / Geoffrey Merck F4FXL - KC3FRA
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


#if !defined(GOERTZEL_H)
#define  GOERTZEL_H

#include "Globals.h"

typedef struct 
{
    int sin, cos, coeff;
} TGoertzelParameters;


enum GOERTZEL_RESULT : uint8_t
{
    GR_NOT_READY = 0,
    GR_READY = 1
};

class CGoertzel {
public:
    //f1,f2,f3 are natural frequencies
    CGoertzel(const TGoertzelParameters& f1, const TGoertzelParameters& f2, const TGoertzelParameters& f3, const int* window, int windowCorr, uint16_t n);

    void reset();
    GOERTZEL_RESULT samples(const q15_t *samples, uint8_t length, unsigned int * f1MagSquared, unsigned int * f2MagSquared, unsigned int* f3MagSquared);

private:
    TGoertzelParameters m_freqs[3];

    q15_t m_min;
    q15_t m_max;

    q15_t m_omegas[3];
    int m_sines[3];
    int m_cosines[3];
    int m_coeffs[3];

    int m_q1s[3];
    int m_q2s[3];

    uint16_t m_processedSamplesCount;
    uint16_t m_n;
    
    const int* m_window;
    int m_windowCorr;
};

#endif
