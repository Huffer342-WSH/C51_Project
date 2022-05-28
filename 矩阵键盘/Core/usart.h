#ifndef __USART_H_
#define __USART_H_

#include "main.h"

#define USE_SendData 1
#define USE_SendString 0
#define USE_printf 1

#if USE_printf
extern int printf(const char *, ...);
#endif

void Uart_Init(void);
void SendData(unsigned char dat);
void SendString(char *s);

#endif