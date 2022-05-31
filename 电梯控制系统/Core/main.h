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

//引脚定义
#define LCD_SID P36  // LCD12864串行SID
#define LCD_SCLK P34 // LCD12864串行SCLK
#define LCD_PSB P37

#define IN1 P20
#define IN2 P21
#define IN3 P22
#define IN4 P23

#define BEEP P11
#define LED P12
#define RELAY P10

#define KeyBoard_Port P0

#define ST_IDLE 0
#define ST_UP 1
#define ST_DOWN 2
#define ST_WAIT 3
#endif