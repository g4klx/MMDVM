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

#include "Goertzel.h"

CGoertzel::CGoertzel(const TGoertzelParameters& f1, const TGoertzelParameters& f2, const TGoertzelParameters& f3, const int* window, int windowCorr, uint16_t n) :
m_min(0),
m_max(0),
m_processedSamplesCount(0),
m_n(n),
m_window(window),//Window should not be deleted by someone else
m_windowCorr(windowCorr)
{
    m_freqs[0] = f1;
    m_freqs[1] = f2;
    m_freqs[2] = f3;

    reset();
}

void CGoertzel::reset()
{
    ::memset(m_q1s, 0, sizeof(m_q1s));
    ::memset(m_q2s, 0, sizeof(m_q2s));
    m_processedSamplesCount = 0U;
    m_max = 0U;
    m_min = 0U;
}

GOERTZEL_RESULT CGoertzel::samples(const q15_t *samples, uint8_t length, unsigned int * f1MagSquared, unsigned int * f2MagSquared, unsigned int* f3MagSquared)
{
    int scalingFactor = (length / 2) * m_windowCorr;
    unsigned int * magnitudes[3] = {f1MagSquared, f2MagSquared, f3MagSquared};
    GOERTZEL_RESULT magnitudesComputed = GR_NOT_READY;

    for(uint8_t sampleIdx = 0; sampleIdx < length; sampleIdx++) {

        if(samples[sampleIdx] < m_min) m_min = samples[sampleIdx];
        if(samples[sampleIdx] > m_max) m_max = samples[sampleIdx];

        for(uint8_t i = 0; i < 3; i++) {
            int q0 = m_freqs[i].coeff * m_q1s[i] - m_q2s[i] + (samples[sampleIdx] * m_window[m_processedSamplesCount]);
            m_q2s[i] = m_q1s[i];
            m_q1s[i] = q0;
        }

        m_processedSamplesCount++;
        //we have collected enough samples, evaluate now,
        if(m_processedSamplesCount == m_n) {
            if(magnitudesComputed == GR_NOT_READY) {
                //however if we already had collected enough samples only keep the magnitudes we computed the first time
                int span = m_max - m_min;
                for(uint8_t i = 0; i < 3; i++) {
                    int real = ((m_q1s[i] * m_freqs[i].cos - m_q2s[i]) / scalingFactor)/span;//we divide by max-min so that we are normalized in the range [0, 1], this way we are input signal levels agnostic
                    int imag = ((m_q1s[i] * m_freqs[i].sin) / scalingFactor)/span;

                    *(magnitudes[i]) = real * real + imag * imag;
                }

                magnitudesComputed = GR_READY;
            }
            reset();
        }
    }

    return magnitudesComputed;
}
