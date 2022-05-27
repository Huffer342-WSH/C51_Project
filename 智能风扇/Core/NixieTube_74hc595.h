#ifndef __74HC595_H_
#define __74HC595_H_

#include "main.h"

#ifndef __MAIN_H_
sbit HC595_DIO = P1 ^ 0;   //串行数据输入
sbit HC595_RCLK = P1 ^ 1;  //时钟脉冲信号――上升沿有效
sbit HC595_SCLK = P1 ^ 2;  //打入信号――――上升沿有效
#endif

void LED4_Display(uint8 *c);  // LED显示
void LED4_DisOne(uint8 *c);
#endif