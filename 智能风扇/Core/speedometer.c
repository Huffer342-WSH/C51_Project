/**
 * @file speedometer.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief
 * 光电门测速，当频率较高时采用500ms计数的方式，当频率较低时采用间隔时间计时的方式
 *        当调用SpdOt_GetTimeFreq()函数获取时间和频率时会自动切换模式
 * @date 2022-04-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "delay.h"
#include "string.h"
#include "speedometer.h"
#include "NixieTube_74hc595.h"

uint16 SpdOt_countTime[SpdOt_MAX_LEN];
uint16 SpdOt_tempTime = 0;

uint16 SpdOt_countFreq[SpdOt_MAX_LEN];
uint16 SpdOt_tempFreq = 0;

static uint8 SpdOt_index = 0;
static uint8 SpdOt_mode = 0;
void SpdOt_Init()  // 5毫秒@11.0592MHz
{
  memset(SpdOt_countTime, 0xff, sizeof(SpdOt_countTime));
  memset(SpdOt_countFreq, 0x00, sizeof(SpdOt_countFreq));
  SpdOt_tempTime = 0XFFFF;
  SpdOt_index = 0;
  SpdOt_mode = 0;
  IT0 = 1;  // set INT0 int type (1:Falling 0:Low level)
  EX0 = 1;  // enable INT0 interrupt

  AUXR |= 0x40;  //定时器时钟1T模式
  TMOD &= 0x0F;  //设置定时器模式
  TMOD |= 0x10;  //设置定时器模式
  TL1 = 0x00;    //设置定时初始值
  TH1 = 0x28;    //设置定时初始值
  TF1 = 0;       //清除TF1标志
  TR1 = 1;       //定时器1开始计时

  ET1 = 1;
  EA = 1;  //开启总中断
}

/* Timer0 interrupt routine */
void tm1_isr() interrupt 3 {
  static uint8 count;

  TL1 = 0x00;  // reload timer1 low byte
  TH1 = 0x28;  // reload timer1 high byte

  if (SpdOt_mode) {
    if (++count > 100)  //累计500ms
    {
      if (++SpdOt_index >= SpdOt_MAX_LEN) {
        SpdOt_index = 0;
      }
      SpdOt_countFreq[SpdOt_index] = SpdOt_tempFreq << 1;
      SpdOt_tempFreq = 0;
      count = 0;
    }
  } else {
    if (SpdOt_tempTime < 0xfff0) {
      SpdOt_tempTime += 5;
    }

    if (SpdOt_tempTime >= 0x7FFF ||
        ((SpdOt_tempTime >> 1) > SpdOt_countTime[SpdOt_index] &&
         SpdOt_countTime[SpdOt_index] < 0x7FFF)) {
      if (++SpdOt_index >= SpdOt_MAX_LEN) {
        SpdOt_index = 0;
      }
      SpdOt_countTime[SpdOt_index] = SpdOt_tempTime;
    }
  }

  //  LED4_DisOne(LED);
  LED4_DisOne(LED);
}

// External interrupt0 service routine
void exint0() interrupt 0  //(location at 0003H)
{
  EX0 = 0;
  if (SpdOt_mode) {
    SpdOt_tempFreq++;
  } else {
    if (++SpdOt_index >= SpdOt_MAX_LEN) {
      SpdOt_index = 0;
    }
    SpdOt_countTime[SpdOt_index] = SpdOt_tempTime;
    SpdOt_tempTime = 0;
  }

  EX0 = 1;
}

void SpdOt_GetTimeFreq(uint16 *Time, float *Freq) {
  data uint8 i;
  uint32 sum = 0;
  if (SpdOt_mode) {
    for (i = 0; i < SpdOt_MAX_LEN; i++) {
      sum += SpdOt_countFreq[i];
      SpdOt_countTime[i] = 3000 / SpdOt_countFreq[i];
    }
    *Freq = (float)sum / (float)SpdOt_MAX_LEN;
    *Time = 3000 * SpdOt_MAX_LEN / sum;
  } else {
    for (i = 0; i < SpdOt_MAX_LEN; i++) {
      sum += SpdOt_countTime[i];
      SpdOt_countFreq[i] = 333 / SpdOt_countTime[i];
    }
    sum /= SpdOt_MAX_LEN;
    *Freq = 333.f / (float)sum;
    *Time = sum;
  }
  SpdOt_mode = *Time < 111;  //设置模式
}
