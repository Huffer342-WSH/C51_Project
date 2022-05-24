#ifndef _I2C_H_
#define _I2C_H_

#include <reg52.h>
#include <intrins.h>

extern bit ack;
//�����ߺ���
void Start_I2c();
//�������ߺ���
void Stop_I2c();
//Ӧ���Ӻ���
void Ack_I2c(bit a);
//�ֽ����ݷ��ͺ���
void SendByte(unsigned char c);
//���ӵ�ַ���Ͷ��ֽ����ݺ���
bit ISendStr(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);
//���ӵ�ַ���Ͷ��ֽ����ݺ���
bit ISendStrExt(unsigned char sla, unsigned char *s, unsigned char no);
//���ӵ�ַ���ֽ����ݺ���

unsigned char RcvByte();

#endif