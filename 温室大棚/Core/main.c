#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "delay.h"
#include "dht11.h"
#include "lcd12864_s.h"
#include "usart.h"

void BootAnime();
void tm0_init();
//��Ļ����
uint8_t ScreenBuf[72];
uint16 tm0Count; //��ʱ��0�жϼ���
uint8 humi_real, temp_real;

#define N 18
#define LINE(x) ScreenBuf + x *N
void main(void)
{
    uint8 i, j, k;

    /*-------------------��ʼ��---------------------------------*/
    P4SW = 0xff;              // P4�˿ھ���IO��P4.7���⣩
    P0M0 = 0x00, P0M1 = 0x00; // P0©��������˫��IO
    Uart_Init();
    lcd12864_Init();
    tm0_init();
    printf("hello\r\n");
    BootAnime();
    lcd12864_Init();
    i = 'A';
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    while (1)
    {
        sprintf(LINE(0), "�¶�:%-3buʪ��:%-3bu", temp_real, humi_real);
        lcd12864_DDRAM_Flush(ScreenBuf, N);
        delayms(200);
    }
}
/*��������*/
void BootAnime()
{
    uint8 i = 0;
    lcd12864_Init();
    delayms(500);
    lcd12864_DDRAM(0, 0, "      ICCS");
    lcd12864_DDRAM(1, 0, "      1085");

    for (i = 0; i < 32; i++)
    {
        lcd12864_WriteCmd(0x34);     //������չָ��
        lcd12864_WriteCmd(0x03);     //����д�����ַ
        lcd12864_WriteCmd(0x40 + i); //д�����ַ
        delayms(80);
        // lcd12864_WriteCmd(0x30);
        // lcd12864_WriteCmd(0x05);
    }
}

/* Timer0 interrupt routine */
void tm0_isr() interrupt 1
{
    //��ʱ����װ
    TL0 = 0x00;
    TH0 = 0x4C;
    // 50*40=2000,����һ�Σ���ȡ����
    if (tm0Count-- == 0)
    {
        tm0Count = 40;
        DHT11_Read(&humi_real, &temp_real);
    }
}

/**
 * @brief ��ʱ��0��ʼ��������50ms
 *
 */
void tm0_init()
{

    AUXR &= 0x7F; //��ʱ��ʱ��12Tģʽ
    TMOD &= 0xF0; //���ö�ʱ��ģʽ
    TMOD |= 0x01; //���ö�ʱ��ģʽ
    TL0 = 0x00;   //���ö�ʱ��ʼֵ
    TH0 = 0x4C;   //���ö�ʱ��ʼֵ
    TF0 = 0;      //���TF0��־
    TR0 = 1;      //��ʱ��0��ʼ��ʱ

    ET0 = 1;      // enable timer0 interrupt
    EA = 1;       // open global interrupt switch
    tm0Count = 0; // initial counter
}
