/* Arduino BufferedWriter Library
 * Copyright (C) 2011 by William Greiman
 *
 * This file is part of the Arduino BufferedWriter Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino BufferedWriter Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <BufferedWriter.h>

//------------------------------------------------------------------------------
/**
 * write HEX
 * \param[in] n number to be written as HEX
 */
void BufferedWriter::putHex(uint32_t n) {
  uint8_t len;
  if (n < 0X10000) {
    len = n < 0X10 ? 1 : n < 0X100 ? 2 : n < 0X1000 ? 3 : 4;
  } else {
    len = n < 0X100000 ? 5 : n < 0X1000000 ? 6 : n < 0X10000000 ? 7 : 8;
  }
  if ((in_ + len) >= sizeof(buf_)) {
    writeBuf();
  }
  in_ += len;
  char* str = buf_ + in_ - 1;
  do {
    uint8_t h = n & 0XF;
    *str-- = h + (h < 10 ? '0' : 'A' - 10);
    n >>= 4;
  } while (n);
}
//------------------------------------------------------------------------------
/** Write a signed 8-bit integer
 * \param[in] n number to be written
 */
void BufferedWriter::putNum(int8_t n) {
  if (n < 0) {
    putChar('-');
    n = -n;
  }
  putNum((uint8_t)n);
}
//------------------------------------------------------------------------------
/** Write an unsigned 8-bit number
 * \param[in] n number to be written
 */
void BufferedWriter::putNum(uint8_t n) {
  if (n > 99) putChar('0' + n / 100);
  if (n > 9) putChar('0' + (n / 10) % 10);
  putChar('0' + n % 10);
}
//------------------------------------------------------------------------------
/** Write a signed 16-bit integer
 * \param[in] n number to be written
 */
void BufferedWriter::putNum(int16_t n) {
  if (n < 0) {
    putChar('-');
    n = -n;
  }
  putNum((uint16_t)n);
}
//------------------------------------------------------------------------------
/** Write a unsigned 16-bit integer
 * \param[in] n number to be written
 */
void BufferedWriter::putNum(uint16_t n) {
  if (n < 256) {
    putNum((uint8_t)n);
    return;
  }
  uint8_t len = n < 1000 ? 3 : n < 10000 ? 4 : 5;
  if ((in_ + len) >= sizeof(buf_)) {
    writeBuf();
  }
  in_ += len;
  char* str = buf_ + in_ - 1;
  do {
    // avoid mod, it's slow.
    uint16_t tmp = n;
    n /= 10;
    *str-- = '0' + tmp - 10 * n;
  } while (n);
}
//------------------------------------------------------------------------------
/** Write a signed 32-bit integer
 * \param[in] n number to be written
 */
void BufferedWriter::putNum(int32_t n) {
  if (n < 0) {
    putChar('-');
    n = -n;
  }
  putNum((uint32_t)n);
}
//------------------------------------------------------------------------------
/** Write an unsigned 32-bit number
 * \param[in] n number to be written
 */
void BufferedWriter::putNum(uint32_t n) {
  uint8_t len;
  if (n < 0X10000) {
    putNum((uint16_t)n);
    return;
  }
  if (n < 10000000) {
    len = n < 100000 ? 5 : n < 1000000 ? 6 : 7;
  } else {
    len = n < 100000000 ? 8 : n < 1000000000 ? 9 : 10;
  }
  if ((in_ + len) >= sizeof(buf_)) {
    writeBuf();
  }
  in_ += len;
  char* str = buf_ + in_ - 1;
  do {
    // avoid mod, it's slow.
    uint32_t tmp = n;
    n /= 10;
    *str-- = '0' + tmp - 10 * n;
  } while (n);
}
//------------------------------------------------------------------------------
/** Write a string
 * \param[in] str string to be written
 */
void BufferedWriter::putStr(char* str) {
  while (*str) putChar(*str++);
}
//------------------------------------------------------------------------------
void BufferedWriter::putStr_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) putChar(c);
}
//------------------------------------------------------------------------------
/** force write of buffer */
void BufferedWriter::writeBuf() {
  if (in_) {
    file_->write(buf_, in_);
    in_ = 0;
  }
}