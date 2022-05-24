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

extern uint8 LED[4];

#define STATE_AUTO (uint8)0
#define STATE_SETTIME (uint8)1
#define STATE_SETSEPPED (uint8)2
#define STATE_MANUAL (uint8)3
#define GLOBAL_IT_OPEN() EA = 1
#define GLOBAL_IT_CLOSE() EA = 0

sbit DS1302_RST = P3 ^ 5;  //时钟芯片
sbit DS1302_DAT = P3 ^ 6;
sbit DS1302_CLK = P3 ^ 7;

sbit SCL = P4 ^ 4;  // I2C  时钟
sbit SDA = P4 ^ 5;  // I2C  数据

sbit Echo = P1 ^ 5;  //超声波echo
sbit Trig = P1 ^ 6;  //超声波trig

sbit LCD_SID = P4 ^ 0;   //串行数据
sbit LCD_SCLK = P4 ^ 1;  //串行同步时钟

sbit HC595_DIO = P1 ^ 0;   //串行数据输入
sbit HC595_RCLK = P1 ^ 1;  //时钟脉冲信号――上升沿有效
sbit HC595_SCLK = P1 ^ 2;  //打入信号――――上升沿有效

sbit DIO = P2 ^ 0;
sbit CLK = P2 ^ 1;
sbit STB = P2 ^ 2;

sbit HW = P1 ^ 7;

#define KeyBoard_Port P0

#define USESTC12C5A60S2_H_

#endif