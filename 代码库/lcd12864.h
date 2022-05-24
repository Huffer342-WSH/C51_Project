#ifndef __12864DISPLAY_H_
#define __12864DISPLAY_H_

#include <STC/STC12C5A60S2.H>
#include <INTRINS.H>

void lcd12864_Init(); //初始化                                               //初始化
bit lcd12864_Busy();  //判断是否忙碌
void lcd12864_TransferData(unsigned char DataOrCommand, bit mdoe); //发送指令或数据 mode=1:数据；mode=0:指令
void lcd12864_DDRAM(unsigned char x, unsigned char y, unsigned char *c); //向DDRAM写入字符
void lcd12864_HCDisp(unsigned char *buf, char *str, char len, char pos_begin, char pos_end,  unsigned int ms);

void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len, char pos_begin1, char pos_end1,
                      char pos_begin2, char pos_end2,  unsigned int ms);
#endif