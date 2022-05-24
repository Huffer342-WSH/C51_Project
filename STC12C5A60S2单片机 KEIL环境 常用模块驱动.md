[TOC]



# STC12C5A60S2

## 管脚图

![image-20220422152432664](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422152432664.png)

![image-20220422152442671](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422152442671.png)

![image-20220422153741200](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422153741200.png)

![image-20220422153754743](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422153754743.png)

![image-20220422153806776](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422153806776.png)



## 引脚配置

#### P4端口设置

![image-20220422152747775](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422152747775.png)

### 特殊功能输出引脚选择

![image-20220422152828698](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422152828698.png)

### 普通引脚配置

从STC-ISP中导出STC12C5A60S2.h，内部有相关特殊寄存器定义

![image-20220422154015137](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422154015137.png)

```c
//新一代 1T 8051系列 单片机I/O 口特殊功能寄存器
//                                      7     6     5     4     3     2     1     0         Reset Value
sfr P0 = 0x80; // 8 bitPort0          P0.7  P0.6  P0.5  P0.4  P0.3  P0.2  P0.1  P0.0       1111,1111
sbit P00 = P0 ^ 0;
sbit P01 = P0 ^ 1;
sbit P02 = P0 ^ 2;
sbit P03 = P0 ^ 3;
sbit P04 = P0 ^ 4;
sbit P05 = P0 ^ 5;
sbit P06 = P0 ^ 6;
sbit P07 = P0 ^ 7;
sfr P0M0 = 0x94; //                                                                         0000,0000
sfr P0M1 = 0x93; //                                                                         0000,0000
```

## 中断

![image-20220422154334608](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422154334608.png)

# STC12C5A60S2单片机 KEIL环境 常用模块驱动



## 命名规范

引脚定义：模块缩写_模块引脚（大写）  如： sbit DS1302_RST = P3 ^ 5; 不易重名的引脚不需要模块缩写

函数定义：模块缩写_功能，组合单词首字母大写，长单词取音节字母  如 ： uint16 HCSR04_GetDist();

工具人变量：i,j,k,t,temp ; 和：sum； 平均： ave； 

## 建议的main.h

main.h中应当包含所有引脚的定义与变量的重命名们尽量，extern必要的全局变量，include通用的头文件

```c
#ifndef __MAIN_H_
#define __MAIN_H_

#include "STC12C5A60S2.h"
#include "intrins.h"

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long
#define uint8_t unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long
#define int8 char
#define int16 int
#define int32 long
#define int8_t char
#define int16_t int
#define int32_t long
#define float32 float

extern uint8 LED[4];

#define STATE_AUTO (uint8)0
#define STATE_SETTIME (uint8)1
#define STATE_SETSEPPED (uint8)2
#define STATE_MANUAL (uint8)3
#define GLOBAL_IT_OPEN() EA = 1
#define GLOBAL_IT_CLOSE() EA = 0

sbit DS1302_RST = P3 ^ 5;  //时钟芯片
sbit DS1302_DAT = P3 ^ 6;
sbit DS1302_CLK = P3 ^ 7;

sbit SCL = P4 ^ 4;  // I2C  时钟
sbit SDA = P4 ^ 5;  // I2C  数据

sbit Echo = P1 ^ 5;  //超声波echo
sbit Trig = P1 ^ 6;  //超声波trig

sbit LCD_SID = P4 ^ 0;   //串行数据
sbit LCD_SCLK = P4 ^ 1;  //串行同步时钟

sbit HC595_DIO = P1 ^ 0;   //串行数据输入
sbit HC595_RCLK = P1 ^ 1;  //时钟脉冲信号――上升沿有效
sbit HC595_SCLK = P1 ^ 2;  //打入信号――――上升沿有效

sbit DIO = P2 ^ 0;
sbit CLK = P2 ^ 1;
sbit STB = P2 ^ 2;

sbit HW = P1 ^ 7;
// PWM:1.3,1.4
//光电门： 外部中断0 3.2
#define KeyBoard_Port P0

#define USESTC12C5A60S2_H_

#endif
```



## 延迟函数

### delay.h

```c
#ifndef __DELAY_H_
#define __DELAY_H_

#define delayms Delayms
#define delayus Delayus
void Delayms(unsigned int t);  //@11.0592MHz
void Delayus(unsigned int t);

#endif
```

### delay.c

```c
#include "delay.h"
#include "intrins.h"
void Delayms(unsigned int t)  //@11.0592MHz
{
  unsigned char i, j;
  while (t--) {
    _nop_();
    i = 11;
    j = 185;
    do {
      while (--j)
        ;
    } while (--i);
  }
}

#if 1
void Delayus(unsigned int t)  //@11.0592MHzŃÓłŮtÎ˘Ăë
{
  while (t--) _nop_();
}
#endif
```

## 串口

串口1独立波特率发生器 115200bit/s

### usart.h

```c
#ifndef __USART_H_
#define __USART_H_
#include "main.h"
#define USE_SendData 1
#define USE_SendString 1

void Uart_Init(void);
void SendData(unsigned char dat);
void SendString(char *s);

#endif
```

### usart.c

```c
#include "usart.h"

bit busy;

/*----------------------------
UART initialization
----------------------------*/

void Uart_Init(void)  // 115200bps@11.0592MHz
{
  PCON &= 0x7F;  //波特率不倍速
  SCON = 0x50;   // 8位数据,可变波特率
  AUXR &= 0xFB;  //定时器时钟12T模式
  BRT = 0xFD;    //设置定时重载值
  AUXR |= 0x01;  //串口1使用独立波特率发射器为波特率发生器
  AUXR |= 0x10;  //启动独立波特率发射器

  EA = 1;  //开总中断
  ES = 1;  //开串口中断
}

/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4 {
  if (RI) {
    RI = 0;  // Clear receive interrupt flag
  }
  if (TI) {
    TI = 0;    // Clear transmit interrupt flag
    busy = 0;  // Clear transmit busy flag
  }
}

#if USE_SendData
/*----------------------------
Send a byte data to UART
Input: dat (data to be sent)
Output:None
----------------------------*/
void SendData(uint8 dat) {
  while (busy)
    ;         // Wait for the completion of the previous data is sent
  ACC = dat;  // Calculate the even parity bit P (PSW.0)
  busy = 1;
  SBUF = ACC;  // Send data to UART buffer
}
#endif

#if USE_SendString
/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void SendString(char *s) {
  while (*s)  // Check the end of the string
  {
    SendData(*s++);  // Send current char and increment string ptr
  }
}
#endif

```



## 内置EEPROM

![image-20220422162252378](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422162252378.png)

每个扇区512字节，每次读写在同一个山区

### eeprom.h

```c
#ifndef __EEPROM_H_
#define __EEPROM_H_

#include "main.h"

#ifndef __STC12C5A60S2_H__
/*Declare SFR associated with the IAP */
sfr IAP_DATA = 0xC2;   // Flash data register
sfr IAP_ADDRH = 0xC3;  // Flash address HIGH
sfr IAP_ADDRL = 0xC4;  // Flash address LOW
sfr IAP_CMD = 0xC5;    // Flash command register
sfr IAP_TRIG = 0xC6;   // Flash command trigger
sfr IAP_CONTR = 0xC7;  // Flash control register
#endif

/*Define ISP/IAP/EEPROM command*/
#define CMD_IDLE 0     // Stand-By
#define CMD_READ 1     // Byte-Read
#define CMD_PROGRAM 2  // Byte-Program
#define CMD_ERASE 3    // Sector-Erase

/*Define ISP/IAP/EEPROM operation const for IAP_CONTR*/
//#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
// #define ENABLE_IAP 0x82  // if SYSCLK<20MHz
#define ENABLE_IAP 0x83  // if SYSCLK<12MHz
//#define ENABLE_IAP 0x84           //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85           //if SYSCLK<3MHz
//#define ENABLE_IAP 0x86           //if SYSCLK<2MHz
//#define ENABLE_IAP 0x87           //if SYSCLK<1MHz

// STC12C60S2 EEPROM 起始地址
#define IAP_ADDRESS 0x0400

uint8 IapReadByte(uint16 addr);
void IapProgramByte(uint16 addr, uint8 dat);
void IapEraseSector(uint16 addr);

#endif
```

### eeprom.c

```c
#include "eeprom.h"

void Delay(uint8 n);
void IapIdle();

/*----------------------------
Software delay function
----------------------------*/
static void Delay(uint8 n) {
  uint16 x;

  while (n--) {
    x = 0;
    while (++x)
      ;
  }
}

/*----------------------------
Disable ISP/IAP/EEPROM function
Make MCU in a safe state
----------------------------*/
void IapIdle() {
  IAP_CONTR = 0;     // Close IAP function
  IAP_CMD = 0;       // Clear command to standby
  IAP_TRIG = 0;      // Clear trigger register
  IAP_ADDRH = 0x80;  // Data ptr point to non-EEPROM area
  IAP_ADDRL = 0;     // Clear IAP address to prevent misuse
}

/*----------------------------
Read one byte from ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
Output:Flash data
----------------------------*/
uint8 IapReadByte(uint16 addr) {
  uint8 dat;  // Data buffer

  IAP_CONTR = ENABLE_IAP;  // Open IAP function, and set wait time
  IAP_CMD = CMD_READ;      // Set ISP/IAP/EEPROM READ command
  IAP_ADDRL = addr;        // Set ISP/IAP/EEPROM address low
  IAP_ADDRH = addr >> 8;   // Set ISP/IAP/EEPROM address high
  IAP_TRIG = 0x5a;         // Send trigger command1 (0x5a)
  IAP_TRIG = 0xa5;         // Send trigger command2 (0xa5)
  _nop_();         // MCU will hold here until ISP/IAP/EEPROM operation complete
  dat = IAP_DATA;  // Read ISP/IAP/EEPROM data
  IapIdle();       // Close ISP/IAP/EEPROM function

  return dat;  // Return Flash data
}

/*----------------------------
Program one byte to ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
       dat (ISP/IAP/EEPROM data)
Output:-
----------------------------*/
void IapProgramByte(uint16 addr, uint8 dat) {
  IAP_CONTR = ENABLE_IAP;  // Open IAP function, and set wait time
  IAP_CMD = CMD_PROGRAM;   // Set ISP/IAP/EEPROM PROGRAM command
  IAP_ADDRL = addr;        // Set ISP/IAP/EEPROM address low
  IAP_ADDRH = addr >> 8;   // Set ISP/IAP/EEPROM address high
  IAP_DATA = dat;          // Write ISP/IAP/EEPROM data
  IAP_TRIG = 0x5a;         // Send trigger command1 (0x5a)
  IAP_TRIG = 0xa5;         // Send trigger command2 (0xa5)
  _nop_();  // MCU will hold here until ISP/IAP/EEPROM operation complete
  IapIdle();
}

/*----------------------------
Erase one sector area
Input: addr (ISP/IAP/EEPROM address)
Output:-
----------------------------*/
void IapEraseSector(uint16 addr) {
  IAP_CONTR = ENABLE_IAP;  // Open IAP function, and set wait time
  IAP_CMD = CMD_ERASE;     // Set ISP/IAP/EEPROM ERASE command
  IAP_ADDRL = addr;        // Set ISP/IAP/EEPROM address low
  IAP_ADDRH = addr >> 8;   // Set ISP/IAP/EEPROM address high
  IAP_TRIG = 0x5a;         // Send trigger command1 (0x5a)
  IAP_TRIG = 0xa5;         // Send trigger command2 (0xa5)
  _nop_();  // MCU will hold here until ISP/IAP/EEPROM operation complete
  IapIdle();
}

```



## PCA模块输出PWM

### pwm.h

```c
#ifndef __PWM_H_
#define __PWM_H_
#include "main.h"

void PWM_Init(void);
#define PWM_SetCAP0(x) CCAP0H = CCAP0L = x
#define PWM_SetCAP1(x) CCAP1H = CCAP1L = = x

#define PWM_SetDutyH0(x) CCAP0H = CCAP0L = 0xff - x
#define PWM_SetDutyH1(x) CCAP1H = CCAP1L = 0xff - x
#endif
```

### pwm.c

```c
#include "pwm.h"
#define FOSC 11059200L

typedef unsigned char BYTE;
typedef unsigned int WORD;

#ifndef __STC12C5A60S2_H__
/*Declare SFR associated with the PCA */
sfr CCON = 0xD8;       // PCA control register
sbit CCF0 = CCON ^ 0;  // PCA module-0 interrupt flag
sbit CCF1 = CCON ^ 1;  // PCA module-1 interrupt flag
sbit CR = CCON ^ 6;    // PCA timer run control bit
sbit CF = CCON ^ 7;    // PCA timer overflow flag
sfr CMOD = 0xD9;       // PCA mode register
sfr CL = 0xE9;         // PCA base timer LOW
sfr CH = 0xF9;         // PCA base timer HIGH
sfr CCAPM0 = 0xDA;     // PCA module-0 mode register
sfr CCAP0L = 0xEA;     // PCA module-0 capture register LOW
sfr CCAP0H = 0xFA;     // PCA module-0 capture register HIGH
sfr CCAPM1 = 0xDB;     // PCA module-1 mode register
sfr CCAP1L = 0xEB;     // PCA module-1 capture register LOW
sfr CCAP1H = 0xFB;     // PCA module-1 capture register HIGH
sfr PCAPWM0 = 0xf2;
sfr PCAPWM1 = 0xf3;

sfr AUXR1 = 0xA2;  // Auxiliary Register 1  -  PCA_P4  SPI_P4  S2_P4  GF2 ADRJ
                   // -    DPS  0000,0000

PCA_P4 : 0, 缺省PCA 在P1 口 1，PCA /
                PWM 从P1 口切换到P4 口 : ECI 从P1 .2 切换到P4 .1 口， PCA0 /
                                         PWM0 从P1 .3 切换到P4 .2 口 PCA1 /
                                         PWM1 从P1 .4 切换到P4 .3 口
#endif

                                         void
                                         PWM_Init(void) {

  CCON = 0;  // Initial PCA control register
             // PCA timer stop running
             // Clear CF flag
             // Clear all module interrupt flag
  CL = 0;    // Reset PCA base timer
  CH = 0;
  CMOD = 0x00;             // Set PCA timer clock source as Fosc/12
                           // Disable PCA timer overflow interrupt
  CCAP0H = CCAP0L = 0xff;  // PWM0 port output 50% duty cycle square wave
  CCAPM0 = 0x42;  // PCA module-0 work in 8-bit PWM mode and no PCA interrupt
  PCA_PWM0 = 0x01;
  CCAP1H = CCAP1L = 0xff;  // PWM1 port output 0% duty cycle square wave
  PCA_PWM1 = 0x00;
  CCAPM1 = 0x42;  // PCA module-1 work in 8-bit PWM mode and no PCA interrupt

  CR = 1;  // PCA timer start run
}

```

### L298N原理图

![image-20220422165948161](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422165948161.png)

## 时钟芯片DS1302

### ds1302.h

```c
#include "main.h"

#ifndef __MAIN_H_
// DS1302引脚定义,可根据实际情况自行修改端口定义
sbit DS1302_RST = P3 ^ 5;
sbit DS1302_DAT = P3 ^ 6;
sbit DS1302_CLK = P3 ^ 7;
#endif
// DS1302地址定义
#define ds1302_sec_add 0x80      //秒数据地址
#define ds1302_min_add 0x82      //分数据地址
#define ds1302_hr_add 0x84       //时数据地址
#define ds1302_date_add 0x86     //日数据地址
#define ds1302_month_add 0x88    //月数据地址
#define ds1302_day_add 0x8a      //星期数据地址
#define ds1302_year_add 0x8c     //年数据地址
#define ds1302_control_add 0x8e  //控制数据地址
#define ds1302_charger_add 0x90
#define ds1302_clkburst_add 0xbe

typedef struct {
  uint8 year_h;
  uint8 year_l;
  uint8 month;
  uint8 day;
  uint8 hour;
  uint8 minute;
  uint8 second;
  uint8 week;
} DS1302_BCD;

typedef union {
  DS1302_BCD name;
  uint8 array[8];
} DS1302_union;
#define USE_ds1302_write_time 1
#define USE_ds1302_read_time 1
void ds1302_init(void);                     // DS1302初始化函数
void ds1302_write_time(DS1302_union *buf);  //向DS302写入时钟数据
void ds1302_read_time(DS1302_union *buf);   //从DS302读出时钟数据

```

### ds1302.c

预编译指令选择读写字节的代码

```c
#include "ds1302.h"

static void ds1302_SPI_SendByte(uint8 byte) {
  uint8 i;

  GLOBAL_IT_CLOSE();

  for (i = 0; i < 8; i++) {
    DS1302_DAT = byte & 0x01;
    DS1302_CLK = 1;
    DS1302_CLK = 0;
    byte >>= 1;
  }

  GLOBAL_IT_OPEN();
}

//从DS1302读出一字节数据
static uint8 ds1302_read_byte(uint8 addr) {
  uint8 i, temp;
  DS1302_RST = 1;  //启动DS1302总线
  //写入目标地址：addr
  addr = addr | 0x01;  //最低位置高，寄存器0位为0时写，为1时读
#if 1
  ds1302_SPI_SendByte(addr);
#else

  GLOBAL_IT_CLOSE();
  for (i = 0; i < 8; i++) {
    if (addr & 0x01) {
      IO_ds1302 = 1;
    } else {
      IO_ds1302 = 0;
    }
    SCK_ds1302 = 1;
    SCK_ds1302 = 0;
    addr = addr >> 1;
  }

#endif
  //输出数据：temp
  GLOBAL_IT_CLOSE();
  DS1302_RST = 1;
  for (i = 0; i < 8; i++) {
    temp = temp >> 1;
    if (DS1302_DAT) {
      temp |= 0x80;
    } else {
      temp &= 0x7F;
    }
    DS1302_CLK = 1;
    DS1302_CLK = 0;
  }
  DS1302_RST = 0;  //停止DS1302总线
  GLOBAL_IT_OPEN();
  return temp;
}

// DS1302初始化函数
void ds1302_init(void) {
  DS1302_RST = 0;  // RST脚置低
  DS1302_CLK = 0;  // SCK脚置低
}
#if USE_ds1302_write_time

//向DS1302写入一字节数据
static void ds1302_write_byte(uint8 addr, uint8 d) {
  DS1302_RST = 1;  //启动DS1302总线
  //写入目标地址：addr
  addr = addr & 0xFE;  //最低位置零，寄存器0位为0时写，为1时读
  GLOBAL_IT_CLOSE();
#if 1
  ds1302_SPI_SendByte(addr);
  ds1302_SPI_SendByte(d);
#else
  uint8 i;
  for (i = 0; i < 8; i++) {
    if (addr & 0x01) {
      IO_ds1302 = 1;
    } else {
      IO_ds1302 = 0;
    }
    SCK_ds1302 = 1;  //产生时钟
    SCK_ds1302 = 0;
    addr = addr >> 1;
  }

  //写入数据：d
  for (i = 0; i < 8; i++) {
    if (d & 0x01) {
      IO_ds1302 = 1;
    } else {
      IO_ds1302 = 0;
    }
    SCK_ds1302 = 1;  //产生时钟
    SCK_ds1302 = 0;
    d = d >> 1;
  }

#endif
  DS1302_RST = 0;  //停止DS1302总线
  GLOBAL_IT_OPEN();
}

//向DS302写入时钟数据
void ds1302_write_time(DS1302_union *buf) {
  ds1302_write_byte(ds1302_control_add, 0x00);  //关闭写保护
  ds1302_write_byte(ds1302_sec_add, 0x80);      //暂停时钟

  ds1302_write_byte(ds1302_year_add, buf->name.year_l);  //年
  ds1302_write_byte(ds1302_month_add, buf->name.month);  //月
  ds1302_write_byte(ds1302_date_add, buf->name.day);     //日
  ds1302_write_byte(ds1302_hr_add, buf->name.hour);      //时
  ds1302_write_byte(ds1302_min_add, buf->name.minute);   //分
  ds1302_write_byte(ds1302_sec_add, buf->name.second);   //秒
  ds1302_write_byte(ds1302_day_add, buf->name.week);     //周
  ds1302_write_byte(ds1302_control_add, 0x80);           //打开写保护
}
#endif
#if USE_ds1302_read_time
//从DS302读出时钟数据
void ds1302_read_time(DS1302_union *buf) {
  buf->name.year_l = ds1302_read_byte(ds1302_year_add);  //年
  buf->name.month = ds1302_read_byte(ds1302_month_add);  //月
  buf->name.day = ds1302_read_byte(ds1302_date_add);     //日
  buf->name.hour = ds1302_read_byte(ds1302_hr_add);      //时
  buf->name.minute = ds1302_read_byte(ds1302_min_add);   //分
  buf->name.second = (ds1302_read_byte(ds1302_sec_add)) &
                     0x7f;  //秒，屏蔽秒的第7位，避免超出59
  buf->name.week = ds1302_read_byte(ds1302_day_add);  //周
}
#endif

```

### 使用案例

```c
DS1302_union ds1302_buf = {0x20, 0x22, 0x4, 0x20, 0x17, 0x41, 0x30, 0x3};
ds1302_write_time(&ds1302_buf);
ds1302_read_time(&ds1302_buf);
```

### 原理图

![image-20220422144549356](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422144549356.png)

## LCD12864串行

片选信号可以悬空，并/串行选择PSB已经焊死，固定串行。若需要以上两个引脚修改宏

### lcd12864_s.h

```c
#ifndef __12864DISPLAY_H_
#define __12864DISPLAY_H_

#include "main.h"
#include "delay.h"

// RS = CS  RW = SID_lcd  E = SCLK_lcd

#define USEPSB 0  // PSB接地就不需要该引脚了
#define USECS 0

#define USE_MACRO 1
#define USE_lcd12864_DDRAM 1
#define USE_lcd12864_DDRAM_Flush 1
#define USE_lcd12864_GDRAM 0
#define USE_lcd12864_HCDisp 0
#define USE_lcd12864_HCDisp2 1
extern uint8_t code DDRAM_POS[8];
/*****LCD接口定义*****/
#ifndef __MAIN_H_
sbit SID_lcd = P4 ^ 0;   //串行数据
sbit SCLK_lcd = P4 ^ 1;  //串行同步时钟
#endif
#if USEPSB

sbit PSB = P2 ^ 3;  //串并口选择 1:并口模式; 0:串口模式;
#endif
#if USECS
sbit CS = P4 ^ 0;  //片选信号
#endif

/*****LCD功能初始化指令*****/
#define CLEAR_SCREEN 0x01  //清屏指令：清屏且AC值为00H
#define AC_INIT 0x02       //将AC设置为00H。且游标移到原点位置
#define CURSE_ADD \
  0x06  //设定游标移到方向及图像整体移动方向（默认游标右移，图像整体不动）
#define FUN_MODE 0x30     //工作模式：8位基本指令集
#define DISPLAY_ON 0x0c   //显示开,显示游标，且游标位置反白
#define DISPLAY_OFF 0x08  //显示关
#define CURSE_DIR 0x14    //游标向右移动:AC=AC+1
#define SET_CG_AC 0x40    //设置AC，范围为：00H~3FH
#define SET_DD_AC 0x80

#define LCDOPT_CMD 0
#define LCDOPT_DATA 1
/*对外函数与宏函数*/
void lcd12864_Init();      //初始化
void lcd12864_Wait(void);  //等待lcd12864退出busy状态
void lcd12864_WriteOpt(uint8 option,
                       uint8 byte);  //发送信息 指令：opt=0 数据：opt=1

#if USE_MACRO
#define lcd12864_WriteCmd(byte) lcd12864_WriteOpt(LCDOPT_CMD, byte)  //写指令
#define lcd12864_WriteData(byte) lcd12864_WriteOpt(LCDOPT_DATA, byte)  //写数据
#else
void lcd12864_WriteCmd(uint8_t Cbyte);   //写指令
void lcd12864_WriteData(uint8_t Dbyte);  //写数据
#endif

#define lcd12864_SetAC(x, y) lcd12864_WriteCmd(DDRAM_POS[y] + x)
#define lcd12864_CursorOn() lcd12864_WriteCmd(0x0f)
#define lcd12864_CursorOff() lcd12864_WriteCmd(0x0c)
#define lcd12864_CursorACC() lcd12864_WriteCmd(CURSE_DIR)
void lcd12864_DDRAM(uint8_t x, uint8_t y, uint8_t *c);
void lcd12864_DDRAM_plus(uint8_t x, uint8_t y, uint8_t *c, uint8 len);
void lcd12864_DDRAM_Flush(uint8_t xdata *c);  //仅仅适用于Xdata区的字符串，64B缓存对于128B的data消耗资源太多
void lcd12864_GDRAM(uint8_t *pic);
void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len,
                      char pos_begin1, char pos_end1, char pos_begin2,
                      char pos_end2,unsigned int ms);  //横向滚动显示一段字符
#endif
```

### lcd12864_s.c

```c
#include "lcd12864_s.h"

uint8_t code DDRAM_POS[8] = {0x80, 0x90, 0x88, 0x98, 0xA0,
                             0xB0, 0xA8, 0xB8};  //一二三四行对应的开头坐标

/*****串行发送一个字节*****/
static void SendByte(uint8_t Dbyte) {
  uint8_t i;
  GLOBAL_IT_CLOSE();
  for (i = 0; i < 8; i++) {
    LCD_SCLK = 0;
    Dbyte <<= 1;   //左移一位
    LCD_SID = CY;  //溢出寄存器
    LCD_SCLK = 1;
    LCD_SCLK = 0;
  }
  GLOBAL_IT_OPEN();
}

/*****串行接收一个字节*****/
static uint8_t ReceiveByte(void) {
  uint8_t i, temp1, temp2;
  temp1 = 0;
  temp2 = 0;
  GLOBAL_IT_CLOSE();
  for (i = 0; i < 8; i++) {
    temp1 = temp1 << 1;
    LCD_SCLK = 0;
    LCD_SCLK = 1;
    LCD_SCLK = 0;
    if (LCD_SID) temp1++;
  }
  for (i = 0; i < 8; i++) {
    temp2 = temp2 << 1;
    LCD_SCLK = 0;
    LCD_SCLK = 1;
    LCD_SCLK = 0;
    if (LCD_SID) temp2++;
  }
  GLOBAL_IT_OPEN();
  return ((0xf0 & temp1) + (0x0f & temp2));
}

/*****检测LCD忙状态*****/
void lcd12864_Wait(void) {
  do SendByte(0xfc);             // 11111,RW(1),RS(0),0
  while (0x80 & ReceiveByte());  // BF(.7)=1 Busy
}

void lcd12864_WriteOpt(uint8 option, uint8 byte) {
#if USECS
  CS = 1;
#endif
  lcd12864_Wait();
  SendByte(option ? 0xfa : 0xf8);  // 11111,RW(0),RS(0),0
  SendByte(0xf0 & byte);           //高四位
  SendByte(0xf0 & byte << 4);      //低四位(先执行<<)
#if USECS
  CS = 0;
#endif
}

#if !USE_MACRO
/*****写指令*****/

void lcd12864_WriteCmd(uint8_t Cbyte) {
#if USECS
  CS = 1;
#endif
  lcd12864_Wait();
  SendByte(0xf8);               // 11111,RW(0),RS(0),0
  SendByte(0xf0 & Cbyte);       //高四位
  SendByte(0xf0 & Cbyte << 4);  //低四位(先执行<<)
#if USECS
  CS = 0;
#endif
}

/*****写数据*****/
void lcd12864_WriteData(uint8_t Dbyte) {
#if USECS
  CS = 1;
#endif
  lcd12864_Wait();
  SendByte(0xfa);               // 11111,RW(0),RS(1),0
  SendByte(0xf0 & Dbyte);       //高四位
  SendByte(0xf0 & Dbyte << 4);  //低四位(先执行<<)
#if USECS
  CS = 0;
#endif
}
#endif

/*****初始化LCD*****/
void lcd12864_Init(void) {
#if USEPSB
  PSB = 0;
#endif
  delayms(50);
  lcd12864_WriteCmd(0x34);  //选择拓展指令集
  delayms(1);
  lcd12864_WriteCmd(0x30);  //选择8bit数据流
  delayms(1);
  lcd12864_WriteCmd(0x0c);  //开显示(无游标、不反白)
  delayms(1);
  lcd12864_WriteCmd(0x06);  //设置游标运动方向
  delayms(1);
  lcd12864_WriteCmd(0x01);  //清除显示，并且设定地址指针为00H
  delayms(20);
  lcd12864_WriteCmd(0x06);  //清除显示，并且设定地址指针为00H
}

/*****显示字符串*****/
#if USE_lcd12864_DDRAM
void lcd12864_DDRAM(uint8_t x, uint8_t y, uint8_t *c) {
  if (x < 7 && y < 8)  //防止溢出
  {
    lcd12864_WriteCmd(DDRAM_POS[y] + x);
    while (*c) {
      lcd12864_WriteData(*c);
      c++;
    }
  }
}
#endif

void lcd12864_DDRAM_plus(uint8_t x, uint8_t y, uint8_t *c, uint8 len) {
  data uint8_t i;
  if (x < 7 && y < 8)  //防止溢出
  {
    lcd12864_WriteCmd(DDRAM_POS[y] + x);
    i = 0;
    while (i < len) {
      lcd12864_WriteData(c[i]);
      i++;
    }
  }
}

#if USE_lcd12864_DDRAM_Flush
void lcd12864_DDRAM_Flush(uint8_t xdata *c) {
  uint8_t i, j;
  uint8_t code jstep[] = {0, 34, 17, 51};
  lcd12864_WriteCmd(0x80);
  for (j = 0; j < 4; j++) {
    for (i = 0; i < 16; i++) {
      lcd12864_WriteData(c[jstep[j] + i] ? c[jstep[j] + i]
                                         : ' ');  //'\0'用空格' '代替
    }
  }
}
#endif
/*****显示图片（显示Gdram内容）*****/

/*绘图显示的步骤
1 关闭绘图显示功能
2 先将垂直的坐标(Y)写入绘图RAM地址
3 再将水平的位元组坐标(X)写入绘图RAM地址
4 将D15－D8写入RAM中
5 将D7－D0写入到RAM中
6 打开绘图显示功能*/

//图片取模方式：横向取模，字节正序
#if USE_lcd12864_GDRAM
void lcd12864_GDRAM(uint8_t *pic)  //显示Gdram内容（显示图片）
{
  uint8_t x, y, i;
  for (i = 0; i < 9; i = i + 8)
    for (y = 0; y < 32; y++) {
      for (x = 0; x < 8; x++) {
        lcd12864_WriteCmd(0x36);          //扩充指令，开绘图显示
        lcd12864_WriteCmd(0x80 + y);      //行地址
        lcd12864_WriteCmd(0x80 + x + i);  //列地址
        lcd12864_WriteData(*pic++);       //写数据 D15－D8
        lcd12864_WriteData(*pic++);       //写数据 D7－D0
        lcd12864_WriteCmd(0x30);
      }
    }
}
#endif

#include "string.h"

#if USE_lcd12864_HCDisp
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
void lcd12864_HCDisp(unsigned char *buf, char *str, char len, char pos_begin,
                     char pos_end,
                     unsigned int ms)  //横向滚动显示一段字符
{
  char i, j;

  for (; pos_begin != pos_end; pos_begin += (pos_begin > pos_end ? -1 : 1)) {
    memset(buf, ' ', 16);
    buf[16] = 0;
    for (i = pos_begin, j = 0; (i < pos_begin + 16) && (j < len); i++, j++) {
      if (i >= 0 && i < 16) {
        buf[i] = str[j];
      }
    }
    lcd12864_DDRAM(0, 1, buf);
    Delayms(ms);
  }
}
#endif

#if USE_lcd12864_HCDisp2
void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len,
                      char pos_begin1, char pos_end1, char pos_begin2,
                      char pos_end2,
                      unsigned int ms)  //横向滚动显示一段字符
{
  char i, j;
  pos_end1 += (pos_begin1 > pos_end1 ? -1 : 1);
  pos_end2 += (pos_begin2 > pos_end2 ? -1 : 1);
  while (pos_begin1 != pos_end1 || pos_begin2 != pos_end2) {
    if (pos_begin1 != pos_end1) {
      memset(buf, ' ', 16);

      for (i = pos_begin1, j = 0; (i < pos_begin1 + 16) && (j < len);
           i++, j++) {
        if (i >= 0 && i < 16) {
          buf[i] = str1[j];
        }
      }
      buf[16] = 0;
      lcd12864_DDRAM(0, 1, buf);
      pos_begin1 += (pos_begin1 > pos_end1 ? -1 : 1);
    }

    Delayms(ms / 2);
    if (pos_begin2 != pos_end2) {
      memset(buf, ' ', 16);
      buf[16] = 0;
      for (i = pos_begin2, j = 0; (i < pos_begin2 + 16) && (j < len);
           i++, j++) {
        if (i >= 0 && i < 16) {
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
```

### 原理图

![image-20220422145128695](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422145128695.png)

### 其余指令

![image-20220422170447244](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422170447244.png)

![image-20220422170506996](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422170506996.png)

![image-20220422170517079](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422170517079.png)



## 矩阵键盘

### matkey.h

```c
#ifndef __MATKEEY_H_
#define __MATKEEY_H_

#include "main.h"

#ifndef __MAIN_H_
#define KeyBoard_Port P0
#endif

uint8 KeyScan(void);
void KeyInput(uint8 *buf, uint8 len, uint8 beginKey, uint8 endKey, uint8 poxX,
              uint8 poxY);  //输入一段字符到内存并动态显示
#endif
```

### matkey.h

`KeyInput()`函数调用了"lcd12864_s.h"中的函数来显示输入的字符

```c
#include "matkey.h"
#include "delay.h"
#include "lcd12864_s.h"
#include "string.h"
#include "stdio.h"

uint8 KeyScan(void) {
  uint8 KeyValue = 0;

  KeyBoard_Port = 0xF0;
  delayms(1);

  /*消抖*/
  if (KeyBoard_Port == 0xF0) return 0;
  Delayms(5);
  if (KeyBoard_Port == 0xF0) return 0;

  //测试行
  switch (KeyBoard_Port) {
    case (0X70):
      KeyValue += 0;
      break;
    case (0Xb0):
      KeyValue += 8;
      break;
    case (0Xd0):
      KeyValue += 4;
      break;
    case (0Xe0):
      KeyValue += 12;
      break;
  }
  //测试列
  KeyBoard_Port = 0X0F;
  delayms(2);
  switch (KeyBoard_Port) {
    case (0X07):
      KeyValue += 4;
      break;
    case (0X0b):
      KeyValue += 3;
      break;
    case (0X0d):
      KeyValue += 2;
      break;
    case (0X0e):
      KeyValue += 1;
      break;
  }
  KeyBoard_Port = 0xF0;

  return KeyValue;
}

void KeyInput(uint8 *buf, uint8 len, uint8 beginKey, uint8 endKey, uint8 poxX,
              uint8 poxY) {
  uint8 code cov[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                      '8', '9', '0', 'b', 'c', 'd', 'e', 'f'};  //按键 => 字符

  data uint8 key = 0;  //记录按键

  data uint8 i = 0;
  memset(buf, ' ', len + 1);                  //初始化内存
  lcd12864_DDRAM_plus(poxX, poxY, buf, len);  //清除输入区域
  lcd12864_SetAC(poxX, poxY);                 //设置光标位置
  lcd12864_CursorOn();                        //显示光标

  while (beginKey == KeyScan())  //等待触发按键松开
    ;

  while (i < len) {
    while (key == KeyScan()) {  //等待按键变化
    }

    key = KeyScan();         //读入新的按键
    if (key == 0) continue;  //未按下，下一个循环
    if (key == endKey) {     //触发结束按键，退出函数
      while (endKey == KeyScan())
        ;
      return;
    }
    buf[i] = cov[key];           //忘内存中写入按键
    lcd12864_WriteData(buf[i]);  //在显示屏显示新输入的按键
    i++;
  }
  lcd12864_CursorOff();  //关闭显示光标
}
```

### 使用案例

```c
uint8_t xdata ScreenBuf[];
keyValue = KeyScan();
KeyInput(ScreenBuf + 12, 3, 15, 16, 6, 0);
```

### 原理图

![image-20220422144434127](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422144434127.png)

## I2C总线

### i2c.h

```c
#ifndef _I2C_H_
#define _I2C_H_

#include "main.h"

#ifndef __MAIN_H_
sbit SCL = P1 ^ 3;  // I2C  时钟
sbit SDA = P1 ^ 4;  // I2C  数据
#endif
extern bit ack;

void Start_I2c();                //起动总线函数
void Stop_I2c();                 //结束总线函数
void Ack_I2c(bit a);             //应答子函数
void SendByte(unsigned char c);  //字节数据发送函数
bit ISendStr(unsigned char sla, unsigned char suba, unsigned char *s,unsigned char no);  //有子地址发送多字节数据函数
bit ISendStrExt(unsigned char sla, unsigned char *s, unsigned char no);  //无子地址发送多字节数据函数
unsigned char RcvByte();            //无子地址读字节数据函数

#endif
```

### i2c.c

```c

#include "i2c.h"

#define NOP() _nop_()  /* 定义空指令 */
#define _Nop() _nop_() /*定义空指令*/

bit ack; /*应答标志位*/

/**
 * @brief 启动I2C总线
 */
void Start_I2c() {
  GLOBAL_IT_CLOSE();
  SDA = 1; /*发送起始条件的数据信号*/
  _Nop();
  SCL = 1;
  _Nop(); /*起始条件建立时间大于4.7us,延时*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SDA = 0; /*发送起始信号*/
  _Nop();  /* 起始条件锁定时间大于4μs*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SCL = 0; /*钳住I2C总线，准备发送或接收数据 */
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}

/**
 * @brief  结束I2C总线,
 */
void Stop_I2c() {
  GLOBAL_IT_CLOSE();
  SDA = 0; /*发送结束条件的数据信号*/
  _Nop();  /*发送结束条件的时钟信号*/
  SCL = 1; /*结束条件建立时间大于4μs*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SDA = 1; /*发送I2C总线结束信号*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}

/**
 * @brief 字节数据发送函数，发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
 * @param c 待发送字节
 */
void SendByte(unsigned char c) {
  unsigned char BitCnt;
  GLOBAL_IT_CLOSE();
  for (BitCnt = 0; BitCnt < 8; BitCnt++) /*要传送的数据长度为8位*/
  {
    if ((c << BitCnt) & 0x80)
      SDA = 1; /*判断发送位*/
    else
      SDA = 0;
    _Nop();
    SCL = 1; /*置时钟线为高，通知被控器开始接收数据位*/
    _Nop();
    _Nop(); /*保证时钟高电平周期大于4μs*/
    _Nop();
    _Nop();
    _Nop();
    SCL = 0;
  }

  _Nop();
  _Nop();
  SDA = 1; /*8位发送完后释放数据线，准备接收应答位*/
  _Nop();
  _Nop();
  SCL = 1;
  _Nop();
  _Nop();
  _Nop();
  if (SDA == 1)
    ack = 0;
  else
    ack = 1; /*判断是否接收到应答信号*/
  SCL = 0;
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}

/**
 * @brief  字节数据接收函数
 *
 * @return unsigned char
 */
unsigned char RcvByte() {
  unsigned char retc;
  unsigned char BitCnt;

  retc = 0;
  SDA = 1; /*置数据线为输入方式*/

  GLOBAL_IT_CLOSE();
  for (BitCnt = 0; BitCnt < 8; BitCnt++) {
    _Nop();
    SCL = 0; /*置时钟线为低，准备接收数据位*/
    _Nop();
    _Nop(); /*时钟低电平周期大于4.7μs*/
    _Nop();
    _Nop();
    _Nop();
    SCL = 1; /*置时钟线为高使数据线上数据有效*/
    _Nop();
    _Nop();
    retc = retc << 1;
    if (SDA == 1) retc = retc + 1; /*读数据位,接收的数据位放入retc中 */
    _Nop();
    _Nop();
  }
  SCL = 0;
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
  return (retc);
}

/**
 * @brief 主控器进行应答信号
 *
 * @param a
 */
void Ack_I2c(bit a) {
  GLOBAL_IT_CLOSE();

  if (a == 0)
    SDA = 0; /*在此发出应答或非应答信号 */
  else
    SDA = 1;
  _Nop();
  _Nop();
  _Nop();
  SCL = 1;
  _Nop();
  _Nop(); /*时钟低电平周期大于4μs*/
  _Nop();
  _Nop();
  _Nop();
  SCL = 0; /*清时钟线，钳住I2C总线以便继续接收*/
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}

```

## ADC PCF8591(I2C通讯)

该模块基于上文的i2c总线通讯。

### pcf8591.h

```c
#ifndef __PCF8591_H_
#define __PCF8591_H_
#include "main.h"
#define USE_AD 1
#define USE_DA 0
#define PCF8591 0x90  // PCF8591 地址

#define ReadAD(x, p) \
  ISendByte(PCF8591, 0x40 + x), (p) = IRcvByte(PCF8591)  //读取AD P通道的值
#define SetDA(x) DACconversion(PCF8591, 0x40, x)         //输出DA
#define DA_Res(p) ReadAD(3, p)                           //读取电位器电位
#define DA_Light(p) ReadAD(0, p)  //读取光敏电阻电位
#define DA_Temp(p) ReadAD(1, p)   //读取热敏电阻电位

bit DACconversion(unsigned char sla, unsigned char c, unsigned char Val);
bit ISendByte(unsigned char sla, unsigned char c);

#endif
```

### pcf8591.c

```c
#include "pcf8591.h"
#include "i2c.h"

#if USE_AD
/*******************************************************************
ADC读字节数据函数
*******************************************************************/
unsigned char IRcvByte(unsigned char sla)
{
    unsigned char c;

    Start_I2c();       //启动总线
    SendByte(sla + 1); //发送器件地址
    if (ack == 0)
        return (0);
    c = RcvByte(); //读取数据0

    Ack_I2c(1); //发送非就答位
    Stop_I2c(); //结束总线
    return (c);
}
/*******************************************************************
ADC发送字节[命令]数据函数
*******************************************************************/
bit ISendByte(unsigned char sla, unsigned char c)
{
    Start_I2c();   //启动总线
    SendByte(sla); //发送器件地址
    if (ack == 0)
        return (0);
    SendByte(c); //发送数据
    if (ack == 0)
        return (0);
    Stop_I2c(); //结束总线
    return (1);
}
#endif

#if USE_DA

/*******************************************************************
DAC 变换, 转化函数
*******************************************************************/
bit DACconversion(unsigned char sla, unsigned char c, unsigned char Val)
{
    Start_I2c();   //启动总线
    SendByte(sla); //发送器件地址
    if (ack == 0)
        return (0);
    SendByte(c); //发送控制字节
    if (ack == 0)
        return (0);
    SendByte(Val); //发送DAC的数值
    if (ack == 0)
        return (0);
    Stop_I2c(); //结束总线
    return (1);
}

#endif
```

### 原理图

![image-20220422144017790](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422144017790.png)



## 超声波HC-SR04

该模块使用了定时器0计时，使用Echo、Trig两个引脚。该模块量程4m，故使用16位整数记录，单位mm。

### hcsr04.h

```c
#ifndef __HCSR04_H_
#define __HCSR04_H_

#include "main.h"

#ifndef __MAIN_H_
sbit Echo = P1 ^ 5;
sbit Trig = P1 ^ 6;
#endif
void HCSR04_Init();       //初始化
uint16 HCSR04_GetDist();  //返回距离，单位mm

#endif  // !__HCSR04_H_

```

### hcsr04.c

```c
#include "hcsr04.h"
#include "delay.h"

static uint32 hcsr0_Count = 0;

void zd0() interrupt 1  // T0中断用来计数器溢出,超过测距范围
{
  TL0 = 0x00;  //设置定时初始值
  TH0 = 0x00;  //设置定时初始值
  hcsr0_Count += 0xffff;
}

void StartModule()  // T1中断用来扫描数码管和计800MS启动模块
{
  Trig = 1;  // 800MS  启动一次模块
  Delayus(20);
  Trig = 0;
}

void HCSR04_Init() {
  AUXR |= 0x80;  //定时器时钟1T模式
  TMOD &= 0xF0;  //设置定时器模式
  TMOD |= 0x01;  //设置定时器模式
  TL0 = 0x00;    //设置定时初始值
  TH0 = 0x00;    //设置定时初始值
  TF0 = 0;       //清除TF0标志

  TR0 = 0;  //定时器0开始计时
  ET0 = 1;
  EA = 1;  //开启总中断
}

uint16 HCSR04_GetDist() {
  data uint32 timeout = 0;
  TH0 = 0;
  TL0 = 0;
  hcsr0_Count = 0;
  StartModule();
  while (!Echo)  //当RX为零时等待
  {
    if (timeout++ > 5000000) {
      return 0xffff;
    }
  }

  TR0 = 1;  //开启计数
  while (Echo && hcsr0_Count < 0xffffff)
    ;       //当RX为1计数并等待
  TR0 = 0;  //关闭计数
  if (hcsr0_Count < 0xffff00) {
    hcsr0_Count += TH0 * 256 + TL0;
    hcsr0_Count /= 65;
  }

  return hcsr0_Count;
}

```

### 原理图

![image-20220422151242060](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422151242060.png)

## 数码管 双74HC595串联轮流显示

两个74HC595串联，并行输出分别接数码管段选和位选，需要快速轮流显示，熄灭时间应小于20ms

### NixieTube_74hc595.h

```c
#ifndef __74HC595_H_
#define __74HC595_H_

#include "main.h"

#ifndef __MAIN_H_
sbit HC595_DIO = P1 ^ 0;   //串行数据输入
sbit HC595_RCLK = P1 ^ 1;  //时钟脉冲信号――上升沿有效
sbit HC595_SCLK = P1 ^ 2;  //打入信号――――上升沿有效
#endif

void LED4_Display(uint8 *c);  // LED显示
void LED4_DisOne(uint8 *c);
#endif
```

### NixieTube_74hc595.c

```c
#include "NixieTube_74hc595.h"

unsigned char code LED_0F[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92,
                               0x82, 0xF8, 0x80, 0x90, 0x8C, 0xBF,
                               0xC6, 0xA1, 0x86, 0xFF, 0xbf};

static void LED_OUT(uint8 X) {
  uint8 i;
  for (i = 0; i < 8; i++) {
    X <<= 1;
    HC595_DIO = CY;  //溢出寄存器
    HC595_SCLK = 0;
    HC595_SCLK = 1;
  }
}

void LED4_Display(uint8 *c) {
  uint8 i;

  for (i = 0; i < 4; i++) {
    LED_OUT(LED_0F[c[i]]);
    LED_OUT(0x01 << i);
    HC595_RCLK = 0;
    HC595_RCLK = 1;
  }
}
/**
 * @brief 显示一位，轮流显示，建议中断调用
 * @param c 需要显示的数据，四位
 */
void LED4_DisOne(uint8 *c) {
  static uint8 i = 0;
  HC595_RCLK = 1;
  i = ++i & 0x03;
  LED_OUT(LED_0F[c[i]]);
  LED_OUT(0x01 << i);
  HC595_RCLK = 0;
}
```

### 原理图

![image-20220422150926989](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422150926989.png)

## 光电门测速 计时器+外部中断 双中断

当频率较高时采用500ms计数的方式，当频率较低时采用间隔时间计时的方式。调用SpdOt_GetTimeFreq()函数获取时间和频率时会自动切换模式

### speedometer.h

```c
#ifndef __SPEEDOMETER_H_
#define __SPEEDOMETER_H_

#include "main.h"

#define SpdOt_MAX_LEN 10
#define TH5MS 0x28
#define TL5MS 0x00
extern uint16 SpdOt_tempTime;

void SpdOt_Init();
void SpdOt_GetTimeFreq(uint16 *Time, float *Freq);

#define SpdOt_CLOSE_IT() EX0 = 0, ET1 = 0
#define SpdOt_OPEN_IT() EX0 = 1, ET1 = 1

#define SpdOt_Start() \
  SpdOt_tempTime = 0, TL1 = TL5MS, TH1 = TH5MS, TR1 = 1, ET1 = 1
#define SpdOt_Stop() \
  SpdOt_tempTime = 0, TL1 = TL5MS, TH1 = TH5MS, TR1 = 0, ET1 = 0

#endif
```

### speedometer.c

```c
#include "delay.h"
#include "string.h"
#include "speedometer.h"
#include "NixieTube_74hc595.h"

uint16 SpdOt_countTime[SpdOt_MAX_LEN];
uint16 SpdOt_tempTime = 0;

uint16 SpdOt_countFreq[SpdOt_MAX_LEN];
uint16 SpdOt_tempFreq = 0;

static uint8 SpdOt_index = 0;
static uint8 SpdOt_mode = 0;
void SpdOt_Init()  // 5毫秒@11.0592MHz
{
  memset(SpdOt_countTime, 0xff, sizeof(SpdOt_countTime));
  memset(SpdOt_countFreq, 0x00, sizeof(SpdOt_countFreq));
  SpdOt_tempTime = 0XFFFF;
  SpdOt_index = 0;
  SpdOt_mode = 0;
  IT0 = 1;  // set INT0 int type (1:Falling 0:Low level)
  EX0 = 1;  // enable INT0 interrupt

  AUXR |= 0x40;  //定时器时钟1T模式
  TMOD &= 0x0F;  //设置定时器模式
  TMOD |= 0x10;  //设置定时器模式
  TL1 = 0x00;    //设置定时初始值
  TH1 = 0x28;    //设置定时初始值
  TF1 = 0;       //清除TF1标志
  TR1 = 1;       //定时器1开始计时

  ET1 = 1;
  EA = 1;  //开启总中断
}

/* Timer0 interrupt routine */
void tm1_isr() interrupt 3 {
  static uint8 count;

  TL1 = 0x00;  // reload timer1 low byte
  TH1 = 0x28;  // reload timer1 high byte

  if (SpdOt_mode) {
    if (++count > 100)  //累计500ms
    {
      if (++SpdOt_index >= SpdOt_MAX_LEN) {
        SpdOt_index = 0;
      }
      SpdOt_countFreq[SpdOt_index] = SpdOt_tempFreq << 1;
      SpdOt_tempFreq = 0;
      count = 0;
    }
  } else {
    if (SpdOt_tempTime < 0xfff0) {
      SpdOt_tempTime += 5;
    }

    if (SpdOt_tempTime >= 0x7FFF ||
        ((SpdOt_tempTime >> 1) > SpdOt_countTime[SpdOt_index] &&
         SpdOt_countTime[SpdOt_index] < 0x7FFF)) {
      if (++SpdOt_index >= SpdOt_MAX_LEN) {
        SpdOt_index = 0;
      }
      SpdOt_countTime[SpdOt_index] = SpdOt_tempTime;
    }
  }

  //  LED4_DisOne(LED);
  LED4_DisOne(LED);
}

// External interrupt0 service routine
void exint0() interrupt 0  //(location at 0003H)
{
  EX0 = 0;
  if (SpdOt_mode) {
    SpdOt_tempFreq++;
  } else {
    if (++SpdOt_index >= SpdOt_MAX_LEN) {
      SpdOt_index = 0;
    }
    SpdOt_countTime[SpdOt_index] = SpdOt_tempTime;
    SpdOt_tempTime = 0;
  }

  EX0 = 1;
}

void SpdOt_GetTimeFreq(uint16 *Time, float *Freq) {
  data uint8 i;
  uint32 sum = 0;
  if (SpdOt_mode) {
    for (i = 0; i < SpdOt_MAX_LEN; i++) {
      sum += SpdOt_countFreq[i];
      SpdOt_countTime[i] = 3000 / SpdOt_countFreq[i];
    }
    *Freq = (float)sum / (float)SpdOt_MAX_LEN;
    *Time = 3000 * SpdOt_MAX_LEN / sum;
  } else {
    for (i = 0; i < SpdOt_MAX_LEN; i++) {
      sum += SpdOt_countTime[i];
      SpdOt_countFreq[i] = 333 / SpdOt_countTime[i];
    }
    sum /= SpdOt_MAX_LEN;
    *Freq = 333.f / (float)sum;
    *Time = sum;
  }
  SpdOt_mode = *Time < 111;  //设置模式
}

```

### 原理图

![image-20220422152022784](F:\Project\C51_Project\STC12C5A60S2单片机 KEIL环境 常用模块驱动.assets\image-20220422152022784.png)

# main.c

