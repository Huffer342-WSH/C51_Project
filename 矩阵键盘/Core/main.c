#include "main.h"
#include "Delay.h"
#include "matkey.h"
#include "usart.h"

void main(void)
{
    uint8 value;
    Uart_Init();

    P0 = 0xF0;
    while (1)
    {
        value = KeyScan();
        printf("P0:%bx value:%bu\r\n", P0, value);
        Delayms(200);
    }
}
