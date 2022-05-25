#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "delay.h"

#include "lcd12864_s.h"

void BootAnime();
//屏幕缓存
uint8_t ScreenBuf[72];
#define N 18
void main(void)
{

    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO

    memset(ScreenBuf, " ", sizeof(ScreenBuf));
    lcd12864_Init();
    BootAnime();
    while (1)
    {
    }

    /*开屏动画*/
}

void BootAnime()
{
    uint8 i = 0;
    lcd12864_Init();
    delayms(500);
    lcd12864_DDRAM(0, 0, "      ICCS");
    lcd12864_DDRAM(0, 1, "      1085");

    for (i = 0; i < 32; i++)
    {
        lcd12864_WriteCmd(0x34);     //开启拓展指令
        lcd12864_WriteCmd(0x03);     //允许写入卷动地址
        lcd12864_WriteCmd(0x40 + i); //写入卷动地址
        delayms(80);
        // lcd12864_WriteCmd(0x30);
        // lcd12864_WriteCmd(0x05);
    }
}
