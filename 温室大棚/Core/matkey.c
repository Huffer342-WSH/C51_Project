#include "matkey.h"
#include "delay.h"
#include "lcd12864_s.h"
#include "string.h"
#include "stdio.h"

uint8 KeyScan(void) {
  uint8 KeyValue = 0;

  KeyBoard_Port = 0xF0;
  delayms(1);

  /*消抖*/
  if (KeyBoard_Port == 0xF0) return 0;
  Delayms(5);
  if (KeyBoard_Port == 0xF0) return 0;

  //测试行
  switch (KeyBoard_Port) {
    case (0X70):
      KeyValue += 0;
      break;
    case (0Xb0):
      KeyValue += 4;
      break;
    case (0Xd0):
      KeyValue += 8;
      break;
    case (0Xe0):
      KeyValue += 12;
      break;
  }
  //测试列
  KeyBoard_Port = 0X0F;
  delayms(2);
  switch (KeyBoard_Port) {
    case (0X07):
      KeyValue += 4;
      break;
    case (0X0b):
      KeyValue += 3;
      break;
    case (0X0d):
      KeyValue += 2;
      break;
    case (0X0e):
      KeyValue += 1;
      break;
  }
  KeyBoard_Port = 0xF0;

  return KeyValue;
}

void KeyInput(uint8 *buf, uint8 len, uint8 beginKey, uint8 endKey, uint8 poxX,
              uint8 poxY) {
  uint8 code cov[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                      '8', '9', '0', 'b', 'c', 'd', 'e', 'f'};  //按键 => 字符

  data uint8 key = 0;  //记录按键

  data uint8 i = 0;
  memset(buf, ' ', len + 1);                  //初始化内存
  lcd12864_DDRAM_plus(poxX, poxY, buf, len);  //清除输入区域
  lcd12864_SetAC(poxX, poxY);                 //设置光标位置
  lcd12864_CursorOn();                        //显示光标

  while (beginKey == KeyScan())  //等待触发按键松开
    ;

  while (i < len) {
    while (key == KeyScan()) {  //等待按键变化
    }

    key = KeyScan();         //读入新的按键
    if (key == 0) continue;  //未按下，下一个循环
    if (key == endKey) {     //触发结束按键，退出函数
      while (endKey == KeyScan())
        ;
      return;
    }
    buf[i] = cov[key];           //忘内存中写入按键
    lcd12864_WriteData(buf[i]);  //在显示屏显示新输入的按键
    i++;
  }
  lcd12864_CursorOff();  //关闭显示光标
}