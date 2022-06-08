#ifndef __USART_H_
#define __USART_H_

#include "main.h"

#define USE_SendData 1
#define USE_SendString 0
#define USE_printf 1

#if USE_printf
extern int printf(const char *, ...);
#endif

#define MAX_BUF 44
extern uint8 Uart1_RxBuf[MAX_BUF];
extern uint8 Rx1len;

void Uart1_Init(void);
void SendData(unsigned char dat);
void SendString(char *s);
void Uart1_Receive(uint8 *pData, uint8 *pLen, uint16 Timeout);

#endif