#include "lcd12864.h"

#include "Delay.h"
/*
V0 ����
RS(CS) ����/ָ��ѡ��
R/W ��/д
E   ʹ��
DB0-7 ������
PSB  0������ 1������
NC  ����
RST RESET ��λ
BLA ������
BLK ���⸺

*/
sbit lcd12864_RST = P2 ^ 0;  //��λ �͵�ƽ��Ч
sbit lcd12864_PSB = P2 ^ 1;  //��/���нӿ�ѡ��H-���У�L-����
sbit lcd12864_E = P2 ^ 2;    //���е�ʹ���źţ����е�ͬ��ʱ��(�ߵ�λ���崫������)
sbit lcd12864_RW = P2 ^ 3;   //���еĶ�дѡ���źţ����е����ݿ�
sbit lcd12864_RS = P2 ^ 4;   //���е�ָ��/����ѡ���źţ����е�Ƭѡ�ź�
#define lcd12864_DATAPORT P0 //���а�λ�˿�

#define Delayus Delayus
#define Delayms Delayms

/*9
void lcd12864_Init();													  //��ʼ�� //��ʼ�� bit lcd12864_Busy();
//�ж��Ƿ�æµ void lcd12864_TransferData(unsigned char DataOrCommand, bit mdoe);		  //����ָ�������
mode=1:���ݣ�mode=0:ָ�� void lcd12864_DDRAM(unsigned char x, unsigned char y, unsigned char *c); //��DDRAMд���ַ�
void lcd12864_i2s(unsigned char x, unsigned char y, unsigned int *n);	  //��ʾһ������
*/

void lcd12864_Init() //��ʼ�������ռ����ĵ�һ��һ����
{
    Delayms(41);
    lcd12864_PSB = 1; //���ڴ���ģʽ
    _nop_();
    lcd12864_RST = 0;
    _nop_();
    lcd12864_RST = 1;
    Delayus(10);
    lcd12864_TransferData(0x30, 0);
    Delayus(100);
    lcd12864_TransferData(0x30, 0);
    Delayus(37);
    lcd12864_TransferData(0x80, 0);
    Delayus(100);
    lcd12864_TransferData(0x10, 0);
    Delayus(100);
    lcd12864_TransferData(0x0C, 0);
    Delayus(100);
    lcd12864_TransferData(0x01, 0);
    Delayms(10);
    lcd12864_TransferData(0x06, 0);
    Delayus(100);
}

bit lcd12864_Busy() //�ж��Ƿ���æµ״̬��æµ�򷵻�0x80���򷵻�0x00
{
    lcd12864_DATAPORT = 0xFF;
    lcd12864_RS = 0;
    lcd12864_RW = 1;
    lcd12864_E = 1;
    return lcd12864_DATAPORT & 0x80;
}

void lcd12864_TransferData(unsigned char DataOrCommand, bit mode) //����ָ������� mode=1:���ݣ�mode=0:ָ��
{                                                                 // DataOrCommand ΪҪ�����ָ���������
    while (lcd12864_Busy() == 0x80)                               //�ж��Ƿ���æµ״̬��æµ��ȴ�
        ;
    lcd12864_RS = mode;
    lcd12864_RW = 0;
    lcd12864_E = 1;
    _nop_();
    lcd12864_DATAPORT = DataOrCommand;
    _nop_();
    lcd12864_E = 0;
}

void lcd12864_DDRAM(unsigned char x, unsigned char y, unsigned char *c)
{
    static unsigned char code pos[8] = {0x80, 0x90, 0x88, 0x98, 0xA0, 0xB0, 0xA8, 0xB8}; //һ�������ж�Ӧ�Ŀ�ͷ����
    unsigned char i;
    if (x < 7 && y < 8) //��ֹ���
    {
        i = pos[y] + x;
        lcd12864_TransferData(i, 0);
        while (*c)
        {
            lcd12864_TransferData(*c, 1);
            c++;
        }
    }
}

void lcd12864_Clear()
{
    lcd12864_TransferData(0x01, 0);
    Delayms(10);
}
