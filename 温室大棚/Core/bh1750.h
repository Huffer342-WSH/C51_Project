#ifndef __GY_30_H_
#define __GY_30_H_

#include "main.h"

/*���Žӿ���.c�ļ�����ֹ�궨���ͻ*/

//����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
// ALT  ADDRESS���Žӵ�ʱ��ַΪ0x46���ӵ�Դʱ��ַΪ0xB8
#define SlaveAddress 0x46

void BH1750_Init();
float BH1750_Read(void); //�����Ķ�ȡ�ڲ��Ĵ�������

#endif