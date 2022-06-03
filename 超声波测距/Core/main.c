#include "main.h"

#include "stdio.h"
#include "stdlib.h"

#include "delay.h"
#include "eeprom.h"
#include "usart.h"

#include "hcsr04.h"

void main(void)
{

    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO

    Uart_Init();
    HCSR04_Init();

    printf("This is a program about HC-SR04\r\n");

    while (1)
    {
        printf("%hu\r\n", HCSR04_GetDist());
        delayms(500);
    }
}
