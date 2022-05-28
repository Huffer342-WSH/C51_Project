// 51��Ƭ��������ʪ�ȴ�����DHT11?Ȼ����LCD1602����ʾ��ǰ����ʪ�ȼ���λ?����ʪ���ڵ�һ����ʾ?�¶��ڵڶ�����ʾ����Ƭ��ΪYL-9��Сϵͳ��

#include "dht11.h"
#include "delay.h"

static uint8 read() //����һ���ֽ�//
{
    uint8 i;
    uint8 Byte = 0;

    for (i = 0; i < 8; i++) //����8bit������
    {
        Byte <<= 1; //���յ�����Ϊ��λ��ǰ?����?
        while (!DHT11_DIO)
            ; //�ȴ�50us�ĵ͵�ƽ��ʼ�źŽ���
        Delay50us();
        Byte |= DHT11_DIO;
        // temp = 0; //ʱ��Ϊ26us-28us?��ʾ���յ�Ϊ����'0'
        // if (DHT11_DIO == 1)
        //     temp = 1; //���26us-28us֮��?��Ϊ�ߵ�ƽ?���ʾ���յ�����Ϊ'1'
        while (DHT11_DIO)
            ; //�ȴ������źŸߵ�ƽ?'0'Ϊ26us-28us?'1'Ϊ70us?
        // Byte <<= 1; //���յ�����Ϊ��λ��ǰ?����?
        // Byte |= temp;
    }
    return Byte;
}

uint8 DHT11_Read(uint8 *humidity, uint8 *temperature)
{
    uint8 TH, TL, HH, HL, check;
    DHT11_DIO = 1;
    delayms(50);
    // �������������ͱ������18ms��֤DHT11�ܼ�⵽��ʼ�ź�
    DHT11_DIO = 0;
    delayms(20);
    DHT11_DIO = 1;
    //��������������ʱ20-40us��DHT11��������80us
    Delay50us();
    if (!DHT11_DIO) //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�//

    {
        while (!DHT11_DIO)
            ; //�жϴӻ����� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����//

        while (DHT11_DIO)
            ;           //�жϴӻ����� 80us �ĸߵ�ƽ�Ƿ����?������������������ݽ���״̬
        HH = read();    //ʪ�ȸ�λ
        HL = read();    //ʪ�ȵ�λ
        TH = read();    //�¶ȸ�λ
        TL = read();    //�¶ȵ�λ
        check = read(); //У��λ
        DHT11_DIO = 0;  //�����һbit���ݽ���Ϻ�?�ӻ����͵�ƽ50us//
        Delay50us();    //���50us����ʱ
        DHT11_DIO = 1;  //������������������?�������״̬
        check = HH + HL + TH + TL;
        if (check == check) //�ж϶������ĸ�����֮���Ƿ���У��λ��ͬ
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