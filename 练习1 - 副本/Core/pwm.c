
// #include "reg51.h"
// #include "intrins.h"
#include "pwm.h"
#define FOSC 11059200L

typedef unsigned char BYTE;
typedef unsigned int WORD;

#ifndef __STC12C5A60S2_H__
/*Declare SFR associated with the PCA */
sfr CCON = 0xD8;      // PCA control register
sbit CCF0 = CCON ^ 0; // PCA module-0 interrupt flag
sbit CCF1 = CCON ^ 1; // PCA module-1 interrupt flag
sbit CR = CCON ^ 6;   // PCA timer run control bit
sbit CF = CCON ^ 7;   // PCA timer overflow flag
sfr CMOD = 0xD9;      // PCA mode register
sfr CL = 0xE9;        // PCA base timer LOW
sfr CH = 0xF9;        // PCA base timer HIGH
sfr CCAPM0 = 0xDA;    // PCA module-0 mode register
sfr CCAP0L = 0xEA;    // PCA module-0 capture register LOW
sfr CCAP0H = 0xFA;    // PCA module-0 capture register HIGH
sfr CCAPM1 = 0xDB;    // PCA module-1 mode register
sfr CCAP1L = 0xEB;    // PCA module-1 capture register LOW
sfr CCAP1H = 0xFB;    // PCA module-1 capture register HIGH
sfr PCAPWM0 = 0xf2;
sfr PCAPWM1 = 0xf3;

sfr AUXR1 = 0xA2; // Auxiliary Register 1  -  PCA_P4  SPI_P4  S2_P4  GF2    ADRJ   -    DPS  0000,0000

PCA_P4 : 0, 缺省PCA 在P1 口 1，PCA / PWM 从P1 口切换到P4 口 : ECI 从P1 .2 切换到P4 .1 口， PCA0 /
                                                              PWM0 从P1 .3 切换到P4 .2 口 PCA1 /
                                                              PWM1 从P1 .4 切换到P4 .3 口
#endif

                                                              void
                                                              PWM_Init(void)
{

    CCON = 0; // Initial PCA control register
              // PCA timer stop running
              // Clear CF flag
              // Clear all module interrupt flag
    CL = 0;   // Reset PCA base timer
    CH = 0;
    CMOD = 0x00;            // Set PCA timer clock source as Fosc/12
                            // Disable PCA timer overflow interrupt
    CCAP0H = CCAP0L = 0xff; // PWM0 port output 50% duty cycle square wave
    CCAPM0 = 0x42;          // PCA module-0 work in 8-bit PWM mode and no PCA interrupt
    PCA_PWM0 = 0x01;
    CCAP1H = CCAP1L = 0xff; // PWM1 port output 0% duty cycle square wave
    PCA_PWM1 = 0x00;
    CCAPM1 = 0x42; // PCA module-1 work in 8-bit PWM mode and no PCA interrupt

    CR = 1; // PCA timer start run
}
