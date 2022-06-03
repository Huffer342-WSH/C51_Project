#include "main.h"

#include "stdio.h"
#include "stdlib.h"

#include "delay.h"
#include "eeprom.h"
#include "usart.h"

typedef struct
{
    int32 len;
    float array[50];
} List_t;

void main(void)
{
    uint16 i, len;
    List_t list, receive;
    list.len = 10;
    list.array[0] = 12.65;
    list.array[2] = 3.1415926;
    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO

    Uart_Init();
    printf("This is list program about eeprom\r\n");
    len = sizeof(list);
    IapEraseSector(SECTOR0_ADDRESS);
    eeprom_WriteData(SECTOR0_ADDRESS, &list, len);
    eeprom_ReadData(SECTOR0_ADDRESS, &receive, len);

    for (i = 0; i < 10; i++)
    {
        printf("%.3F\t%.3f\r\n", list.array[i], receive.array[i]);
    }

    while (1)
    {
    }
}
