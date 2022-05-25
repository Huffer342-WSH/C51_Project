#include "bh1750.h"

/*���Žӿ�*/
#define SCL BH1750_SCL
#define SDA BH1750_SDA
/**
 * @brief �ӳ�5΢��
 *
 */

/*===================================================*/
/*                 I2C��غ��� ��ͨ�ã�               */
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
 * @brief I2C��ʼ�ź�
 *
 */
static void I2C_Start()
{
    SDA = 1;    //����������
    SCL = 1;    //����ʱ����
    Delay5us(); //��ʱ
    SDA = 0;    //�����½���
    Delay5us(); //��ʱ
    SCL = 0;    //����ʱ����
}

/**
 * @brief I2Cֹͣ�ź�
 *
 */
static void I2C_Stop()
{
    SDA = 0;    //����������
    SCL = 1;    //����ʱ����
    Delay5us(); //��ʱ
    SDA = 1;    //����������
    Delay5us(); //��ʱ
}

/**
 * @brief I2C����Ӧ���ź�
 *
 * @param ack 0:ACK  1:NAK
 */
static void I2C_SendACK(bit ack)
{
    SDA = ack;  //дӦ���ź�
    SCL = 1;    //����ʱ����
    Delay5us(); //��ʱ
    SCL = 0;    //����ʱ����
    Delay5us(); //��ʱ
}

/**
 * @brief ����Ӧ���ź�
 *
 * @return bit
 */
static bit I2C_RecvACK()
{
    SCL = 1;    //����ʱ����
    Delay5us(); //��ʱ
    CY = SDA;   //��Ӧ���ź�
    SCL = 0;    //����ʱ����
    Delay5us(); //��ʱ

    return CY;
}

/**
 * @brief ��IIC���߷���һ���ֽ�����
 *
 * @param dat
 */
static void I2C_SendByte(uint8 dat)
{
    uint8 i;

    for (i = 0; i < 8; i++) // 8λ������
    {
        dat <<= 1;  //�Ƴ����ݵ����λ
        SDA = CY;   //�����ݿ�
        SCL = 1;    //����ʱ����
        Delay5us(); //��ʱ
        SCL = 0;    //����ʱ����
        Delay5us(); //��ʱ
    }
    I2C_RecvACK();
}

/**
 * @brief ��IIC���߽���һ���ֽ�����
 *
 * @return uint8
 */
static uint8 I2C_RecvByte()
{
    uint8 i;
    uint8 dat = 0;

    SDA = 1;                //ʹ���ڲ�����,׼����ȡ����,
    for (i = 0; i < 8; i++) // 8λ������
    {
        dat <<= 1;
        SCL = 1;    //����ʱ����
        Delay5us(); //��ʱ
        dat |= SDA; //������
        SCL = 0;    //����ʱ����
        Delay5us(); //��ʱ
    }
    return dat;
}

/*===================================================*/
/*                 I2C��غ��� END                    */
/*===================================================*/

/**
 * @brief ����BH1750�Ĵ���
 *
 * @param REG_Address
 */
static void BH1750_SetReg(uint8 REG_Address)
{
    I2C_Start();                //��ʼ�ź�
    I2C_SendByte(SlaveAddress); //�����豸��ַ+д�ź�
    I2C_SendByte(REG_Address);  //�ڲ��Ĵ�����ַ��
                                //  I2C_SendByte(REG_data);       //�ڲ��Ĵ������ݣ�
    I2C_Stop();                 //����ֹͣ�ź�
}

/**
 * @brief BH1750��ʼ�� ����1lux
 *
 */
void BH1750_Init()
{
    BH1750_SetReg(0x01);
    BH1750_SetReg(0x10);
}

/**
 * @brief ��ȡ����ǿ��
 *
 * @return float
 */
float BH1750_Read(void)
{
    uint16 temp;
    I2C_Start();                    //��ʼ�ź�
    I2C_SendByte(SlaveAddress + 1); //�����豸��ַ+���ź�
    temp = I2C_RecvByte() << 8;
    I2C_SendACK(0);
    temp += I2C_RecvByte();
    I2C_SendACK(0);
    I2C_Stop(); //ֹͣ�ź�
    return (float)temp / 1.2f;
}
