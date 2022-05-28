#include "usart.h"

#if USE_printf

char putchar(char c)
{
    SendData(c);
    return (c);
}
#endif

static bit busy;

/*----------------------------
UART initialization
----------------------------*/

void Uart_Init(void) // 9600bps@11.0592MHz
{
    PCON &= 0x7F; //�����ʲ�����
    SCON = 0x50;  // 8λ����,�ɱ䲨����
    AUXR |= 0x04; //��ʱ��ʱ��1Tģʽ
    BRT = 0xDC;   //���ö�ʱ����ֵ
    AUXR |= 0x01; //����1ʹ�ö��������ʷ�����Ϊ�����ʷ�����
    AUXR |= 0x10; //�������������ʷ�����

    TI = 1;
    EA = 1; //�����ж�
    ES = 1; //�������ж�
}

/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4
{
    if (RI)
    {
        RI = 0; // Clear receive interrupt flag
    }
    if (TI)
    {
        TI = 0;   // Clear transmit interrupt flag
        busy = 0; // Clear transmit busy flag
    }
}

#if USE_SendData
/*----------------------------
Send a byte data to UART
Input: dat (data to be sent)
Output:None
----------------------------*/
void SendData(uint8 dat)
{
    while (busy)
        ;      // Wait for the completion of the previous data is sent
    ACC = dat; // Calculate the even parity bit P (PSW.0)
    busy = 1;
    SBUF = ACC; // Send data to UART buffer
}
#endif

#if USE_SendString
/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void SendString(char *s)
{
    while (*s) // Check the end of the string
    {
        SendData(*s++); // Send current char and increment string ptr
    }
}
#endif
