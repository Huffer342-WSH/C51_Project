#include "main.h"

#include "stdio.h"
#include "stdlib.h"

#include "usart.h"
#include "delay.h"

#include "ds1302.h"
#include "lcd12864_s.h"
#include "pcf8591.h"
#include "hcsr04.h"
#include "pwm.h"
#include "matkey.h"
#include "speedometer.h"

uint8_t xdata ScreenBuf[69] = "0123456789abcdefg0123456789abcdefg0123456789abcdefg0123456789abcdefg";
void main(void)
{
    uint8 adValue, keyValue;
    uint16 dist;
    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO

    Delayms(100);
    Uart_Init();
    Delayms(100);
    lcd12864_Init();
    Delayms(100);
    ds1302_init();
    //  Delayms(100);
    // ds1302_write_time();
    Delayms(100);
    HCSR04_Init();
    PWM_Init();
    SpdOt_Init();
    SpdOt_Start();

    /*开屏动画*/

    lcd12864_HCDisp2(ScreenBuf + 16, "0123456789", "9876543210", 10, -10, 3, 16, 3, 10);
    delayms(200);
    lcd12864_HCDisp2(ScreenBuf + 16, "0123456789", "9876543210", 10, 3, 16, 3, -10, 10);
    KeyBoard_Port = 0x0F;

    while (1)
    {
        /*状态机*/
        keyValue = KeyScan();
        if (state == STATE_AUTO)
        {
            if (keyValue == 15)
            {
                state = STTATE_SETTIME;
            }
            else if (keyValue == 16)
            {
                state = STATE_SETSEPPED;
            }
        }

        /*执行对应状态操作*/
        if (state == STATE_AUTO)
        {
            lcd12864_DDRAM_Flush(ScreenBuf);
            /*AD*/
            DA_Res(adValue);
            PWM_SetDutyH0(adValue);
            sprintf(ScreenBuf, "AUTO PWM:[%3bu]", adValue);
            /*ds1302*/
            ds1302_read_time();
            sprintf(ScreenBuf + 17, "%2x.%02x:%02x:%02x    ", (uint16)ds1302_buf.name.day, (uint16)ds1302_buf.name.hour,
                    (uint16)ds1302_buf.name.minute, (uint16)ds1302_buf.name.second);
            /*HCSR04*/
            dist = HCSR04_GetDist();
            sprintf(ScreenBuf + 34, "Dis:[%4u]mm    ", dist);

            /*KeyBoard*/

            if (KeyScan() == 15)
            {
                KeyInput(ScreenBuf + 51, 7, 15, 16, 0, 3);
            }
            keyValue = KeyScan();
            sprintf(ScreenBuf + 58, " T:%5u ", SpdOt_GetTime());

            Delayms(10);
        }
        else if (state == STATE_SETSEPPED)
        {
            KeyInput(ScreenBuf +)
        }
        else if (state == STTATE_SETTIME)
        {
            /* code */
        }
    }
}
