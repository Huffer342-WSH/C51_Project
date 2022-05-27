#include "hcsr04.h"
#include "delay.h"

static uint32 hcsr0_Count = 0;

void zd0() interrupt 1  // T0中断用来计数器溢出,超过测距范围
{
  TL0 = 0x00;  //设置定时初始值
  TH0 = 0x00;  //设置定时初始值
  hcsr0_Count += 0xffff;
}

void StartModule()  // T1中断用来扫描数码管和计800MS启动模块
{
  Trig = 1;  // 800MS  启动一次模块
  Delayus(20);
  Trig = 0;
}

void HCSR04_Init() {
  AUXR |= 0x80;  //定时器时钟1T模式
  TMOD &= 0xF0;  //设置定时器模式
  TMOD |= 0x01;  //设置定时器模式
  TL0 = 0x00;    //设置定时初始值
  TH0 = 0x00;    //设置定时初始值
  TF0 = 0;       //清除TF0标志

  TR0 = 0;  //定时器0开始计时
  ET0 = 1;
  EA = 1;  //开启总中断
}

uint16 HCSR04_GetDist() {
  data uint32 timeout = 0;
  TH0 = 0;
  TL0 = 0;
  hcsr0_Count = 0;
  StartModule();
  while (!Echo)  //当RX为零时等待
  {
    if (timeout++ > 5000000) {
      return 0xffff;
    }
  }

  TR0 = 1;  //开启计数
  while (Echo && hcsr0_Count < 0xffffff)
    ;       //当RX为1计数并等待
  TR0 = 0;  //关闭计数
  if (hcsr0_Count < 0xffff00) {
    hcsr0_Count += TH0 * 256 + TL0;
    hcsr0_Count /= 65;
  }

  return hcsr0_Count;
}
