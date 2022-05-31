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

uint8_t code DDRAM_POS[8] = {0x80, 0x90, 0x88, 0x98, 0xA0, 0xB0, 0xA8, 0xB8}; //一二三四行对应的开头坐标

/**
 * @brief 串行发送一个字节
 *
 * @param Dbyte
 */
static void SendByte(uint8_t Dbyte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        Dbyte <<= 1;       //左移一位
        LCD12864_SID = CY; //溢出寄存
        LCD12864_SCLK = 0;
        LCD12864_SCLK = 1;
        LCD12864_SCLK = 0;
    }
}

/**
 * @brief 串行接收一个字节
 *
 * @return uint8_t
 */
static uint8_t ReceiveByte(void)
{
    uint8_t i, temp1, temp2;
    temp1 = 0;
    temp2 = 0;
    GLOBAL_IT_CLOSE();
    for (i = 0; i < 8; i++)
    {
        temp1 = temp1 << 1;
        LCD12864_SCLK = 0;
        LCD12864_SCLK = 1;
        LCD12864_SCLK = 0;
        if (LCD12864_SID)
            temp1++;
    }
    for (i = 0; i < 8; i++)
    {
        temp2 = temp2 << 1;
        LCD12864_SCLK = 0;
        LCD12864_SCLK = 1;
        LCD12864_SCLK = 0;
        if (LCD12864_SID)
            temp2++;
    }
    GLOBAL_IT_OPEN();
    return ((0xf0 & temp1) + (0x0f & temp2));
}

/**
 * @brief 等待LCD12864空闲
 *
 */
void lcd12864_Wait(void)
{
    do
    {
        delayms(2);
        SendByte(0xfc);             // 11111,RW(1),RS(0),0
    } while (0x80 & ReceiveByte()); // BF(.7)=1 Busy
}

/**
 * @brief 发送数据/指令
 *
 * @param option 0>>写指令 1>>写数据
 * @param byte 待传输的字节
 */
void lcd12864_WriteOpt(uint8 option, uint8 byte)
{
#if USECS
    CS = 1;
#endif
    lcd12864_Wait();
    SendByte(option ? 0xfa : 0xf8); // 11111,RW(0),RS(0),0
    SendByte(0xf0 & byte);          //高四位
    SendByte(0xf0 & byte << 4);     //低四位
    lcd12864_Wait();
#if USECS
    CS = 0;
#endif
}

#if !USE_MACRO
/*****写指*****/

void lcd12864_WriteCmd(uint8_t Cbyte)
{
#if USECS
    CS = 1;
#endif
    lcd12864_Wait();
    SendByte(0xf8);              // 11111,RW(0),RS(0),0
    SendByte(0xf0 & Cbyte);      //高四
    SendByte(0xf0 & Cbyte << 4); //低四(先执<<)
#if USECS
    CS = 0;
#endif
}

/*****写数*****/
void lcd12864_WriteData(uint8_t Dbyte)
{
#if USECS
    CS = 1;
#endif
    lcd12864_Wait();
    SendByte(0xfa);              // 11111,RW(0),RS(1),0
    SendByte(0xf0 & Dbyte);      //高四
    SendByte(0xf0 & Dbyte << 4); //低四(先执<<)
#if USECS
    CS = 0;
#endif
}
#endif

/**
 * @brief 初始化LCD
 *
 */
void lcd12864_Init(void)
{
#if USEPSB
    PSB = 0;
#endif
    delayms(50);
    lcd12864_WriteCmd(0x34); //选择拓展指令
    delayms(1);
    lcd12864_WriteCmd(0x30); //选择8bit数据
    delayms(1);
    lcd12864_WriteCmd(0x0c); //开显示(无游标、不反白)
    delayms(1);
    lcd12864_WriteCmd(0x06); //设置游标运动方向
    delayms(1);
    lcd12864_WriteCmd(0x01); //清除显示，并且设定地址指针
    delayms(20);
    lcd12864_WriteCmd(0x06);
    delayms(50);
}

#if USE_lcd12864_DDRAM

/**
 * @brief 显示字符串,以'\0'结尾
 *
 * @param m 行
 * @param n 列
 * @param s 字符串
 */
void lcd12864_DDRAM(uint8_t m, uint8_t n, uint8_t *s)
{
    if (m < 7 && n < 8) //防止溢出
    {
        lcd12864_WriteCmd(DDRAM_POS[m] + n);
        while (*s)
        {
            lcd12864_WriteData(*s);
            s++;
        }
    }
}

/**
 * @brief 显示字符串,长度len
 *
 * @param m 行
 * @param n 列
 * @param s 字符串
 * @param len
 */
void lcd12864_DDRAM_plus(uint8_t m, uint8_t n, uint8_t *s, uint8 len)
{

    data uint8_t i;
    if (n < 8 && m < 8) //防止溢出
    {
        lcd12864_WriteCmd(DDRAM_POS[m] + n);
        i = 0;
        while (i < len)
        {
            lcd12864_WriteData(s[i]);
            i++;
        }
    }
}
#endif

#if USE_lcd12864_DDRAM_Flush
/**
 * @brief 刷写屏幕
 *
 * @param str 待刷入的字符串
 * @param lineLength 每行的长度
 */
void lcd12864_DDRAM_Flush(uint8_t *str, uint8 lineLength)
{
    uint8_t i, j;
    uint8_t jstep[4];
    jstep[0] = 0;
    jstep[1] = lineLength * 2;
    jstep[2] = lineLength;
    jstep[3] = lineLength * 3;
    lcd12864_WriteCmd(0x80);
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 16; i++)
        {
            lcd12864_WriteData(str[jstep[j] + i] ? str[jstep[j] + i] : ' '); //'\0'用空格' '代替
        }
    }
}

#endif
/*****显示图片（显示Gdram内容*****/

/*绘图显示的步
1 关闭绘图显示功能
2 先将垂直的坐(Y)写入绘图RAM地址
3 再将水平的位元组坐标(X)写入绘图RAM地址
4 将D15－D8写入RAM
5 将D7－D0写入到RAM
6 打开绘图显示功能*/

//图片取模方式：横向取模，字节正序
#if USE_lcd12864_GDRAM
void lcd12864_GDRAM(uint8_t *pic) //显示Gdram内容（显示图片）
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
                lcd12864_WriteData(*pic++);      //写数 D15－D8
                lcd12864_WriteData(*pic++);      //写数 D7－D0
                lcd12864_WriteCmd(0x30);
            }
        }
}
#endif

#include "string.h"

#if USE_lcd12864_HCDisp
/**
 * @brief 横向滚动显示一段字
 *
 * @param buf 缓冲 至少17字节
 * @param str 需要显示的字符
 * @param len 字符串长
 * @param pos_begin 开始位
 * @param pos_end 结束位置
 * @param ms 间隔时间
 */
void lcd12864_HCDisp(unsigned char *buf, char *str, char len, char pos_begin, char pos_end,
                     unsigned int ms) //横向滚动显示一段字
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
#endif

#if USE_lcd12864_HCDisp2
void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len, char pos_begin1, char pos_end1,
                      char pos_begin2, char pos_end2,
                      unsigned int ms) //横向滚动显示一段字
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
            lcd12864_DDRAM(0, 1, buf);
            pos_begin1 += (pos_begin1 > pos_end1 ? -1 : 1);
        }

        Delayms(ms / 2);
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
            lcd12864_DDRAM(0, 2, buf);
            pos_begin2 += (pos_begin2 > pos_end2 ? -1 : 1);
        }
        Delayms(ms / 2);
    }
}

#endif