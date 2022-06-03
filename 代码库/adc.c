#include "adc.h"

uint16 GetADCResult(uint8 ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
    _nop_(), _nop_(), _nop_(), _nop_(); // ��ѯ֮ǰ����ȴ�
    while (!(ADC_CONTR & ADC_FLAG))
        ;                             // �ȴ���ɱ�־
    ADC_CONTR &= ~ADC_FLAG;           // �ر�ADC
    return (ADC_RES << 8) + ADC_RESL; // ��2λ+��8λ
}

void InitADC(uint8 ch)
{
    P1ASF = ch; // ��P1�˿ڶ�Ӧbit��ADC
    AUXR1 |= 0x04; // 10λA/D ת����������2 λ����ADC_RES �Ĵ����ĵ�2 λ, ��8 λ����ADC_RESL �Ĵ���
    ADC_RES = 0; // ����
    ADC_RESL = 0;
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
}
