#include "speedometer.h"
#include "delay.h"
#include "string.h"

uint16  SpdOt_count[SpdOt_MAX_LEN];
uint16  SpdOt_temp;

static uint8 SpdOt_index;

 void SpdOt_SaveTime()
{

    if (++SpdOt_index >= SpdOt_MAX_LEN)
    {
        SpdOt_index = 0;
    }
    if (SpdOt_temp)
    {
        SpdOt_count[SpdOt_index] = SpdOt_temp;
        SpdOt_temp = 0;
    }
}

void SpdOt_Init() // 10ms
{
    memset(SpdOt_count, 0xff, sizeof(SpdOt_count));
    SpdOt_temp = 0;

    IT0 = 1; // set INT0 int type (1:Falling 0:Low level)
    EX0 = 1; // enable INT0 interrupt

    AUXR |= 0x40;    //定时器时钟1T模式
    TMOD &= 0x0F;    //设置定时器模式
    TL1 = T1MS;      //设置定时初始值
    TH1 = T1MS >> 8; //设置定时初始值
    TF1 = 0;         //清除TF1标志
    TR1 = 1;         //定时器1开始计时

    ET1 = 1;
    EA = 1; //开启总中断
}

/* Timer0 interrupt routine */
void tm1_isr() interrupt 3
{
    TL1 = T1MS;      // reload timer1 low byte
    TH1 = T1MS >> 8; // reload timer1 high byte

    SpdOt_temp++;

    if (SpdOt_temp > 10000)
    {
        if (++SpdOt_index >= SpdOt_MAX_LEN)
        {
            SpdOt_index = 0;
        }
        if (SpdOt_temp)
        {
            SpdOt_count[SpdOt_index] = SpdOt_temp;
            SpdOt_temp = 0;
        }
    }
}

// External interrupt0 service routine
void exint0() interrupt 0 //(location at 0003H)
{

    if (++SpdOt_index >= SpdOt_MAX_LEN)
    {
        SpdOt_index = 0;
    }
    if (SpdOt_temp)
    {
        SpdOt_count[SpdOt_index] = SpdOt_temp;
        SpdOt_temp = 0;
    }
}

uint16 SpdOt_GetTime()
{
    data uint8 i;
    uint32 sum = 0;
    for (i = 0; i < SpdOt_MAX_LEN; i++)
    {
        sum += SpdOt_count[i];
    }
    return sum / SpdOt_MAX_LEN;
}
