#ifndef __HCSR04_H_
#define __HCSR04_H_

#include "main.h"

#ifndef __MAIN_H_
#definr Echo P15
#definr Trig P16
#endif

void HCSR04_Init();      //初始化
uint16 HCSR04_GetDist(); //返回距离，单位mm

#endif // !__HCSR04_H_
