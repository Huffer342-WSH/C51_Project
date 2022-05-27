#include "main.h"
#include "rc522.h"
#include "usart.h"

#include "delay.h"
void main()
{
    CardSerial_t id;
    Uart_Init(); //����ͨ�ų�ʼ��
    PcdInit();
    printf("\r\n========================================\r\n"
           "This is a program about the MF-RC522\r\n"
           "========================================\r\n\r\n");
    while (1)
    {
        PcdFindcard(&id);
        printf("IC��ID:%lx\r\n", id);
        delayms(500);
    }
}