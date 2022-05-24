#include "main.h"

#include "stdio.h"
#include "stdlib.h"

#include "usart.h"
#include "delay.h"

#include "ds1302.h"
#include "lcd12864_s.h"
#include "pcf8591.h"
#include "hcsr04.h"
#include "pwm.h"
#include "matkey.h"
#include "speedometer.h"
#include "NixieTube_74hc595.h"
#include "eeprom.h"

void SetTimeMain();
//屏幕缓存
uint8_t xdata ScreenBuf[] =
    "0123456789abcdefg0123456789abcdefg0123456789abcdefg0123456789abcdefg000000"
    "0000";
//用于LED的8位显示缓存
uint8 LED[4];
//初始时间定义                                             //当前时间
DS1302_union ds1302_buf = {0x20, 0x22, 0x4, 0x20, 0x17, 0x41, 0x30, 0x3};

//状态机变量
uint8 state;
uint8 pwnCC;

void main(void) {
  uint8 adValue, keyValue;
  uint16 dist;
  uint8 temp8;
  uint16 temp16a;
  float tempf;
  /*-------------------初始化---------------------------------*/
  P4SW = 0xff;               // P4端口均做IO（P4.7除外）
  P0M0 = 0x00, P0M1 = 0x00;  // P0漏弱上拉，双向IO

  Delayms(100);
  Uart_Init();
  Delayms(100);
  lcd12864_Init();
  Delayms(100);
  ds1302_init();
  Delayms(100);
  // ds1302_write_time(&ds1302_buf);
  Delayms(100);
  HCSR04_Init();
  PWM_Init();
  SendString("hello");
  /*开屏动画*/
  lcd12864_HCDisp2(ScreenBuf + 16, "0123456789", "9876543210", 10, -10, 3, 16,
                   3, 10);
  delayms(200);
  lcd12864_HCDisp2(ScreenBuf + 16, "0123456789", "9876543210", 10, 3, 16, 3,
                   -10, 10);
  KeyBoard_Port = 0x0F;

  SpdOt_Init();

  state = STATE_AUTO;
  while (1) {
    SendString("hello");
    Delayms(100);  //延迟适当的时间
    /*状态机*/
    keyValue = KeyScan();
    if (state == STATE_AUTO) {
      if (keyValue == 14) {
        state = STATE_SETTIME;
      } else if (keyValue == 15) {
        state = STATE_SETSEPPED;
      } else if (keyValue == 16) {
        state = STATE_MANUAL;
      }
    } else if (state == STATE_MANUAL) {
      if (keyValue == 16) {
        state = STATE_AUTO;
      } else if (keyValue == 15) {
        state = STATE_SETSEPPED;
      }
    } else if (state == STATE_SETTIME) {
      state = STATE_AUTO;
    } else if (state == STATE_SETSEPPED) {
      state = STATE_MANUAL;
    }

    /*执行对应状态操作*/
    if (state == STATE_AUTO) {
      /*AD*/
      DA_Res(adValue);
      pwnCC = adValue;
      PWM_SetDutyH0(pwnCC);
      sprintf(ScreenBuf, "AUTO   PWM: %3bu   ", pwnCC);
      /*ds1302*/
      ds1302_read_time(&ds1302_buf);
      temp8 = IapReadByte(IAP_ADDRESS);
      sprintf(ScreenBuf + 17, "%2x.%02x:%02x:%02x %d ",
              (uint16)ds1302_buf.name.day, (uint16)ds1302_buf.name.hour,
              (uint16)ds1302_buf.name.minute, (uint16)ds1302_buf.name.second,
              (uint16)temp8);
      LED[0] = ds1302_buf.name.minute & 0xf;
      LED[1] = ds1302_buf.name.minute >> 4;
      LED[2] = ds1302_buf.name.hour & 0xf;
      LED[3] = ds1302_buf.name.hour >> 4;

      /*HCSR04*/
      dist = HCSR04_GetDist();
      if (HW) {
        sprintf(ScreenBuf + 34, "Dis:[%4u]mm 1 ", dist);
      } else {
        sprintf(ScreenBuf + 34, "Dis:[%4u]mm 0 ", dist);
      }

      /*KeyBoard*/

      /*测速模块*/
      SpdOt_GetTimeFreq(&temp16a, &tempf);
      if (temp16a < 0xfff0) {
        sprintf(ScreenBuf + 51, "T:%5u V:%.2f ", temp16a, tempf);
      } else {
        sprintf(ScreenBuf + 51, "T:*INF* V: 0   ");
      }
      lcd12864_DDRAM_Flush(ScreenBuf);
    } else if (state == STATE_MANUAL) {
      lcd12864_DDRAM_Flush(ScreenBuf);
      PWM_SetDutyH0(pwnCC);
      sprintf(ScreenBuf, "MANUAL PWM: %3bu ", pwnCC);
      /*ds1302*/
      ds1302_read_time(&ds1302_buf);
      sprintf(ScreenBuf + 17, "%2x.%02x:%02x:%02x    ",
              (uint16)ds1302_buf.name.day, (uint16)ds1302_buf.name.hour,
              (uint16)ds1302_buf.name.minute, (uint16)ds1302_buf.name.second);
      LED[0] = ds1302_buf.name.minute & 0xf;
      LED[1] = ds1302_buf.name.minute >> 4;
      LED[2] = ds1302_buf.name.hour & 0xf;
      LED[3] = ds1302_buf.name.hour >> 4;
      /*HCSR04*/
      dist = HCSR04_GetDist();
      if (HW) {
        sprintf(ScreenBuf + 34, "Dis:[%4u]mm 1 ", dist);
      } else {
        sprintf(ScreenBuf + 34, "Dis:[%4u]mm 0 ", dist);
      }

      /*测速模块*/
      SpdOt_GetTimeFreq(&temp16a, &tempf);
      if (temp16a < 0xfff0) {
        sprintf(ScreenBuf + 51, "T:%5u V:%.2f ", temp16a, tempf);
      } else {
        sprintf(ScreenBuf + 51, "T:*INF* V: 0   ");
      }
      lcd12864_DDRAM_Flush(ScreenBuf);
    } else if (state == STATE_SETSEPPED) {
      KeyInput(ScreenBuf + 12, 3, 15, 16, 6, 0);
      pwnCC = atoi(ScreenBuf + 12);

    } else if (state == STATE_SETTIME) {
      SetTimeMain();
    }
  }
}

void SetTimeMain() {
  uint8 code cov[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                      '8', '9', '0', 'b', 'c', 'd', 'e', 'f'};  //
  uint8 key = 0, t = 0;                                         //记录按键
  lcd12864_SetAC(0, 1);  //设置光标位置
  lcd12864_CursorOn();   //显示光标

  /*天*/
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t = cov[key] - '0';
  t <<= 4;
  lcd12864_WriteData(cov[key]);
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t += cov[key] - '0';
  if (t < 0x32) {
    lcd12864_WriteData(cov[key]);
  }
  ds1302_buf.name.day = t;
  lcd12864_WriteData('.');
  /*时*/
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t = cov[key] - '0';
  t <<= 4;
  lcd12864_WriteData(cov[key]);
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t += cov[key] - '0';
  lcd12864_WriteData(cov[key]);
  if (t < 0x24) {
    ds1302_buf.name.hour = t;
  }
  lcd12864_WriteData(':');
  /*分*/
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t = cov[key] - '0';
  t <<= 4;
  lcd12864_WriteData(cov[key]);
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t += cov[key] - '0';
  lcd12864_WriteData(cov[key]);
  if (t < 0x60) {
    ds1302_buf.name.minute = t;
  }
  lcd12864_WriteData(':');
  /*秒*/
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t = cov[key] - '0';
  t <<= 4;
  lcd12864_WriteData(cov[key]);
  while (0 != KeyScan())  //等待触发按键松开
    ;
  while (0 == KeyScan()) {  //等待按键变化
  }
  key = KeyScan();
  t += cov[key] - '0';
  lcd12864_WriteData(cov[key]);
  if (t < 0x60) {
    ds1302_buf.name.second = t;
  }
  lcd12864_CursorOff();
  ds1302_write_time(&ds1302_buf);
}