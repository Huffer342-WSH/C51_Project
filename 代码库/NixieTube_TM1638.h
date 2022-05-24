#ifndef _TM1638_H
#define _TM1638_H
#include "main.h"

#ifndef __MAIN_H_
sbit DIO = P1 ^ 0;
sbit CLK = P1 ^ 1;
sbit STB = P1 ^ 2;
#endif

#define DATA_COMMAND 0X40
#define DISP_COMMAND 0x80
#define ADDR_COMMAND 0XC0

extern unsigned char code SEG_Cathode[16];

void init_TM1638(void);
void Write_DATA(unsigned char add, unsigned char DATA);  //指定地址写入数据
unsigned char Read_key(void);
void Write_allLED(
    unsigned char LED_flag);  //控制全部LED函数，LED_flag表示各个LED状态

#endif