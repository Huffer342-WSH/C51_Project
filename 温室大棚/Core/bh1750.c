#include "bh1750.h"

/*引脚接口*/
#define SCL BH1750_SCL
#define SDA BH1750_SDA
/**
 * @brief 延迟5微妙
 *
 */

/*===================================================*/
/*                 I2C相关函数 （通用）               */
/*===================================================*/

static void Delay5us()
{
    unsigned char i;

    _nop_();
    _nop_();
    _nop_();
    i = 10;
    while (--i)
        ;
}

/**
 * @brief I2C起始信号
 *
 */
static void I2C_Start()
{
    SDA = 1;    //拉高数据线
    SCL = 1;    //拉高时钟线
    Delay5us(); //延时
    SDA = 0;    //产生下降沿
    Delay5us(); //延时
    SCL = 0;    //拉低时钟线
}

/**
 * @brief I2C停止信号
 *
 */
static void I2C_Stop()
{
    SDA = 0;    //拉低数据线
    SCL = 1;    //拉高时钟线
    Delay5us(); //延时
    SDA = 1;    //产生上升沿
    Delay5us(); //延时
}

/**
 * @brief I2C发送应答信号
 *
 * @param ack 0:ACK  1:NAK
 */
static void I2C_SendACK(bit ack)
{
    SDA = ack;  //写应答信号
    SCL = 1;    //拉高时钟线
    Delay5us(); //延时
    SCL = 0;    //拉低时钟线
    Delay5us(); //延时
}

/**
 * @brief 接收应答信号
 *
 * @return bit
 */
static bit I2C_RecvACK()
{
    SCL = 1;    //拉高时钟线
    Delay5us(); //延时
    CY = SDA;   //读应答信号
    SCL = 0;    //拉低时钟线
    Delay5us(); //延时

    return CY;
}

/**
 * @brief 向IIC总线发送一个字节数据
 *
 * @param dat
 */
static void I2C_SendByte(uint8 dat)
{
    uint8 i;

    for (i = 0; i < 8; i++) // 8位计数器
    {
        dat <<= 1;  //移出数据的最高位
        SDA = CY;   //送数据口
        SCL = 1;    //拉高时钟线
        Delay5us(); //延时
        SCL = 0;    //拉低时钟线
        Delay5us(); //延时
    }
    I2C_RecvACK();
}

/**
 * @brief 从IIC总线接收一个字节数据
 *
 * @return uint8
 */
static uint8 I2C_RecvByte()
{
    uint8 i;
    uint8 dat = 0;

    SDA = 1;                //使能内部上拉,准备读取数据,
    for (i = 0; i < 8; i++) // 8位计数器
    {
        dat <<= 1;
        SCL = 1;    //拉高时钟线
        Delay5us(); //延时
        dat |= SDA; //读数据
        SCL = 0;    //拉低时钟线
        Delay5us(); //延时
    }
    return dat;
}

/*===================================================*/
/*                 I2C相关函数 END                    */
/*===================================================*/

/**
 * @brief 设置BH1750寄存器
 *
 * @param REG_Address
 */
static void BH1750_SetReg(uint8 REG_Address)
{
    I2C_Start();                //起始信号
    I2C_SendByte(SlaveAddress); //发送设备地址+写信号
    I2C_SendByte(REG_Address);  //内部寄存器地址，
                                //  I2C_SendByte(REG_data);       //内部寄存器数据，
    I2C_Stop();                 //发送停止信号
}

/**
 * @brief BH1750初始化 精度1lux
 *
 */
void BH1750_Init()
{
    BH1750_SetReg(0x01);
    BH1750_SetReg(0x10);
}

/**
 * @brief 读取光照强度
 *
 * @return float
 */
float BH1750_Read(void)
{
    uint16 temp;
    I2C_Start();                    //起始信号
    I2C_SendByte(SlaveAddress + 1); //发送设备地址+读信号
    temp = I2C_RecvByte() << 8;
    I2C_SendACK(0);
    temp += I2C_RecvByte();
    I2C_SendACK(0);
    I2C_Stop(); //停止信号
    return (float)temp / 1.2f;
}
