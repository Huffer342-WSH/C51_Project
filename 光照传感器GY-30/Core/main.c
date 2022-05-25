#include "main.h"
#include "delay.h"

#include "bh1750.h"

#include "usart.h"

void main(void)
{
    float temp;
    Uart_Init();
    printf("This is a program about the GY-30\r\n");
    BH1750_Init();
    while (1)
    {
        temp = BH1750_Read(); //�����������ݣ��洢��BUF��
        printf("����ǿ��:%.2f lux\r\n", temp);
        delayms(500); //��ʱ180ms
    }
}
