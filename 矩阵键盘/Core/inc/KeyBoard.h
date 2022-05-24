#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include "main.h"

#define GPIO_KEY P1

unsigned char KeyScan(void);
void KeyInput(uint8 *buf, uint8 len, uint8 endKey);

#endif