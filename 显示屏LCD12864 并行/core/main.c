#include <STC/STC12C5A60S2.H>
#include "lcd12864.h"
#include "Delay.h"
#include "stdio.h"
#include "string.h"
void main()
{
    int i;
    unsigned char buf[] = "0123456789abcdef";

    while (1)
    {

        lcd12864_Init();
        Delayms(100);
        lcd12864_DDRAM(0, 0, buf);
        lcd12864_DDRAM(0, 1, "×ªËÙ£º");
        lcd12864_DDRAM(0, 4, "¹þ¹þ¹þ");
        sprintf(buf,"1%4f*",0.12);
        lcd12864_DDRAM(0, 3,buf);
        while(1);
        lcd12864_HCDisp(buf, "0123456789", 10, 0, 17, 100);
        lcd12864_HCDisp2(buf, "2019123456", "2019654321", 10, -10, 3, 16, 3, 300);
        for (i = 0; i < 20; i++)
        {
            lcd12864_TransferData(0x34, 0);
            lcd12864_TransferData(0x03, 0);
            lcd12864_TransferData(0x40 + i, 0);
            Delayms(500);
            lcd12864_TransferData(0x30, 0);
            lcd12864_TransferData(0x05, 0);
        }

        Delayms(2000);
    }
}