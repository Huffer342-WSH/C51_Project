#ifndef __HCSR04_H_
#define __HCSR04_H_

#include "main.h"

#ifndef __MAIN_H_
sbit Echo = P1 ^ 5;
sbit Trig = P1 ^ 6;
#endif
void HCSR04_Init();
uint16 HCSR04_GetDist();

#endif // !__HCSR04_H_
