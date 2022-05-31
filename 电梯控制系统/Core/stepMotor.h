#include "main.h"

extern int32 POS_aim, POS_real;

#define ResetPos(x) POS_aim = POS_real = x
#define SetPos(x) POS_aim = x //设定目标位置
#define GetPos() POS_real     //获取当前位置

int32 SM_move(int32 x);                   //移动x步
void SM_MvOneStep();                      //根据目标位置移动一步，定时器调用实现运动到指定位置
void SM_Wait(int32 posMin, int32 posMax); //等待步进电机运动到指定范围

void tm0_init();
