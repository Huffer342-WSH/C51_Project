#include "lcd12864.h"

#include "Delay.h"
/*
V0 亮度
RS(CS) 数据/指令选择
R/W 读/写
E   使能
DB0-7 数据线
PSB  0：串行 1：并行
NC  悬空
RST RESET 复位
BLA 背光正
BLK 背光负

*/
sbit lcd12864_RST = P2 ^ 0;  //复位 低电平有效
sbit lcd12864_PSB = P2 ^ 1;  //并/串行接口选择：H-并行；L-串行
sbit lcd12864_E = P2 ^ 2;    //并行的使能信号；串行的同步时钟(高电位脉冲传送数据)
sbit lcd12864_RW = P2 ^ 3;   //并行的读写选择信号；串行的数据口
sbit lcd12864_RS = P2 ^ 4;   //并行的指令/数据选择信号；串行的片选信号
#define lcd12864_DATAPORT P0 //并行八位端口

#define Delayus Delayus
#define Delayms Delayms

/*9
void lcd12864_Init();													  //初始化 //初始化 bit lcd12864_Busy();
//判断是否忙碌 void lcd12864_TransferData(unsigned char DataOrCommand, bit mdoe);		  //发送指令或数据
mode=1:数据；mode=0:指令 void lcd12864_DDRAM(unsigned char x, unsigned char y, unsigned char *c); //向DDRAM写入字符
void lcd12864_i2s(unsigned char x, unsigned char y, unsigned int *n);	  //显示一个数字
*/

void lcd12864_Init() //初始化，按照技术文档一步一步来
{
    Delayms(41);
    lcd12864_PSB = 1; //并口传输模式
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

bit lcd12864_Busy() //判断是否处于忙碌状态，忙碌则返回0x80否则返回0x00
{
    lcd12864_DATAPORT = 0xFF;
    lcd12864_RS = 0;
    lcd12864_RW = 1;
    lcd12864_E = 1;
    return lcd12864_DATAPORT & 0x80;
}

void lcd12864_TransferData(unsigned char DataOrCommand, bit mode) //发送指令或数据 mode=1:数据；mode=0:指令
{                                                                 // DataOrCommand 为要传输的指令或者数据
    while (lcd12864_Busy() == 0x80)                               //判断是否处于忙碌状态，忙碌则等待
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
    static unsigned char code pos[8] = {0x80, 0x90, 0x88, 0x98, 0xA0, 0xB0, 0xA8, 0xB8}; //一二三四行对应的开头坐标
    unsigned char i;
    if (x < 7 && y < 8) //防止溢出
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
