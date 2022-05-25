#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "delay.h"
#include "dht11.h"
#include "lcd12864_s.h"
#include "usart.h"

void BootAnime();
void tm0_init();
//屏幕缓存
uint8_t ScreenBuf[72];
uint16 tm0Count; //定时器0中断计数
uint8 humi_real, temp_real;

#define N 18
#define LINE(x) ScreenBuf + x *N
void main(void)
{
    uint8 i, j, k;

    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO
    Uart_Init();
    lcd12864_Init();
    tm0_init();
    printf("hello\r\n");
    BootAnime();
    lcd12864_Init();
    i = 'A';
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    while (1)
    {
        sprintf(LINE(0), "温度:%-3bu湿度:%-3bu", temp_real, humi_real);
        lcd12864_DDRAM_Flush(ScreenBuf, N);
        delayms(200);
    }
}
/*开屏动画*/
void BootAnime()
{
    uint8 i = 0;
    lcd12864_Init();
    delayms(500);
    lcd12864_DDRAM(0, 0, "      ICCS");
    lcd12864_DDRAM(1, 0, "      1085");

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

/* Timer0 interrupt routine */
void tm0_isr() interrupt 1
{
    //计时器重装
    TL0 = 0x00;
    TH0 = 0x4C;
    // 50*40=2000,两秒一次，读取数据
    if (tm0Count-- == 0)
    {
        tm0Count = 40;
        DHT11_Read(&humi_real, &temp_real);
    }
}

/**
 * @brief 定时器0初始化，周期50ms
 *
 */
void tm0_init()
{

    AUXR &= 0x7F; //定时器时钟12T模式
    TMOD &= 0xF0; //设置定时器模式
    TMOD |= 0x01; //设置定时器模式
    TL0 = 0x00;   //设置定时初始值
    TH0 = 0x4C;   //设置定时初始值
    TF0 = 0;      //清除TF0标志
    TR0 = 1;      //定时器0开始计时

    ET0 = 1;      // enable timer0 interrupt
    EA = 1;       // open global interrupt switch
    tm0Count = 0; // initial counter
}
