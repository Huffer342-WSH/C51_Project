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
  PCON &= 0x7F;  //�����ʲ�����
  SCON = 0x50;   // 8λ����,�ɱ䲨����
  AUXR &= 0xFB;  //��ʱ��ʱ��12Tģʽ
  BRT = 0xFD;    //���ö�ʱ����ֵ
  AUXR |= 0x01;  //����1ʹ�ö��������ʷ�����Ϊ�����ʷ�����
  AUXR |= 0x10;  //�������������ʷ�����

  EA = 1;  //�����ж�
  ES = 1;  //�������ж�
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
