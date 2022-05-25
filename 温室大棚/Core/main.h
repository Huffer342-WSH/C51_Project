#ifndef __MAIN_H_
#define __MAIN_H_

#include "STC12C5A60S2.h"
#include "intrins.h"

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long
#define uint8_t unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long
#define int8 char
#define int16 int
#define int32 long
#define int8_t char
#define int16_t int
#define int32_t long
#define float32 float

#define GLOBAL_IT_OPEN() EA = 1
#define GLOBAL_IT_CLOSE() EA = 0

sbit LCD_SID = P4 ^ 0;  // LCD12864串行SID
sbit LCD_SCLK = P4 ^ 1; // LCD12864串行SCLK

sbit DHT11_DIO = P1 ^ 0; // dht11data端接单片机的P1^0口//

#endif