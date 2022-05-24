#include "hcsr04.h"
#include "delay.h"

static uint32 hcsr0_Count = 0;

void zd0() interrupt 1  // T0�ж��������������,������෶Χ
{
  TL0 = 0x00;  //���ö�ʱ��ʼֵ
  TH0 = 0x00;  //���ö�ʱ��ʼֵ
  hcsr0_Count += 0xffff;
}

void StartModule()  // T1�ж�����ɨ������ܺͼ�800MS����ģ��
{
  Trig = 1;  // 800MS  ����һ��ģ��
  Delayus(20);
  Trig = 0;
}

void HCSR04_Init() {
  AUXR |= 0x80;  //��ʱ��ʱ��1Tģʽ
  TMOD &= 0xF0;  //���ö�ʱ��ģʽ
  TMOD |= 0x01;  //���ö�ʱ��ģʽ
  TL0 = 0x00;    //���ö�ʱ��ʼֵ
  TH0 = 0x00;    //���ö�ʱ��ʼֵ
  TF0 = 0;       //���TF0��־

  TR0 = 0;  //��ʱ��0��ʼ��ʱ
  ET0 = 1;
  EA = 1;  //�������ж�
}

uint16 HCSR04_GetDist() {
  data uint32 timeout = 0;
  TH0 = 0;
  TL0 = 0;
  hcsr0_Count = 0;
  StartModule();
  while (!Echo)  //��RXΪ��ʱ�ȴ�
  {
    if (timeout++ > 5000000) {
      return 0xffff;
    }
  }

  TR0 = 1;  //��������
  while (Echo && hcsr0_Count < 0xffffff)
    ;       //��RXΪ1�������ȴ�
  TR0 = 0;  //�رռ���
  if (hcsr0_Count < 0xffff00) {
    hcsr0_Count += TH0 * 256 + TL0;
    hcsr0_Count /= 65;
  }

  return hcsr0_Count;
}
