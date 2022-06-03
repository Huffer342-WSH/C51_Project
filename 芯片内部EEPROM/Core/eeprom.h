/*============= encoding: Simplified Chinese GB2312 =============*/
#ifndef __EEPROM_H_
#define __EEPROM_H_

#include "main.h"

#ifndef __STC12C5A60S2_H_
/*Declare SFR associated with the IAP */
sfr IAP_DATA = 0xC2;  // Flash data register
sfr IAP_ADDRH = 0xC3; // Flash address HIGH
sfr IAP_ADDRL = 0xC4; // Flash address LOW
sfr IAP_CMD = 0xC5;   // Flash command register
sfr IAP_TRIG = 0xC6;  // Flash command trigger
sfr IAP_CONTR = 0xC7; // Flash control register
#endif

/*Define ISP/IAP/EEPROM command*/
#define CMD_IDLE 0    // Stand-By
#define CMD_READ 1    // Byte-Read
#define CMD_PROGRAM 2 // Byte-Program
#define CMD_ERASE 3   // Sector-Erase

/*Define ISP/IAP/EEPROM operation const for IAP_CONTR*/
//#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
// #define ENABLE_IAP 0x82  // if SYSCLK<20MHz
#define ENABLE_IAP 0x83 // if SYSCLK<12MHz
//#define ENABLE_IAP 0x84           //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85           //if SYSCLK<3MHz
//#define ENABLE_IAP 0x86           //if SYSCLK<2MHz
//#define ENABLE_IAP 0x87           //if SYSCLK<1MHz

// STC12C60S2 EEPROM 起始地址
#define SECTOR0_ADDRESS 0x0400
#define SECTOR1_ADDRESS 0x0400
uint8 IapReadByte(uint16 addr);                            //读一个字节
void IapProgramByte(uint16 addr, uint8 dat);               //写一个字节
void IapEraseSector(uint16 addr);                          //擦除一个扇区
void eeprom_WriteData(uint16 addr, void *dat, uint16 len); //写一串数据
void eeprom_ReadData(uint16 addr, void *dat, uint16 len);  //读一串数据
#endif