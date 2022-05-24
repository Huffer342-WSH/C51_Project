#ifndef _I2C_H_
#define _I2C_H_

#include <reg52.h>
#include <intrins.h>

extern bit ack;
//起动总线函数
void Start_I2c();
//结束总线函数
void Stop_I2c();
//应答子函数
void Ack_I2c(bit a);
//字节数据发送函数
void SendByte(unsigned char c);
//有子地址发送多字节数据函数
bit ISendStr(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);
//无子地址发送多字节数据函数
bit ISendStrExt(unsigned char sla, unsigned char *s, unsigned char no);
//无子地址读字节数据函数

unsigned char RcvByte();

#endif