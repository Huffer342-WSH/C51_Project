#include "adc.h"

uint16 GetADCResult(uint8 ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
    _nop_(), _nop_(), _nop_(), _nop_(); // 查询之前必须等待
    while (!(ADC_CONTR & ADC_FLAG))
        ;                             // 等待完成标志
    ADC_CONTR &= ~ADC_FLAG;           // 关闭ADC
    return (ADC_RES << 8) + ADC_RESL; // 高2位+低8位
}

void InitADC(uint8 ch)
{
    P1ASF = ch; // 打开P1端口对应bit的ADC
    AUXR1 |= 0x04; // 10位A/D 转换结果的最高2 位放在ADC_RES 寄存器的低2 位, 低8 位放在ADC_RESL 寄存器
    ADC_RES = 0; // 清零
    ADC_RESL = 0;
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
}
