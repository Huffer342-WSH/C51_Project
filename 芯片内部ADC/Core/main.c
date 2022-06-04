#include "main.h"

#include "stdio.h"
#include "stdlib.h"

#include "adc.h"
#include "delay.h"
#include "usart.h"
void main(void)
{

    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO
    Uart_Init();
    InitADC();
    printf("This is a program about internal ADC");
    while (1)
    {
        printf("%4hu \r\n", GetADCResult(0));
        delayms(500);
    }
}
