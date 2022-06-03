#include "main.h"

/*============= �Ĵ������� =============*/
#ifndef __STC12C5A60S2_H__
#define
/*���� ADC ����Ĵ���*/
sfr ADC_CONTR = 0xBC; // ADC control register
sfr ADC_RES = 0xBD;   // ADC high 8-bit result register
sfr ADC_RESL = 0xBE;  // ADC low 2-bit result register
sfr P1ASF = 0x9D;     // P1 secondary function control register

sfr AUXR1 = 0xA2; // Auxiliary Register 1  -  PCA_P4  SPI_P4  S2_P4  GF2    ADRJ   -    DPS  0000,0000
/*
ADRJ:
    0, 10 λA/D ת������ĸ�8 λ����ADC_RES �Ĵ���, ��2 λ����ADC_RESL �Ĵ���
    1��10 λA/D ת����������2 λ����ADC_RES �Ĵ����ĵ�2 λ, ��8 λ����ADC_RESL �Ĵ���
*/
#endif

/*Define ADC operation const for ADC_CONTR*/
#define ADC_POWER 0x80   // ADC power control bit
#define ADC_FLAG 0x10    // ADC complete flag
#define ADC_START 0x08   // ADC start control bit
#define ADC_SPEEDLL 0x00 // 420 clocks
#define ADC_SPEEDL 0x20  // 280 clocks
#define ADC_SPEEDH 0x40  // 140 clocks
#define ADC_SPEEDHH 0x60 // 70 clocks

void InitADC(uint8 ch);
uint16 GetADCResult(uint8 ch);
