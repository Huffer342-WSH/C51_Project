
/*********************************************************************************
* ����дʱ�䡿�� 2011.07.07
* ����    �ߡ��� �������
* ����    ������ V1.0
* ����    վ���� http://ylelectronic.taobao.com/
* ��Q      Q���� 348439350
* ����    ������ �˳��������ѧϰ��ο���������ע����Ȩ��������Ϣ��
* ���������ܡ��� DA-AD����
* ��ʹ��˵������ ��Ƭ����STC89C52
                 ���ڲ����ʣ�9600
                 ����һ������4���ֽ�
                 1 ��������
                 2 ��������
                 3 ����
                 4 �ɵ�0-5V��ѹ����ɫ��λ�����ڣ�

**********************************************************************************/
/********************************************************************************************/

#include <reg52.h> //������Ƭ���Ĵ�����ͷ�ļ�
#include <I2C.H>
#include "pcf8591.h"

#define uchar unsigned char
#define PCF8591 0x90 // PCF8591 ��ַ

// ��������
unsigned char AD_CHANNEL;
unsigned long xdata LedOut[8];
unsigned int D[32];
/*
sbit P2_0 = P2^0;
sbit P2_1 = P2^1;
sbit P2_2 = P2^2;
sbit P2_3 = P2^3;
*/
unsigned char date;

uchar code table[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};

// ��������

void display();
void delay(uchar i);
bit DACconversion(unsigned char sla, unsigned char c, unsigned char Val);
bit ISendByte(unsigned char sla, unsigned char c);
unsigned char IRcvByte(unsigned char sla);
void Initial_com(void);
//******************************************************************/
main()
{

    Initial_com();
    while (1)
    {

        /********����AD-DA����*************/

        DA_Res(&D[0]);

        /********���½�AD��ֵͨ�����ڷ��ͳ�ȥ*************/
        delay(200);
        SBUF = D[0];
        //        delay(200);
        //        SBUF = D[1];
        //        delay(200);
        //        SBUF = D[2];
        //        delay(200);
        //        SBUF = D[3];
        delay(200);
        if (RI)
        {
            date = SBUF; //��Ƭ������
            SBUF = date; //��Ƭ������
            RI = 0;
        }
    }
}

void delay(uchar i)
{
    uchar j, k;
    for (j = i; j > 0; j--)
        for (k = 125; k > 0; k--)
            ;
}

//*****���ڳ�ʼ������***********

//******************************
void Initial_com(void)
{
    EA = 1;      //�����ж�
    ES = 1;      //�������ж�
    ET1 = 1;     //����ʱ��T1���ж�
    TMOD = 0x20; //��ʱ��T1���ڷ�ʽ2�жϲ���������
    PCON = 0x00; // SMOD=0
    SCON = 0x50; // ��ʽ1 �ɶ�ʱ������
    TH1 = 0xfd;  //����������Ϊ9600
    TL1 = 0xfd;
    TR1 = 1; //����ʱ��T1���п���λ
}
