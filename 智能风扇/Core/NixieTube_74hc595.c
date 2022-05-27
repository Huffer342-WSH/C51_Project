#include "NixieTube_74hc595.h"

unsigned char code LED_0F[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92,
                               0x82, 0xF8, 0x80, 0x90, 0x8C, 0xBF,
                               0xC6, 0xA1, 0x86, 0xFF, 0xbf};

static void LED_OUT(uint8 X) {
  uint8 i;
  for (i = 0; i < 8; i++) {
    X <<= 1;
    HC595_DIO = CY;  //溢出寄存器
    HC595_SCLK = 0;
    HC595_SCLK = 1;
  }
}

void LED4_Display(uint8 *c) {
  uint8 i;

  for (i = 0; i < 4; i++) {
    LED_OUT(LED_0F[c[i]]);
    LED_OUT(0x01 << i);
    HC595_RCLK = 0;
    HC595_RCLK = 1;
  }
}
/**
 * @brief 显示一位，轮流显示，建议中断调用
 *
 * @param c 需要显示的数据，四位
 */
void LED4_DisOne(uint8 *c) {
  static uint8 i = 0;
  HC595_RCLK = 1;
  i = ++i & 0x03;
  LED_OUT(LED_0F[c[i]]);
  LED_OUT(0x01 << i);
  HC595_RCLK = 0;
}