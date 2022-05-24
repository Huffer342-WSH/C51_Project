#ifndef __PCF8591_H_
#define __PCF8591_H_

#define USE_AD 1
#define USE_DA 0
#define PCF8591 0x90 // PCF8591 地址

#define ReadAD(x, p) ISendByte(PCF8591, 0x40 + x), (p) = IRcvByte(PCF8591)
#define SetDA(x) DACconversion(PCF8591, 0x40, x)
#define DA_Res(p) ReadAD(3, p)
#define DA_Light(p) ReadAD(0x40, p)
#define DA_Temp(p) ReadAD(0x41, p)

bit DACconversion(unsigned char sla, unsigned char c, unsigned char Val);
bit ISendByte(unsigned char sla, unsigned char c);
unsigned char IRcvByte(unsigned char sla);

#endif