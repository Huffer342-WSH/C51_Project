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
unsigned char IRcvByte(unsigned char sla);

#endif