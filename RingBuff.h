/*
 * Copyright (C) 2017 Wojciech Krutnik N0CALL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * FIFO ring buffer
 * source:
 * http://stackoverflow.com/questions/6822548/correct-way-of-implementing-a-uart-receive-buffer-in-a-small-arm-microcontroller
 * (modified)
 *
 */
 
#if !defined(RINGBUFF_H)
#define RINGBUFF_H

#define RINGBUFF_SIZE(ringBuff) (ringBuff.size)    /* serial buffer in bytes (power 2)   */
#define RINGBUFF_MASK(ringBuff) (ringBuff.size-1U) /* buffer size mask                   */

/* Buffer read / write macros                                                   */
#define RINGBUFF_RESET(ringBuff)         (ringBuff).rdIdx = ringBuff.wrIdx = 0
#define RINGBUFF_WRITE(ringBuff, dataIn) (ringBuff).data[RINGBUFF_MASK(ringBuff) & ringBuff.wrIdx++] = (dataIn)
#define RINGBUFF_READ(ringBuff)          ((ringBuff).data[RINGBUFF_MASK(ringBuff) & ((ringBuff).rdIdx++)])
#define RINGBUFF_EMPTY(ringBuff)         ((ringBuff).rdIdx == (ringBuff).wrIdx)
#define RINGBUFF_FULL(ringBuff)          ((RINGBUFF_MASK(ringBuff) & ringBuff.rdIdx) == (RINGBUFF_MASK(ringBuff) & ringBuff.wrIdx))
#define RINGBUFF_COUNT(ringBuff)         (RINGBUFF_MASK(ringBuff) & ((ringBuff).wrIdx - (ringBuff).rdIdx))

/* Buffer type                                                                  */
#define DECLARE_RINGBUFFER_TYPE(name, _size)                           \
          typedef struct{                                              \
            uint32_t size;                                             \
            uint32_t wrIdx;                                            \
            uint32_t rdIdx;                                            \
            uint8_t data[_size];                                       \
          }name##_t

#endif
