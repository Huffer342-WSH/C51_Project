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
#if 0
        while (y < 8 && *c)
        {
            if (y < 7 && i > pos[y] + 15) //����
            {
                y++;
                i = pos[y];
                lcd12864_TransferData(i, 0);
            }

            if (*c >= 0xA0 && i % 2 && i <= pos[y] + 13) //���������ַ�δ֪�����Զ��ӿո����λ��
            {
                lcd12864_TransferData(' ', 1);
                lcd12864_TransferData(*c, 1);
                c++;
                lcd12864_TransferData(*c, 1);
                c++;
                i += 3;
            }
            else if (*c >= 0xA0 && i <= pos[y] + 14) //λ����ȷ�������ַ��������
            {
                lcd12864_TransferData(*c, 1);
                c++;
                lcd12864_TransferData(*c, 1);
                c++;
                i += 2;
            }
            else //�����ַ��������
            {
                lcd12864_TransferData(*c, 1);
                i++;
                c++;
            }
        }
#endif
    }
}

void lcd12864_Clear()
{
    lcd12864_TransferData(0x01, 0);
    Delayms(10);
}

#include "string.h"

/**
 * @brief ���������ʾһ���ַ�
 *
 * @param buf ������ ����17�ֽ�
 * @param str ��Ҫ��ʾ���ַ���
 * @param len �ַ�������
 * @param pos_begin ��ʼλ��
 * @param pos_end ����λ��
 * @param ms ���ʱ��
 */
void lcd12864_HCDisp(unsigned char *buf, char *str, char len, char pos_begin, char pos_end,
                     unsigned int ms) //���������ʾһ���ַ�
{
    char i, j;

    for (; pos_begin != pos_end; pos_begin += (pos_begin > pos_end ? -1 : 1))
    {
        memset(buf, ' ', 16);
        buf[16] = 0;
        for (i = pos_begin, j = 0; (i < pos_begin + 16) && (j < len); i++, j++)
        {
            if (i >= 0 && i < 16)
            {
                buf[i] = str[j];
            }
        }
        lcd12864_DDRAM(0, 1, buf);
        Delayms(ms);
    }
}

void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len, char pos_begin1, char pos_end1,
                      char pos_begin2, char pos_end2,
                      unsigned int ms) //���������ʾһ���ַ�
{
    char i, j;
    pos_end1 += (pos_begin1 > pos_end1 ? -1 : 1);
    pos_end2 += (pos_begin2 > pos_end2 ? -1 : 1);
    while (pos_begin1 != pos_end1 || pos_begin2 != pos_end2)
    {
        if (pos_begin1 != pos_end1)
        {
            memset(buf, ' ', 16);

            for (i = pos_begin1, j = 0; (i < pos_begin1 + 16) && (j < len); i++, j++)
            {
                if (i >= 0 && i < 16)
                {
                    buf[i] = str1[j];
                }
            }
            buf[16] = 0;
            lcd12864_DDRAM(0, 0, buf);
            pos_begin1 += (pos_begin1 > pos_end1 ? -1 : 1);
        }

        Delayms(ms / 2);
        lcd12864_DDRAM(0, 2, "a");
        if (pos_begin2 != pos_end2)
        {
            memset(buf, ' ', 16);
            buf[16] = 0;
            for (i = pos_begin2, j = 0; (i < pos_begin2 + 16) && (j < len); i++, j++)
            {
                if (i >= 0 && i < 16)
                {
                    buf[i] = str2[j];
                }
            }
            lcd12864_DDRAM(0, 2, "b");
            lcd12864_DDRAM(0, 3, buf);
            lcd12864_DDRAM(0, 2, "c");
            pos_begin2 += (pos_begin2 > pos_end2 ? -1 : 1);
        }
        Delayms(ms / 2);
    }
}