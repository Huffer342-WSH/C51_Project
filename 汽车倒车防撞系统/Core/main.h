#ifndef __MAIN_H_
#define __MAIN_H_

#include "STC12C5A60S2.h"
#include "intrins.h"

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long
#define uint8_t unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long
#define int8 char
#define int16 int
#define int32 long
#define int8_t char
#define int16_t int
#define int32_t long
#define float32 float

//Òý½Å¶¨Òå
#define LCD_SID P36  // LCD12864´®ÐÐSID
#define LCD_SCLK P34 // LCD12864´®ÐÐSCLK
#define LCD_PSB P37

#define Echo P10 //³¬Éù²¨echo
#define Trig P11 //³¬Éù²¨trig

#define LED P15
#define BEEP P16
// pwm 1.3 1.4
#define KeyBoard_Port P0
#define ST_Default 0
#define ST_CLOCK 1
#define ST_SET 2
#define ST_MENU 3
#define ST_HIS 4

#define FromAD 1
#define FromHC 0

#endif