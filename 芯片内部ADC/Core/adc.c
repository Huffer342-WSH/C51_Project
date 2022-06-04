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
    P1ASF = p; // 打开P1端口对应bit的ADC
    AUXR1 |= 0x04; // 10位A/D 转换结果的最高2 位放在ADC_RES 寄存器的低2 位, 低8 位放在ADC_RESL 寄存器
    ADC_RES = 0; // 清零
    ADC_RESL = 0;
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch_list[index];
    delayms(10);

    EADC = 1; // ADC 中断允许位
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
//     _nop_(), _nop_(), _nop_(), _nop_(); // 查询之前必须等待
//     while (!(ADC_CONTR & ADC_FLAG))
//         ;                             // 等待完成标志
//     ADC_CONTR &= ~ADC_FLAG;           // 关闭ADC
//     return (ADC_RES << 8) + ADC_RESL; // 高2位+低8位
// }

uint16 GetADCResult(uint8 ch)
{

    return value[ch]; // 高2位+低8位
}