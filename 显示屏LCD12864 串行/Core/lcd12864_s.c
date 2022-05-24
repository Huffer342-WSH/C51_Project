/**
 * @file lcd12864.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief LCD12864_ST7920串行测试程序
 * @date 2022-04-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "lcd12864_s.h"

/*
sbit CS="P0"^4;                //RS=CS
sbit SID="P0"^5;               //RW=SID
sbit SCLK="P0"^6;               //E=SCLK
*/

/*****LCD接口定义*****/

sbit SID = P4 ^ 0;  //串行数据
sbit SCLK = P4 ^ 1; //串行同步时钟
#if USEPSB
sbit PSB = P2 ^ 3; //串并口选择 1:并口模式; 0:串口模式;
#endif
#if USECS
sbit CS = P4 ^ 0; //片选信号
#endif

/*****串行发送一个字节*****/
void SendByte(uint8_t Dbyte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        SCLK = 0;
        Dbyte = Dbyte << 1; //左移一位
        SID = CY;           //移出的位给SID
        SCLK = 1;
        SCLK = 0;
    }
}

/*****串行接收一个字节*****/
uint8_t ReceiveByte(void)
{
    uint8_t i, temp1, temp2;
    temp1 = 0;
    temp2 = 0;
    for (i = 0; i < 8; i++)
    {
        temp1 = temp1 << 1;
        SCLK = 0;
        SCLK = 1;
        SCLK = 0;
        if (SID)
            temp1++;
    }
    for (i = 0; i < 8; i++)
    {
        temp2 = temp2 << 1;
        SCLK = 0;
        SCLK = 1;
        SCLK = 0;
        if (SID)
            temp2++;
    }
    return ((0xf0 & temp1) + (0x0f & temp2));
}

/*****检测LCD忙状态*****/
void lcd12864_Wait(void)
{
    do
        SendByte(0xfc);           // 11111,RW(1),RS(0),0
    while (0x80 & ReceiveByte()); // BF(.7)=1 Busy
}

/*****写指令*****/
void lcd12864_WriteCmd(uint8_t Cbyte)
{
#if USECS
    CS = 1;
#endif
    lcd12864_Wait();
    SendByte(0xf8);              // 11111,RW(0),RS(0),0
    SendByte(0xf0 & Cbyte);      //高四位
    SendByte(0xf0 & Cbyte << 4); //低四位(先执行<<)
#if USECS
    CS = 0;
#endif
}

/*****写数据*****/
void lcd12864_WriteData(uint8_t Dbyte)
{
#if USECS
    CS = 1;
#endif
    lcd12864_Wait();
    SendByte(0xfa);              // 11111,RW(0),RS(1),0
    SendByte(0xf0 & Dbyte);      //高四位
    SendByte(0xf0 & Dbyte << 4); //低四位(先执行<<)
#if USECS
    CS = 0;
#endif
}

/*****初始化LCD*****/
void lcd12864_Init(void)
{
#if USEPSB
    PSB = 0;
#endif
    delayms(50);
    lcd12864_WriteCmd(0x34); //选择基本指令集
    delayms(1);
    lcd12864_WriteCmd(0x30); //选择8bit数据流
    delayms(1);
    lcd12864_WriteCmd(0x0c); //开显示(无游标、不反白)
    delayms(1);
    lcd12864_WriteCmd(0x01); //清除显示，并且设定地址指针为00H
    delayms(20);
}

/*****显示字符串*****/
void lcd12864_DDRAM(uint8_t x, uint8_t y, uint8_t *c)
{
    static uint8_t code pos[8] = {0x80, 0x90, 0x88, 0x98, 0xA0, 0xB0, 0xA8, 0xB8}; //一二三四行对应的开头坐标
    uint8_t i;
    if (x < 7 && y < 8) //防止溢出
    {
        i = pos[y] + x;
        lcd12864_WriteCmd(i);
        while (*c)
        {
            lcd12864_WriteData(*c);
            c++;
        }
    }
}

void lcd12864_DDRAM_Flush(uint8_t xdata *c)
{
    uint8_t i, j;
    uint8_t code jstep[] = {0, 32, 16, 48};
    lcd12864_WriteCmd(0x80);
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 16; i++)
        {
            _nop_();
            lcd12864_WriteData(c[jstep[j] + i]);
        }
    }
}
/*****显示图片（显示Gdram内容）*****/

/*绘图显示的步骤
1 关闭绘图显示功能
2 先将垂直的坐标(Y)写入绘图RAM地址
3 再将水平的位元组坐标(X)写入绘图RAM地址
4 将D15－D8写入RAM中
5 将D7－D0写入到RAM中
6 打开绘图显示功能*/

//图片取模方式：横向取模，字节正序
void pic_Disp(uint8_t *pic) //显示Gdram内容（显示图片）
{
    uint8_t x, y, i;
    for (i = 0; i < 9; i = i + 8)
        for (y = 0; y < 32; y++)
        {
            for (x = 0; x < 8; x++)
            {
                lcd12864_WriteCmd(0x36);         //扩充指令，开绘图显示
                lcd12864_WriteCmd(0x80 + y);     //行地址
                lcd12864_WriteCmd(0x80 + x + i); //列地址
                lcd12864_WriteData(*pic++);      //写数据 D15－D8
                lcd12864_WriteData(*pic++);      //写数据 D7－D0
                lcd12864_WriteCmd(0x30);
            }
        }
}

/*****整屏显示竖条*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
//当i=0时显示上面128×32,当i=8时显示下面128×32
void LcdFill_Vertical()
{
    uint8_t x, y, i;
    for (i = 0; i < 9; i = i + 8)
        for (y = 0; y < 0x20; y++)
        {
            for (x = 0; x < 8; x++)
            {
                lcd12864_WriteCmd(0x36);         //扩充指令 绘图显示
                lcd12864_WriteCmd(y + 0x80);     //行地址
                lcd12864_WriteCmd(x + 0x80 + i); //列地址
                lcd12864_WriteData(0x55);        //写数据 D15－D8
                lcd12864_WriteData(0x55);        //写数据 D7－D0
                lcd12864_WriteCmd(0x30);
            }
        }
}

/*****整屏显示横条*****/
void LcdFill_Level()
{
    uint8_t x, y, i;
    uint8_t k;
    for (i = 0; i < 9; i += 8)
        for (y = 0; y < 0x20; y++)
        {
            if ((y % 2) == 1)
                k = 0xff;
            else
                k = 0;
            for (x = 0; x < 8; x++)
            {
                lcd12864_WriteCmd(0x36);         //扩充指令 绘图显示
                lcd12864_WriteCmd(y + 0x80);     //行地址
                lcd12864_WriteCmd(x + 0x80 + i); //列地址
                lcd12864_WriteData(k);
                lcd12864_WriteData(k);
                lcd12864_WriteCmd(0x30);
            }
        }
}

/*****整屏充满亮点或暗点*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
//当i=0时显示上面128×32,当i=8时显示下面128×32
void Lcd_Fill(uint8_t test)
{
    uint8_t x, y, i;
    for (i = 0; i < 9; i = i + 8)
        for (y = 0; y < 0x20; y++)
        {
            for (x = 0; x < 8; x++)
            {
                lcd12864_WriteCmd(0x36);         //扩充指令 绘图显示
                lcd12864_WriteCmd(y + 0x80);     //行地址
                lcd12864_WriteCmd(x + 0x80 + i); //列地址
                lcd12864_WriteData(test);        //写数据 D15－D8
                lcd12864_WriteData(test);        //写数据 D7－D0
                lcd12864_WriteCmd(0x30);
            }
        }
}
