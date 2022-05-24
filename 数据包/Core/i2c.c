
#include "i2c.h"

#define NOP() _nop_()  /* �����ָ�� */
#define _Nop() _nop_() /*�����ָ��*/

bit ack; /*Ӧ���־λ*/

/**
 * @brief ����I2C����
 */
void Start_I2c() {
  GLOBAL_IT_CLOSE();
  SDA = 1; /*������ʼ�����������ź�*/
  _Nop();
  SCL = 1;
  _Nop(); /*��ʼ��������ʱ�����4.7us,��ʱ*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SDA = 0; /*������ʼ�ź�*/
  _Nop();  /* ��ʼ��������ʱ�����4��s*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SCL = 0; /*ǯסI2C���ߣ�׼�����ͻ�������� */
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}

/**
 * @brief  ����I2C����,
 */
void Stop_I2c() {
  GLOBAL_IT_CLOSE();
  SDA = 0; /*���ͽ��������������ź�*/
  _Nop();  /*���ͽ���������ʱ���ź�*/
  SCL = 1; /*������������ʱ�����4��s*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SDA = 1; /*����I2C���߽����ź�*/
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}

/**
 * @brief �ֽ����ݷ��ͺ�������������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
 * @param c �������ֽ�
 */
void SendByte(unsigned char c) {
  unsigned char BitCnt;
  GLOBAL_IT_CLOSE();
  for (BitCnt = 0; BitCnt < 8; BitCnt++) /*Ҫ���͵����ݳ���Ϊ8λ*/
  {
    if ((c << BitCnt) & 0x80)
      SDA = 1; /*�жϷ���λ*/
    else
      SDA = 0;
    _Nop();
    SCL = 1; /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
    _Nop();
    _Nop(); /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/
    _Nop();
    _Nop();
    _Nop();
    SCL = 0;
  }

  _Nop();
  _Nop();
  SDA = 1; /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
  _Nop();
  _Nop();
  SCL = 1;
  _Nop();
  _Nop();
  _Nop();
  if (SDA == 1)
    ack = 0;
  else
    ack = 1; /*�ж��Ƿ���յ�Ӧ���ź�*/
  SCL = 0;
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}

/**
 * @brief  �ֽ����ݽ��պ���
 *
 * @return unsigned char
 */
unsigned char RcvByte() {
  unsigned char retc;
  unsigned char BitCnt;

  retc = 0;
  SDA = 1; /*��������Ϊ���뷽ʽ*/

  GLOBAL_IT_CLOSE();
  for (BitCnt = 0; BitCnt < 8; BitCnt++) {
    _Nop();
    SCL = 0; /*��ʱ����Ϊ�ͣ�׼����������λ*/
    _Nop();
    _Nop(); /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/
    _Nop();
    _Nop();
    _Nop();
    SCL = 1; /*��ʱ����Ϊ��ʹ��������������Ч*/
    _Nop();
    _Nop();
    retc = retc << 1;
    if (SDA == 1) retc = retc + 1; /*������λ,���յ�����λ����retc�� */
    _Nop();
    _Nop();
  }
  SCL = 0;
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
  return (retc);
}

/**
 * @brief ����������Ӧ���ź�
 *
 * @param a
 */
void Ack_I2c(bit a) {
  GLOBAL_IT_CLOSE();

  if (a == 0)
    SDA = 0; /*�ڴ˷���Ӧ����Ӧ���ź� */
  else
    SDA = 1;
  _Nop();
  _Nop();
  _Nop();
  SCL = 1;
  _Nop();
  _Nop(); /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
  _Nop();
  _Nop();
  _Nop();
  SCL = 0; /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
  _Nop();
  _Nop();
  GLOBAL_IT_OPEN();
}
