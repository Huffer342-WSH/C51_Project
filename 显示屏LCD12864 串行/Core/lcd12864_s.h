#ifndef __12864DISPLAY_H_
#define __12864DISPLAY_H_

#include "STC12C5A60S2.h"
#include "intrins.h"
#include "delay.h"

#define uint8_t unsigned char
#define uint16_t unsigned int //宏定义
#define delayms(x) Delayms(x)

#define USEPSB 0 // PSB接地就不需要该引脚了
#define USECS 0
/*****LCD功能初始化指令*****/
#define CLEAR_SCREEN 0x01 //清屏指令：清屏且AC值为00H
#define AC_INIT 0x02      //将AC设置为00H。且游标移到原点位置
#define CURSE_ADD 0x06 //设定游标移到方向及图像整体移动方向（默认游标右移，图像整体不动）
#define FUN_MODE 0x30  //工作模式：8位基本指令集
#define DISPLAY_ON 0x0c  //显示开,显示游标，且游标位置反白
#define DISPLAY_OFF 0x08 //显示关
#define CURSE_DIR 0x14   //游标向右移动:AC=AC+1
#define SET_CG_AC 0x40   //设置AC，范围为：00H~3FH
#define SET_DD_AC 0x80

void lcd12864_Init(); //初始化 //初始化
void lcd12864_Wait(void);
void lcd12864_WriteCmd(uint8_t Cbyte);
void lcd12864_WriteData(uint8_t Dbyte);
void lcd12864_DDRAM(uint8_t x, uint8_t y, uint8_t *c);
void lcd12864_DDRAM_Flush(uint8_t xdata *c);//仅仅适用于Xdata区的字符串，64B缓存对于128B的data消耗资源太多
void Lcd_Fill(uint8_t test);
void LcdFill_Level();
void LcdFill_Vertical();
void pic_Disp(uint8_t *pic);
#endif