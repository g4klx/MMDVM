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

#if !defined(CTCSSDECODER_H)
#define  CTCSSDECODER_H

#include "Globals.h"
#include "Goertzel.h"
#include "HannWindow.h"

#define N_SAMPLES 12000

typedef struct
{
    TGoertzelParameters toneLow, tone, toneHi;
} TCTCSSTone;


/* 
 * Those are precalculated values (sin, cos, coeff) for ech tone, tone - 2Hz, tone + 2Hz.
 * Those are Q15 values but stored in int. This to avoid unnecessary cast during Goertzel calculation.
 * We need in since intermediate values in goertzel will overflow Q15
 */
const TCTCSSTone TONE_067_0 {{558, 32763, 65527}, {575, 32763, 65526}, {592, 32763, 65525}};
const TCTCSSTone TONE_069_3 {{577, 32763, 65526}, {594, 32763, 65525}, {612, 32762, 65525}};
const TCTCSSTone TONE_071_9 {{600, 32763, 65525}, {617, 32762, 65524}, {634, 32762, 65524}};
const TCTCSSTone TONE_074_4 {{621, 32762, 65524}, {638, 32762, 65524}, {655, 32761, 65523}};
const TCTCSSTone TONE_077_0 {{643, 32762, 65523}, {661, 32761, 65523}, {678, 32761, 65522}};
const TCTCSSTone TONE_079_7 {{667, 32761, 65522}, {684, 32761, 65522}, {701, 32761, 65521}};
const TCTCSSTone TONE_082_5 {{691, 32761, 65521}, {708, 32760, 65521}, {725, 32760, 65520}};
const TCTCSSTone TONE_085_4 {{715, 32760, 65520}, {733, 32760, 65520}, {750, 32759, 65519}};
const TCTCSSTone TONE_088_5 {{742, 32760, 65519}, {759, 32759, 65518}, {776, 32759, 65518}};
const TCTCSSTone TONE_091_5 {{768, 32759, 65518}, {785, 32759, 65517}, {802, 32758, 65516}};
const TCTCSSTone TONE_094_8 {{796, 32758, 65517}, {813, 32758, 65516}, {830, 32757, 65515}};
const TCTCSSTone TONE_097_4 {{818, 32758, 65516}, {835, 32757, 65515}, {853, 32757, 65514}};
const TCTCSSTone TONE_100_0 {{841, 32757, 65514}, {858, 32757, 65514}, {875, 32756, 65513}};
const TCTCSSTone TONE_103_5 {{871, 32756, 65513}, {888, 32756, 65512}, {905, 32756, 65511}};
const TCTCSSTone TONE_107_2 {{902, 32756, 65511}, {920, 32755, 65510}, {937, 32755, 65509}};
const TCTCSSTone TONE_110_9 {{934, 32755, 65509}, {951, 32754, 65508}, {968, 32754, 65507}};
const TCTCSSTone TONE_114_8 {{968, 32754, 65507}, {985, 32753, 65506}, {1002, 32753, 65505}};
const TCTCSSTone TONE_123_0 {{1038, 32752, 65503}, {1055, 32751, 65502}, {1072, 32750, 65501}};
const TCTCSSTone TONE_127_3 {{1075, 32750, 65501}, {1092, 32750, 65500}, {1109, 32749, 65498}};
const TCTCSSTone TONE_131_8 {{1113, 32749, 65498}, {1130, 32748, 65497}, {1148, 32748, 65496}};
const TCTCSSTone TONE_136_5 {{1154, 32748, 65495}, {1171, 32747, 65494}, {1188, 32746, 65493}};
const TCTCSSTone TONE_141_3 {{1195, 32746, 65492}, {1212, 32746, 65491}, {1229, 32745, 65490}};
const TCTCSSTone TONE_146_2 {{1237, 32745, 65489}, {1254, 32744, 65488}, {1271, 32743, 65487}};
const TCTCSSTone TONE_150_0 {{1269, 32743, 65487}, {1286, 32743, 65485}, {1304, 32742, 65484}};
const TCTCSSTone TONE_151_4 {{1281, 32743, 65486}, {1298, 32742, 65485}, {1316, 32742, 65483}};
const TCTCSSTone TONE_156_7 {{1327, 32741, 65482}, {1344, 32740, 65481}, {1361, 32740, 65479}};
const TCTCSSTone TONE_159_8 {{1353, 32740, 65480}, {1370, 32739, 65479}, {1388, 32739, 65477}};
const TCTCSSTone TONE_162_2 {{1374, 32739, 65478}, {1391, 32738, 65477}, {1408, 32738, 65475}};
const TCTCSSTone TONE_165_5 {{1402, 32738, 65476}, {1419, 32737, 65474}, {1436, 32736, 65473}};
const TCTCSSTone TONE_167_9 {{1423, 32737, 65474}, {1440, 32736, 65473}, {1457, 32736, 65471}};
const TCTCSSTone TONE_171_3 {{1452, 32736, 65472}, {1469, 32735, 65470}, {1486, 32734, 65469}};
const TCTCSSTone TONE_173_8 {{1473, 32735, 65470}, {1490, 32734, 65468}, {1508, 32733, 65467}};
const TCTCSSTone TONE_177_3 {{1503, 32733, 65467}, {1520, 32733, 65465}, {1538, 32732, 65464}};
const TCTCSSTone TONE_179_9 {{1526, 32732, 65465}, {1543, 32732, 65463}, {1560, 32731, 65462}};
const TCTCSSTone TONE_183_5 {{1556, 32731, 65462}, {1574, 32730, 65460}, {1591, 32729, 65459}};
const TCTCSSTone TONE_186_2 {{1580, 32730, 65460}, {1597, 32729, 65458}, {1614, 32728, 65456}};
const TCTCSSTone TONE_188_8 {{1602, 32729, 65458}, {1619, 32728, 65456}, {1636, 32727, 65454}};
const TCTCSSTone TONE_189_9 {{1611, 32728, 65457}, {1628, 32728, 65455}, {1646, 32727, 65453}};
const TCTCSSTone TONE_192_8 {{1636, 32727, 65454}, {1653, 32726, 65453}, {1670, 32725, 65451}};
const TCTCSSTone TONE_196_6 {{1669, 32725, 65451}, {1686, 32725, 65449}, {1703, 32724, 65447}};
const TCTCSSTone TONE_199_5 {{1694, 32724, 65448}, {1711, 32723, 65447}, {1728, 32722, 65445}};
const TCTCSSTone TONE_203_5 {{1728, 32722, 65445}, {1745, 32722, 65443}, {1762, 32721, 65441}};
const TCTCSSTone TONE_206_5 {{1753, 32721, 65442}, {1771, 32720, 65440}, {1788, 32719, 65438}};
const TCTCSSTone TONE_210_7 {{1789, 32719, 65438}, {1807, 32718, 65436}, {1824, 32717, 65434}};
const TCTCSSTone TONE_213_8 {{1816, 32718, 65435}, {1833, 32717, 65433}, {1850, 32716, 65431}};
const TCTCSSTone TONE_218_1 {{1853, 32716, 65431}, {1870, 32715, 65429}, {1887, 32714, 65427}};
const TCTCSSTone TONE_221_3 {{1880, 32714, 65428}, {1897, 32713, 65426}, {1915, 32712, 65424}};
const TCTCSSTone TONE_225_7 {{1918, 32712, 65424}, {1935, 32711, 65422}, {1952, 32710, 65420}};
const TCTCSSTone TONE_229_1 {{1947, 32710, 65420}, {1964, 32709, 65418}, {1981, 32708, 65416}};
const TCTCSSTone TONE_233_6 {{1986, 32708, 65416}, {2003, 32707, 65413}, {2020, 32706, 65411}};
const TCTCSSTone TONE_237_1 {{2016, 32706, 65412}, {2033, 32705, 65410}, {2050, 32704, 65408}};
const TCTCSSTone TONE_241_8 {{2056, 32703, 65407}, {2073, 32702, 65405}, {2090, 32701, 65403}};
const TCTCSSTone TONE_245_5 {{2087, 32701, 65403}, {2105, 32700, 65401}, {2122, 32699, 65398}};
const TCTCSSTone TONE_250_3 {{2129, 32699, 65398}, {2146, 32698, 65395}, {2163, 32697, 65393}};
const TCTCSSTone TONE_254_1 {{2161, 32697, 65393}, {2178, 32696, 65391}, {2195, 32694, 65389}};



class CCTCSSDEcoder {
public:
    //threshold must be 0.0 to 1.0;
    CCTCSSDEcoder(const TCTCSSTone& tone, q15_t threshold);

    void samples(const q15_t* samples, uint8_t length);
    bool hasCTCSS();

private:
    unsigned int m_thresholdSquared;
    bool m_hasCTCSS;
    CGoertzel* m_goertzel;

};

#endif
