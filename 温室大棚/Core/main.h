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

// P30
#define LCD_SID P36  // LCD12864′?DDSID
#define LCD_SCLK P34 // LCD12864′?DDSCLK
#define LCD_PSB P37

sbit DHT11_DIO = P2 ^ 0; // dht11data端接单片机的P1^0口//

sbit BH1750_SDA = P2 ^ 1; // IIC数据引脚定义
sbit BH1750_SCL = P2 ^ 2; // IIC时钟引脚定义

sbit RC522_NSS = P2 ^ 3; //片选
sbit RC522_SCK = P2 ^ 4;
sbit RC522_SI = P2 ^ 5;
sbit RC522_SO = P2 ^ 6;
sbit RC522_RST = P2 ^ 7;

#define KeyBoard_Port P0

#define ST_DEFAULT 0
#define ST_LOGIN 1
#define ST_MENU 2
#define ST_MENU_G 3
#define ST_MANAGE 4
#define ST_DATA 5
#define ST_NEW 6
#define ST_LIGHT 7
#define ST_TEMP 8
#define ST_HUMI 9

#define R_ADMIN 2
#define R_GENERIC 1
#define R_NONE 0
#endif