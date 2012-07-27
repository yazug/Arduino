/*
 *  DS18B20.h
 *  
 *
 *  Created by ‰º‰’ ^ˆê on 10/06/06.
 *  Copyright 2010 ‹ãBH‹Æ‘åŠwî•ñHŠw•”. All rights reserved.
 *
 */

#ifndef DS18B20_h
#define DS18B20_h

#define CONVERT_T 0x44
#define WRITE_SCRATCHPAD 0x4E
#define READ_SCRATCHPAD 0xBE

inline float Celsius(int val) { return (float)0.0625*val; }

#endif //  DS18B20_h