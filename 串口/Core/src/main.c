#include "STC12C5A60S2.h"
#include "intrins.h"
#include "usart.h"
#include "str0.h"
void main()
{
  unsigned int  i;
    Uart_Init();
   
     for (i = 0; i < STC12C5A60S2__________KEIL__________________________txt_len; i++)
    {
        SendData( STC12C5A60S2__________KEIL__________________________txt[i]);
    }
    
    while (1)
        ;
}
