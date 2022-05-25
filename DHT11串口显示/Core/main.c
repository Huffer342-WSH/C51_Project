#include "main.h"
#include "delay.h"
#include "usart.h"

#include "dht11.h"
void main(void)
{
    uint8 i = 0x77, j, k;
    Uart_Init();
    printf("This is a program about the DHT11");
    while (1)
    {
        delayms(1000);
        i = DHT11_Read(&j, &k);
        printf("DHT11[ Humidity:%bu\tTemperature:%bu ]\r\n", j, k);
    }
}
