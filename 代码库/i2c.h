#ifndef _I2C_H_
#define _I2C_H_

#include "main.h"

#ifndef __MAIN_H_
sbit SCL = P1 ^ 3;  // I2C  ʱ��
sbit SDA = P1 ^ 4;  // I2C  ����
#endif
extern bit ack;

void Start_I2c();                //�����ߺ���
void Stop_I2c();                 //�������ߺ���
void Ack_I2c(bit a);             //Ӧ���Ӻ���
void SendByte(unsigned char c);  //�ֽ����ݷ��ͺ���
bit ISendStr(unsigned char sla, unsigned char suba, unsigned char *s,
             unsigned char no);  //���ӵ�ַ���Ͷ��ֽ����ݺ���
bit ISendStrExt(unsigned char sla, unsigned char *s,
                unsigned char no);  //���ӵ�ַ���Ͷ��ֽ����ݺ���
unsigned char RcvByte();            //���ӵ�ַ���ֽ����ݺ���

#endif