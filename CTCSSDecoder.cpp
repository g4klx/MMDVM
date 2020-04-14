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

#include "CTCSSDecoder.h"

CCTCSSDEcoder::CCTCSSDEcoder(const TCTCSSTone& tone, q15_t threshold) :
m_thresholdSquared(threshold * threshold),
m_hasCTCSS(false)
{
    m_goertzel = new CGoertzel(tone.toneLow, tone.tone, tone.toneHi, HANN_WINDOW, HANN_WINDOW_CORR, N_SAMPLES);
}


void CCTCSSDEcoder::samples(const q15_t* samples, uint8_t length)
{
    unsigned int f1MagSquared, f2MagSquared, f3MagSquared;
    GOERTZEL_RESULT result = m_goertzel->samples(samples, length, &f1MagSquared, &f2MagSquared, &f3MagSquared);
    
    if(result == GR_READY) {
        // Goertzel says it has something for us, so checkt it
        // make sure the strongest tone is the wanted one and not the neighbouring tone
        m_hasCTCSS  =   f2MagSquared >= m_thresholdSquared
                        && f2MagSquared > f1MagSquared
                        && f2MagSquared > f3MagSquared;
    }
}

bool CCTCSSDEcoder::hasCTCSS()
{
    return m_hasCTCSS;
}
