#ifndef __PWM_H_
#define __PWM_H_
#include "main.h"

void PWM_Init(void);
#define PWM_SetCAP0(x) CCAP0H = CCAP0L = x
#define PWM_SetCAP1(x) CCAP1H = CCAP1L = = x

#define PWM_SetDutyH0(x) CCAP0H = CCAP0L = 0xff - x
#define PWM_SetDutyH1(x) CCAP1H = CCAP1L = 0xff - x
#endif