// 51单片机控制温湿度传感器DHT11然后在LCD1602上显示当前的温湿度及单位其中湿度在第一行显示温度在第二行显示。单片机为YL-9最小系统。

#include "dht11.h"
#include "delay.h"

static uint8 read() //接收一个字节//
{
    uint8 i;
    uint8 Byte = 0;

    for (i = 0; i < 8; i++) //接收8bit的数据
    {
        Byte <<= 1; //接收的数据为高位在前右移
        while (!DHT11_DIO)
            ; //等待50us的低电平开始信号结束
        Delay50us();
        Byte |= DHT11_DIO;
        // temp = 0; //时间为26us-28us表示接收的为数据'0'
        // if (DHT11_DIO == 1)
        //     temp = 1; //如果26us-28us之后还为高电平则表示接收的数据为'1'
        while (DHT11_DIO)
            ; //等待数据信号高电平'0'为26us-28us'1'为70us
        // Byte <<= 1; //接收的数据为高位在前右移
        // Byte |= temp;
    }
    return Byte;
}

uint8 DHT11_Read(uint8 *humidity, uint8 *temperature)
{
    uint8 TH, TL, HH, HL, check;
    DHT11_DIO = 1;
      delayms(50);
    // 主机把总线拉低必须大于18ms保证DHT11能检测到起始信号
    DHT11_DIO = 0;
    delayms(20);
    DHT11_DIO = 1;
    //主机拉高总线延时20-40us后DHT11拉低总线80us
    Delay50us();
    if (!DHT11_DIO) //判断从机是否有低电平响应信号//

    {
        while (!DHT11_DIO)
            ; //判断从机发出 80us 的低电平响应信号是否结束//

        while (DHT11_DIO)
            ;           //判断从机发出 80us 的高电平是否结束如结束则主机进入数据接收状态
        HH = read();    //湿度高位
        HL = read();    //湿度低位
        TH = read();    //温度高位
        TL = read();    //温度低位
        check = read(); //校验位
        DHT11_DIO = 0;  //当最后一bit数据接完毕后从机拉低电平50us//
        Delay50us();    //差不多50us的延时
        DHT11_DIO = 1;  //总线由上拉电阻拉高进入空闲状态
        check = HH + HL + TH + TL;
        if (check == check) //判断读到的四个数据之和是否与校验位相同
        {
            *humidity = HH;
            *temperature = TH;
            check = check;
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return 0;
    }
}