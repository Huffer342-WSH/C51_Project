#include "main.h"
#include "delay.h"
#include "stepMotor.h"
#include "usart.h"

void main(void)
{

    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
   // P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO
    tm0_init();
    Uart_Init();

    /*=============初始化=============*/

    /*=============启动动画=============*/

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

    /*=============主循环=============*/
}
