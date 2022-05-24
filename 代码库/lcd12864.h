#ifndef __12864DISPLAY_H_
#define __12864DISPLAY_H_

#include <STC/STC12C5A60S2.H>
#include <INTRINS.H>

void lcd12864_Init(); //��ʼ��                                               //��ʼ��
bit lcd12864_Busy();  //�ж��Ƿ�æµ
void lcd12864_TransferData(unsigned char DataOrCommand, bit mdoe); //����ָ������� mode=1:���ݣ�mode=0:ָ��
void lcd12864_DDRAM(unsigned char x, unsigned char y, unsigned char *c); //��DDRAMд���ַ�
void lcd12864_HCDisp(unsigned char *buf, char *str, char len, char pos_begin, char pos_end,  unsigned int ms);

void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len, char pos_begin1, char pos_end1,
                      char pos_begin2, char pos_end2,  unsigned int ms);
#endif