#include "main.h"
#include "delay.h"
#include "stepMotor.h"
#include "usart.h"

void main(void)
{

    /*-------------------��ʼ��---------------------------------*/
    P4SW = 0xff;              // P4�˿ھ���IO��P4.7���⣩
   // P0M0 = 0x00, P0M1 = 0x00; // P0©��������˫��IO
    tm0_init();
    Uart_Init();

    /*=============��ʼ��=============*/

    /*=============��������=============*/

    while (1)
    {
        // SetPos(4000);
        // delayms(4000);
        // SetPos(0);
        // delayms(4000);
        SM_move(4000);
        delayms(500);
        SM_move(-4000);
        delayms(500);
        printf("pos: %ld  %ld \r\n", GetPos(), POS_aim);
    }

    /*=============��ѭ��=============*/
}
