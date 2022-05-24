#include "main.h"
#include "usart.h"
#include "Delay.h"
#include "KeyBoard.h"
void main(void){
    char str[3]={0};
    Uart_Init();
#include <STC12C5A60S2.H>
    P1 = 0xF0;
    while(1){
        SendData(0x77);
        Delayms(500);
        SendData(P1);
        
        
    }
    
    
}

