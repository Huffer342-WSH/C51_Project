#ifndef __SPEEDOMETER_H_
#define __SPEEDOMETER_H_

#include "main.h"

#define T1MS 0xD4CD
#define SpdOt_MAX_LEN 10

extern uint16 SpdOt_count[SpdOt_MAX_LEN];
extern uint16 SpdOt_temp;

void SpdOt_Init();
uint16 SpdOt_GetTime();

#define SpdOt_CLOSE_IT() EX0 = 0, ET1 = 0
#define SpdOt_OPEN_IT() EX0 = 1, ET1 = 1
#define SpdOt_Start() SpdOt_temp = 0, TL1 = T1MS, TH1 = T1MS >> 8, TR1 = 1, ET1 = 1
#define SpdOt_Stop() SpdOt_temp = 0, TL1 = T1MS, TH1 = T1MS >> 8, TR1 = 0, ET1 = 0

#endif