#ifndef __12864DISPLAY_H_
#define __12864DISPLAY_H_

#include "delay.h"
#include "main.h"

//串并行引脚命名:RS = CS  RW = SID_lcd  E = SCLK_lcd

/*****LCD接口定义*****/
#define LCD12864_SID LCD_SID
#define LCD12864_SCLK LCD_SCLK

#define USEPSB 0 // PSB接地就不需要该引脚了
#define USECS 0

#define USE_MACRO 1
#define USE_lcd12864_DDRAM 1
#define USE_lcd12864_DDRAM_Flush 1
#define USE_lcd12864_GDRAM 0
#define USE_lcd12864_HCDisp 0
#define USE_lcd12864_HCDisp2 1
extern uint8_t code DDRAM_POS[8];

#if USEPSB

sbit PSB = P2 ^ 3; //串并口选择 1:并口模式; 0:串口模式;
#endif
#if USECS
sbit CS = P4 ^ 0; //片选信号
#endif

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

#define LCDOPT_CMD 0
#define LCDOPT_DATA 1
/*对外函数与宏函数*/
void lcd12864_Init();     //初始化
void lcd12864_Wait(void); //等待lcd12864退出busy状态
void lcd12864_WriteOpt(uint8 option,
                       uint8 byte); //发送信息 指令：opt=0 数据：opt=1

#if USE_MACRO
#define lcd12864_WriteCmd(byte) lcd12864_WriteOpt(LCDOPT_CMD, byte)   //写指令
#define lcd12864_WriteData(byte) lcd12864_WriteOpt(LCDOPT_DATA, byte) //写数据
#else
void lcd12864_WriteCmd(uint8_t Cbyte);  //写指令
void lcd12864_WriteData(uint8_t Dbyte); //写数据
#endif

/*常用指令*/
#define lcd12864_SetAC(x, y) lcd12864_WriteCmd(DDRAM_POS[y] + x)
#define lcd12864_CursorOn() lcd12864_WriteCmd(0x0f)
#define lcd12864_CursorOff() lcd12864_WriteCmd(0x0c)
#define lcd12864_CursorACC() lcd12864_WriteCmd(CURSE_DIR)

/*屏幕显示*/
void lcd12864_DDRAM(uint8_t m, uint8_t n, uint8_t *s);
void lcd12864_DDRAM_plus(uint8_t m, uint8_t n, uint8_t *s, uint8 len);
void lcd12864_DDRAM_Flush(uint8_t *str, uint8_t lineLength); //刷写整个屏幕
void lcd12864_GDRAM(uint8_t *pic);
void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len, char pos_begin1, char pos_end1,
                      char pos_begin2, char pos_end2,
                      unsigned int ms); //横向滚动显示一段字符
#endif