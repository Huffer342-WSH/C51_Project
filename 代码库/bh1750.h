#ifndef __GY_30_H_
#define __GY_30_H_

#include "main.h"

sbit BH1750_SCL = P1 ^ 0; // IIC时钟引脚定义
sbit BH1750_SDA = P1 ^ 1; // IIC数据引脚定义

/*引脚接口在.c文件，防止宏定义冲突*/

//定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
// ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0xB8
#define SlaveAddress 0x46

void BH1750_Init();
float BH1750_Read(void); //连续的读取内部寄存器数据

#endif