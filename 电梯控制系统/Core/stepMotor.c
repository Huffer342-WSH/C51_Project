#include "stepMotor.h"
#include "delay.h"
// #define IN1 P00
// #define IN2 P01
// #define IN3 P02
// #define IN4 P03

//*============= 全局变量 =============*/
static uint8 code coil[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};
static uint8 phase = 0;

int32 POS_aim = 0, POS_real = 0;

/*============= 私有宏 & 函数 =============*/
#define Coil_Off() IN1 = 0, IN2 = 0, IN3 = 0, IN4 = 0
#define PrevPhase() phase = --phase & 0x07
#define NextPhase() phase = ++phase & 0x07

/**
 * @brief 输出四相
 *
 * @param x
 */
void Coil_Out(uint8 x)
{
    data uint8 i = coil[x];
    x = x & 0x07;
    IN1 = 0x01 & i;
    IN2 = 0x02 & i;
    IN3 = 0x04 & i;
    IN4 = 0x08 & i;
}

//*============= API =============*/
/**
 * @brief 移动x步
 *
 * @param x 步数
 * @return int32
 */
int32 SM_move(int32 x)
{
    TR0 = 0;
    if (x > 0)
    {
        while (x--)
        {
            Coil_Out(NextPhase());
            delayms(1);
        }
    }
    else
    {
        while (x++)
        {
            Coil_Out(PrevPhase());
            delayms(1);
            POS_real--;
            POS_aim--;
        }
    }
    POS_aim = POS_real += x;
    TR0 = 1;
    return POS_real;
}

/**
 * @brief 根据目标位置移动一步，定时器调用实现运动到指定位置
 *
 */
void SM_MvOneStep()
{
    if (POS_aim > POS_real)
    {
        Coil_Out(NextPhase());
        POS_real++;
    }
    else if (POS_aim < POS_real)
    {
        Coil_Out(PrevPhase());
        POS_real--;
    }
    else
    {
        Coil_Off();
    }
}

/**
 * @brief 等待步进电机运动到指定范围
 *        目标位置需要在指定范围内
 *
 * @param posMin 范围下界
 * @param posMax 范围上界
 */
void SM_Wait(int32 posMin, int32 posMax)
{
    if (POS_aim < posMin || POS_aim > posMax)
        return;
    if (posMin > posMax)
        return;
    while (POS_real < posMin || POS_real > posMax)
    {
    }
}

/*============= 定时器案例 =============*/
/* Timer0 interrupt routine */
// void tm0_isr() interrupt 1
// {
//     TL0 = 0xCD; //设置定时初始值
//     TH0 = 0xD4; // reload timer0 high byte

//     SM_MvOneStep();
// }

//-----------------------------------------------

/* main program */
void tm0_init()
{
    AUXR |= 0x80; //定时器时钟1T模式
    TMOD &= 0xF0; //设置定时器模式
    TMOD |= 0x01; //设置定时器模式
    TL0 = 0xCD;   //设置定时初始值
    TH0 = 0xD4;   //设置定时初始值
    TF0 = 0;      //清除TF0标志
    TR0 = 1;      //定时器0开始计时

    ET0 = 1; // enable timer0 interrupt
    EA = 1;  // open global interrupt switch
}
