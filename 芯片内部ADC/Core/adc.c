#include "adc.h"
#include "delay.h"

static uint8 ch_list[] = {2};
static uint16 value[1];
static uint8 len = 1;
static uint8 index = 0;

void InitADC()
{
    uint8 i, p = 0;
    for (i = 0; i < len; i++)
    {
        p |= 1 << ch_list[i];
    }
    P1ASF = p; // ��P1�˿ڶ�Ӧbit��ADC
    AUXR1 |= 0x04; // 10λA/D ת����������2 λ����ADC_RES �Ĵ����ĵ�2 λ, ��8 λ����ADC_RESL �Ĵ���
    ADC_RES = 0; // ����
    ADC_RESL = 0;
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch_list[index];
    delayms(10);

    EADC = 1; // ADC �ж�����λ
    EA = 1;
}

void adc_isr() interrupt 5
{
    value[index] = (ADC_RES << 8) + ADC_RESL;
    ADC_CONTR &= !ADC_FLAG; // Clear ADC interrupt flag
    if (++index >= len)
    {
        index = 0;
    }
    _nop_(), _nop_(), _nop_(), _nop_();
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch_list[index];
}

// uint16 GetADCResult(uint8 ch)
// {
//     ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
//     _nop_(), _nop_(), _nop_(), _nop_(); // ��ѯ֮ǰ����ȴ�
//     while (!(ADC_CONTR & ADC_FLAG))
//         ;                             // �ȴ���ɱ�־
//     ADC_CONTR &= ~ADC_FLAG;           // �ر�ADC
//     return (ADC_RES << 8) + ADC_RESL; // ��2λ+��8λ
// }

uint16 GetADCResult(uint8 ch)
{

    return value[ch]; // ��2λ+��8λ
}