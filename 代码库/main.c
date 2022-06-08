#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "delay.h"
#include "usart.h"


uint8_t ScreenBuf[72]; //屏幕缓存
#define N 18
#define LINE(x) ScreenBuf + x *N
#define flush() lcd12864_DDRAM_Flush(ScreenBuf, N)


void main(void)
{
    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO

    /*=============初始化=============*/
    Uart_Init();
    printf("hello\r\n");
   
    while (1)
    {
       
        
    }
}
