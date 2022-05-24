#include "KeyBoard.h"
#include "Delay.h"
#include "main.h"

#define delayms Delayms
unsigned char KeyScan(void)
{
    unsigned char KeyValue = 255;
    char a = 0;
    if (GPIO_KEY != 0x0f)
    {               //读取按键是否按下
        delayms(5); //延时5ms进行消抖
        if (GPIO_KEY != 0x0f)
        { //再次检测键盘是否按下
            //测试列
            GPIO_KEY = 0X0F;
            switch (GPIO_KEY)
            {
            case (0X07):
                KeyValue = 0;
                break;
            case (0X0b):
                KeyValue = 1;
                break;
            case (0X0d):
                KeyValue = 2;
                break;
            case (0X0e):
                KeyValue = 3;
                break;
            }
            //测试行
            GPIO_KEY = 0XF0;
            switch (GPIO_KEY)
            {
            case (0X70):
                KeyValue = KeyValue;
                break;
            case (0Xb0):
                KeyValue = KeyValue + 4;
                break;
            case (0Xd0):
                KeyValue = KeyValue + 8;
                break;
            case (0Xe0):
                KeyValue = KeyValue + 12;
                break;
            }
            delayms(10);
        }
        else
        {
            return 0;
        }
    }
    return KeyValue + 1;
}