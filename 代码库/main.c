#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "delay.h"
#include "usart.h"


uint8_t ScreenBuf[72]; //��Ļ����
#define N 18
#define LINE(x) ScreenBuf + x *N
#define flush() lcd12864_DDRAM_Flush(ScreenBuf, N)


void main(void)
{
    /*-------------------��ʼ��---------------------------------*/
    P4SW = 0xff;              // P4�˿ھ���IO��P4.7���⣩
    P0M0 = 0x00, P0M1 = 0x00; // P0©��������˫��IO

    /*=============��ʼ��=============*/
    Uart_Init();
    printf("hello\r\n");
   
    while (1)
    {
       
        
    }
}
