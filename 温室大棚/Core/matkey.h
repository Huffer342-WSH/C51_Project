#ifndef __MATKEEY_H_
#define __MATKEEY_H_

#include "main.h"

#ifndef __MAIN_H_
#define KeyBoard_Port P0
#endif

uint8 KeyScan(void);
void KeyInput(uint8 *buf, uint8 len, uint8 beginKey, uint8 endKey, uint8 poxX,
              uint8 poxY); //输入一段字符到内存并动态显示
#endif