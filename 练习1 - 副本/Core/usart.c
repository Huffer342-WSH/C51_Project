#include "STC12C5A60S2.h"
#include "intrins.h"
#include "usart.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

sbit bit9 = P2 ^ 2;  // P2.2 show UART data bit9
bit busy;

/*----------------------------
UART initialization
----------------------------*/

void Uart_Init(void)  // 115200bps@11.0592MHz
{
  PCON &= 0x7F;  //波特率不倍速
  SCON = 0x50;   // 8位数据,可变波特率
  AUXR &= 0xFB;  //定时器时钟12T模式
  BRT = 0xFD;    //设置定时重载值
  AUXR |= 0x01;  //串口1使用独立波特率发射器为波特率发生器
  AUXR |= 0x10;  //启动独立波特率发射器

  EA = 1;  //开总中断
  ES = 1;  //开串口中断
}

/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4 {
  if (RI) {
    RI = 0;  // Clear receive interrupt flag
    // P0 = SBUF;          //P0 show UART data
    bit9 = RB8;  // P2.2 show parity bit
  }
  if (TI) {
    TI = 0;    // Clear transmit interrupt flag
    busy = 0;  // Clear transmit busy flag
  }
}

/*----------------------------
Send a byte data to UART
Input: dat (data to be sent)
Output:None
----------------------------*/
void SendData(BYTE dat) {
  while (busy)
    ;         // Wait for the completion of the previous data is sent
  ACC = dat;  // Calculate the even parity bit P (PSW.0)
  busy = 1;
  SBUF = ACC;  // Send data to UART buffer
}

/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void SendString(char *s) {
  while (*s)  // Check the end of the string
  {
    SendData(*s++);  // Send current char and increment string ptr
  }
}
