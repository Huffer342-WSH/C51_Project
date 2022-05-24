
/*********************************************************************************
* 【编写时间】： 2011.07.07
* 【作    者】： 雁翎电子
* 【版    本】： V1.0
* 【网    站】： http://ylelectronic.taobao.com/
* 【Q      Q】： 348439350
* 【声    明】： 此程序仅用于学习与参考，引用请注明版权和作者信息！
* 【函数功能】： DA-AD试验
* 【使用说明】： 单片机：STC89C52
                 串口波特率：9600
                 串口一共发送4个字节
                 1 光敏电阻
                 2 热敏电阻
                 3 悬空
                 4 可调0-5V电压（蓝色电位器调节）

**********************************************************************************/
/********************************************************************************************/

#include <reg52.h> //包含单片机寄存器的头文件
#include <I2C.H>
#include "pcf8591.h"

#define uchar unsigned char
#define PCF8591 0x90 // PCF8591 地址

// 变量定义
unsigned char AD_CHANNEL;
unsigned long xdata LedOut[8];
unsigned int D[32];
/*
sbit P2_0 = P2^0;
sbit P2_1 = P2^1;
sbit P2_2 = P2^2;
sbit P2_3 = P2^3;
*/
unsigned char date;

uchar code table[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};

// 函数申明

void display();
void delay(uchar i);
bit DACconversion(unsigned char sla, unsigned char c, unsigned char Val);
bit ISendByte(unsigned char sla, unsigned char c);
unsigned char IRcvByte(unsigned char sla);
void Initial_com(void);
//******************************************************************/
main()
{

    Initial_com();
    while (1)
    {

        /********以下AD-DA处理*************/

        DA_Res(&D[0]);

        /********以下将AD的值通过串口发送出去*************/
        delay(200);
        SBUF = D[0];
        //        delay(200);
        //        SBUF = D[1];
        //        delay(200);
        //        SBUF = D[2];
        //        delay(200);
        //        SBUF = D[3];
        delay(200);
        if (RI)
        {
            date = SBUF; //单片机接受
            SBUF = date; //单片机发送
            RI = 0;
        }
    }
}

void delay(uchar i)
{
    uchar j, k;
    for (j = i; j > 0; j--)
        for (k = 125; k > 0; k--)
            ;
}

//*****串口初始化函数***********

//******************************
void Initial_com(void)
{
    EA = 1;      //开总中断
    ES = 1;      //允许串口中断
    ET1 = 1;     //允许定时器T1的中断
    TMOD = 0x20; //定时器T1，在方式2中断产生波特率
    PCON = 0x00; // SMOD=0
    SCON = 0x50; // 方式1 由定时器控制
    TH1 = 0xfd;  //波特率设置为9600
    TL1 = 0xfd;
    TR1 = 1; //开定时器T1运行控制位
}
