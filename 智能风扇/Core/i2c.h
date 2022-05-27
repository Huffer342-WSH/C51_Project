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
bit ISendStr(unsigned char sla, unsigned char suba, unsigned char *s,
             unsigned char no);  //有子地址发送多字节数据函数
bit ISendStrExt(unsigned char sla, unsigned char *s,
                unsigned char no);  //无子地址发送多字节数据函数
unsigned char RcvByte();            //无子地址读字节数据函数

#endif