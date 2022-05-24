#ifndef __USART_H_
#define __USART_H_

void Uart_Init(void);
void SendData(unsigned char dat);
void SendString(char *s);

#endif