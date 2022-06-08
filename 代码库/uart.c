#include "uart.h"
#include "delay.h"
#include "string.h"

#if USE_printf

char putchar(char c)
{
    SendData(c);
    return (c);
}
#endif
uint8 Uart1_RxBuf[MAX_BUF];
uint8 Rx1len = 0;
static bit busy;

/*----------------------------
UART initialization
----------------------------*/

void Uart1_Init(void) // 115200bps@11.0592MHz
{
    PCON &= 0x7F; //波特率不倍速
    SCON = 0x52;  // 8位数据,可变波特率
    AUXR |= 0x04; //定时器时钟1T模式
    BRT = 0xFD;   //设置定时重载值
    AUXR |= 0x01; //串口1使用独立波特率发射器为波特率发生器
    AUXR |= 0x10; //启动独立波特率发射器

    TI = 1;
    EA = 1; //开总中断
    ES = 1; //开串口中断
}

/*----------------------------
UART interrupt service routine
----------------------------*/
static void Uart_Isr() interrupt 4
{
    if (RI)
    {
        RI = 0; // Clear receive interrupt flag
        if (Rx1len < MAX_BUF)
        {
            Uart1_RxBuf[Rx1len++] = SBUF;
        }
    }
    if (TI)
    {
        TI = 0;   // Clear transmit interrupt flag
        busy = 0; // Clear transmit busy flag
    }
}

#if USE_SendData

void SendData(uint8 dat)
{
    while (busy)
        ; // Wait for the completion of the previous data is sent
    busy = 1;
    SBUF = dat; // Send data to UART buffer
}
#endif

#if USE_SendString
void SendString(char *s)
{
    while (*s) // Check the end of the string
    {
        SendData(*s++); // Send current char and increment string ptr
    }
}
#endif

void Uart1_Receive(uint8 *pData, uint8 *pLen, uint16 Timeout)
{
    uint8 temp = 0;
    //超时等待
    while (!Rx1len)
    {
        --Timeout;

        if (!Timeout) //超时,退出
        {
            *pLen = 0;
            return;
        }
    }
    while (temp != Rx1len) //等待串口空闲
    {
        temp = Rx1len;
        delayms(2);
    }
    memcpy(pData, Uart1_RxBuf, Rx1len);
    *pLen = Rx1len;
    pData[Rx1len] = '\0';
    Rx1len = 0;
}