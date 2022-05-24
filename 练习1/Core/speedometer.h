#ifndef __SPEEDOMETER_H_
#define __SPEEDOMETER_H_

#include "main.h"

#define SpdOt_MAX_LEN 10
#define TH5MS 0x28
#define TL5MS 0x00
extern uint16 SpdOt_tempTime;

void SpdOt_Init();
void SpdOt_GetTimeFreq(uint16 *Time, float *Freq);

#define SpdOt_CLOSE_IT() EX0 = 0, ET1 = 0
#define SpdOt_OPEN_IT() EX0 = 1, ET1 = 1

#define SpdOt_Start() \
  SpdOt_tempTime = 0, TL1 = TL5MS, TH1 = TH5MS, TR1 = 1, ET1 = 1
#define SpdOt_Stop() \
  SpdOt_tempTime = 0, TL1 = TL5MS, TH1 = TH5MS, TR1 = 0, ET1 = 0

#endif