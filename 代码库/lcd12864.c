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
#if 0
        while (y < 8 && *c)
        {
            if (y < 7 && i > pos[y] + 15) //换行
            {
                y++;
                i = pos[y];
                lcd12864_TransferData(i, 0);
            }

            if (*c >= 0xA0 && i % 2 && i <= pos[y] + 13) //遇到中文字符未知错误自动加空格调整位置
            {
                lcd12864_TransferData(' ', 1);
                lcd12864_TransferData(*c, 1);
                c++;
                lcd12864_TransferData(*c, 1);
                c++;
                i += 3;
            }
            else if (*c >= 0xA0 && i <= pos[y] + 14) //位置正确的中文字符正常输出
            {
                lcd12864_TransferData(*c, 1);
                c++;
                lcd12864_TransferData(*c, 1);
                c++;
                i += 2;
            }
            else //西文字符正常输出
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
 * @brief 横向滚动显示一段字符
 *
 * @param buf 缓冲区 至少17字节
 * @param str 需要显示的字符串
 * @param len 字符串长度
 * @param pos_begin 开始位置
 * @param pos_end 结束位置
 * @param ms 间隔时间
 */
void lcd12864_HCDisp(unsigned char *buf, char *str, char len, char pos_begin, char pos_end,
                     unsigned int ms) //横向滚动显示一段字符
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
                      unsigned int ms) //横向滚动显示一段字符
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