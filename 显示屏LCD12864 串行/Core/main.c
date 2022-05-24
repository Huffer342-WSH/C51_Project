/**
 * @file main.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief LCD12864_ST7920���в��Գ���
 * @date 2022-04-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "main.h"
#include "lcd12864_s.h"
#include "delay.h"

uint8_t xdata ScreenBuf[] = "0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrs";
/*****������*****/
void main(void)
{
    Delayms(500);
    lcd12864_Init();
    lcd12864_DDRAM(0, 0, "2019217123 ");
    lcd12864_DDRAM(1, 1, "���в��Գ���");
    lcd12864_DDRAM(0, 2, "���в��Գ��������");
    lcd12864_DDRAM(2, 3, "���в��Գ���");
    Delayms(1000);
    while (1)
    {
        lcd12864_DDRAM_Flush(ScreenBuf);
        Delayms(1000);
    }
}
