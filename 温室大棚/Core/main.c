#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bh1750.h"
#include "delay.h"
#include "dht11.h"
#include "lcd12864_s.h"
#include "matkey.h"
#include "pwm.h"
#include "rc522.h"
#include "usart.h"

void BootAnime();
void tm0_init();
//��Ļ����
uint8_t ScreenBuf[72];
uint16 tm0Count; //��ʱ��0�жϼ���

// IC��
#define MAXCARD 3
#define SU (uint32)0x69c5b461
typedef struct
{
    CardSerial_t id[MAXCARD];
    uint8 rights[MAXCARD];
    uint8 len;
} idList_t;
idList_t idList;
CardSerial_t id;

typedef struct
{
    uint8 temp[15];
    uint8 humi[15];
    float light[15];
    uint8 head;
} info_t;

info_t his_info;

//��ʪ��
uint8 humi_real, temp_real, humi_th = 70, temp_th = 30; //��ʪ��
float light_real, light_th = 10.0f;                     //����

uint8 keyvalue;

uint8 fanPWM = 100;
uint8 fanCount = 0;

uint8 LED_H = 255;
uint8 LED_M = 150;
uint8 LED_L = 70;

//״̬��
uint8 state = ST_DEFAULT;
uint8 statePre, stateNext;

#define N 18
#define LINE(x) ScreenBuf + x *N
#define flush() lcd12864_DDRAM_Flush(ScreenBuf, N)

#define LED_ON() PWM_SetCAP1(ledPWM)
#define LED_OFF() PWM_SetCAP1(0)

#define FAN_ON() PWM_SetDutyH0(fanPWM)
#define FAN_OFF() PWM_SetDutyH0(0)

void stateDefault();
void alarm();
void changeState();
void stateLogin();
void stateMenu();
void stateManage();
void stateDate();
void stateNew();
void stateLight();
void stateTemp();
void stateHumi();
/**
 * @brief ��ʱ��0��ʼ��������50ms
 *
 */
void tm0_init()
{

    AUXR &= 0x7F; //��ʱ��ʱ��12Tģʽ
    TMOD &= 0xF0; //���ö�ʱ��ģʽ
    TMOD |= 0x01; //���ö�ʱ��ģʽ
    TL0 = 0x00;   //���ö�ʱ��ʼֵ
    TH0 = 0x4C;   //���ö�ʱ��ʼֵ
    TF0 = 0;      //���TF0��־
    TR0 = 1;      //��ʱ��0��ʼ��ʱ

    ET0 = 1;      // enable timer0 interrupt
    EA = 1;       // open global interrupt switch
    tm0Count = 0; // initial counter
}

uint8 findCard(uint32 id)
{
    uint8 i;
    for (i = 0; i < idList.len; i++)
    {
        if (idList.id[i].u32 == id)
        {
            return idList.rights[i];
        }
    }
    return R_NONE;
}

uint8 addCard(uint32 id)
{
    uint8 i;
    i = findCard(id);
    if (i == R_NONE)
    {
        idList.id[idList.len].u32 = id;
        idList.rights[idList.len] = R_GENERIC;
        idList.len++;
    }
    return i;
}
void main(void)
{
    uint8 i;

    /*-------------------��ʼ��---------------------------------*/
    P4SW = 0xff;              // P4�˿ھ���IO��P4.7���⣩
    P0M0 = 0x00, P0M1 = 0x00; // P0©��������˫��IO

    /*=============��ʼ��=============*/
    lcd12864_Init();
    BH1750_Init();
    PWM_Init();
    PcdInit();
    Uart_Init();
    tm0_init();
    printf("hello\r\n");

    for (i = 1; i < MAXCARD; i++)
    {
        idList.id[i].u32 = 0;
        idList.rights[i] = R_NONE;
    }
    idList.id[0].u32 = SU;
    idList.rights[0] = R_ADMIN;
    idList.len = 1;
    /*=============��������=============*/
    // BootAnime();

    /*=============��ѭ��=============*/
    while (1)
    {
        changeState();

        if (state == ST_DEFAULT)
        {
            stateDefault();
        }
        else if (state == ST_LOGIN)
        {
            stateLogin();
        }
        else if (state == ST_MENU)
        {
            stateMenu();
        }
        else if (state == ST_MANAGE)
        {
            stateManage();
        }
        else if (state == ST_DATA)
        {
            stateDate();
        }
        else if (state == ST_NEW)
        {
            stateNew();
        }
        else if (state == ST_LIGHT)
        {
            stateLight();
        }
        else if (state == ST_TEMP)
        {
            stateTemp();
        }
        else if (state == ST_HUMI)
        {
            stateHumi();
        }
    }
}

void changeState()
{
    keyvalue = KeyScan();
    if (state == ST_DEFAULT)
    {
        if (keyvalue == 1)
        {
            statePre = state;
            state = ST_LOGIN;
        }
    }
    else if (state == ST_LOGIN)
    {
        statePre = state;
        state = stateNext;
    }
    else if (state == ST_MENU)
    {
        statePre = state;
        state = stateNext;
    }
    else if (state == ST_MANAGE)
    {
        statePre = state;
        state = stateNext;
    }
    else if (state == ST_DATA)
    {
        statePre = state;
        state = stateNext;
    }
    else if (state == ST_NEW)
    {
        statePre = state;
        state = stateNext;
    }
    else if (state == ST_LIGHT)
    {
        statePre = state;
        state = stateNext;
    }
    else if (state == ST_TEMP)
    {
        statePre = state;
        state = stateNext;
    }
    else if (state == ST_HUMI)
    {
        statePre = state;
        state = stateNext;
    }
}
/*��������*/
void BootAnime()
{
    uint8 i = 0;
    lcd12864_Init();
    delayms(500);
    lcd12864_DDRAM(0, 0, "      ICCS");
    lcd12864_DDRAM(1, 0, "      1085");

    for (i = 0; i < 32; i++)
    {
        lcd12864_WriteCmd(0x34);     //������չָ��
        lcd12864_WriteCmd(0x03);     //����д�����ַ
        lcd12864_WriteCmd(0x40 + i); //д�����ַ
        delayms(80);
        // lcd12864_WriteCmd(0x30);
        // lcd12864_WriteCmd(0x05);
        lcd12864_WriteCmd(0x30); //ѡ��8bit����
    }
    lcd12864_Init();
}

/* Timer0 interrupt routine */
void tm0_isr() interrupt 1
{
    uint8 i;
    //��ʱ����װ
    TL0 = 0x00;
    TH0 = 0x4C;
    // 50*40=2000,����һ�Σ���ȡ����
    if (tm0Count-- == 0)
    {
        tm0Count = 40;
        DHT11_Read(&humi_real, &temp_real);
        light_real = BH1750_Read();

        for (i = 14; i >= 1; i--)
        {
            his_info.temp[i] = his_info.temp[i - 1];
            his_info.humi[i] = his_info.humi[i - 1];
            his_info.light[i] = his_info.light[i - 1];
        }
        his_info.temp[0] = temp_real;
        his_info.humi[0] = humi_real;
        his_info.light[0] = light_real;
    }
    if (fanCount)
    {
        fanCount--;
    }
}

void stateDefault()
{
    uint8 rfstate, flag;
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    rfstate = PcdFindcard(&id);
    sprintf(LINE(0), "�¶�:%-3buʪ��:%-3bu", temp_real, humi_real);
    sprintf(LINE(1), "��ǿ:%7.2f lx", light_real);
    if (rfstate == 0)
    {
        sprintf(LINE(2), "ID:%lx", id.u32);
        flag = findCard(id.u32);
        if (flag == R_ADMIN)
        {
            sprintf(LINE(3), "%-15s", "����");
        }
        else if (flag == R_GENERIC)
        {
            sprintf(LINE(3), "%-15s", "�û���");
        }
        else if (flag == R_NONE)
        {
            sprintf(LINE(3), "%-15s", "���û���");
            // addCard(card);
        }
    }

    lcd12864_DDRAM_Flush(ScreenBuf, N);
    alarm();
}
void alarm()
{
    static uint8 alarmTH = 0, alarmLight = 0;
    //��ʪ�ȱ���
    if (temp_real > temp_th || humi_real > humi_th)
    {
        if (alarmTH == 0)
        {
            printf("��ʪ�ȳ���\r\n");
            alarmTH = 1;
            fanCount = 100;
        }
        FAN_ON();
    }
    else if (fanCount == 0)
    {
        alarmTH = 0;
        FAN_OFF();
    }
    //���ձ���
    if (light_real < light_th)
    {
        if (alarmLight == 0)
        {
            printf("��������ƫ��\r\n");
            alarmLight = 1;
        }
        if (light_real < light_th / 3)
        {
            PWM_SetCAP1(LED_H);
        }
        else if (light_real < light_th * 2 / 3)
        {
            PWM_SetCAP1(LED_M);
        }
        else
        {
            PWM_SetCAP1(LED_L);
        }
    }
    else
    {
        alarmLight = 0;
        LED_OFF();
    }
}
void stateLogin()
{
    uint8 rfstat = 1, flag = 0;
    uint32 card;
    uint8 count = 0;
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "    ��ˢ��");
    sprintf(LINE(3), "Press 16 exit");
    flush();
    while (1)
    {

        rfstat = PcdFindcard(&id);
        if (rfstat)
        {
            count = 0;
            memset(ScreenBuf, ' ', sizeof(ScreenBuf));
            sprintf(LINE(0), "    ��ˢ��");
            sprintf(LINE(3), "Press 16 exit");
            flush();
        }
        else
        {
            count++;
            card = id.u32;

            flag = findCard(card);

            sprintf(LINE(0), "%-13s%-3bu", "��⵽��", count);
            if (flag == R_ADMIN)
            {
                sprintf(LINE(1), "����");
            }
            else if (flag == R_GENERIC)
            {
                sprintf(LINE(1), "�û���");
            }
            else if (flag == R_NONE)
            {
                sprintf(LINE(1), "���û���");
                // addCard(card);
            }

            sprintf(LINE(2), "ID:%lx", id.u32);
            sprintf(LINE(3), "Press 16 exit");
            flush();
        }

        if (KeyScan() == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        if (count > 5)
        {
            if (flag == R_ADMIN)
            {
                stateNext = ST_MENU;
                return;
            }
            else if (flag == R_GENERIC)
            {
                stateNext = ST_DATA;

                return;
            }
            else
            {
                count = 0;
            }
        }
    }
}
void stateMenu()
{
    uint8 key;
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "1.�����û�");
    sprintf(LINE(1), "2.�鿴��¼");
    sprintf(LINE(2), "3.���û�");
    sprintf(LINE(3), "Press 16 exit");

    while (1)
    {
        flush();
        key = KeyScan();
        while (KeyScan())
            ;
        if (key == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        else if (key == 13)
        {
            memset(ScreenBuf, ' ', sizeof(ScreenBuf));
            sprintf(LINE(0), "1.�����û�");
            sprintf(LINE(1), "2.�鿴��¼");
            sprintf(LINE(2), "3.���û�");
            sprintf(LINE(3), "Press 16 exit");
        }
        else if (key == 14)
        {
            memset(ScreenBuf, ' ', sizeof(ScreenBuf));
            sprintf(LINE(0), "4.���ù���");
            sprintf(LINE(1), "5.�����¶�");
            sprintf(LINE(2), "6.����ʪ��");
            sprintf(LINE(3), "Press 16 exit");
        }
        else if (key == 1)
        {
            stateNext = ST_MANAGE;
            return;
        }
        else if (key == 2)
        {
            stateNext = ST_DATA;
            return;
        }
        else if (key == 3)
        {
            stateNext = ST_NEW;
            return;
        }
        else if (key == 4)
        {
            stateNext = ST_LIGHT;
            return;
        }
        else if (key == 5)
        {
            stateNext = ST_TEMP;
            return;
        }
        else if (key == 6)
        {
            stateNext = ST_HUMI;
            return;
        }
    }
}

// void stateMenuG()
// {
//     uint8 key;
//     memset(ScreenBuf, ' ', sizeof(ScreenBuf));
//     sprintf(LINE(1), "1.�鿴��¼");
//     sprintf(LINE(3), "Press 16 exit");
//     while (1)
//     {
//         flush();
//         key = KeyScan();
//         while (KeyScan())
//             ;
//         if (key == 16)
//         {
//             stateNext = ST_DEFAULT;
//             return;
//         }
//         else if (key == 1)
//         {
//             stateNext = ST_DATA;
//             return;
//         }
//     }
// }
void stateManage()
{

    uint8 key;
    uint8 idIndex = 0;
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "%-15s", "ѡ��һ�ſ�");

    sprintf(LINE(3), "exit:16 new:15");
    while (1)
    {
        flush();
        sprintf(LINE(0), "%-15s", "ѡ��һ�ſ�");
        sprintf(LINE(1), "ID:%08lx", idList.id[idIndex].u32);
        if (idList.rights[idIndex] == R_ADMIN)
        {
            sprintf(LINE(2), "%-8s%b3u/%-2bu", "����Ա", idIndex + 1, idList.len);
        }
        else
        {
            sprintf(LINE(2), "%-8s%3bu/%-2bu", "��ͨ�û�", idIndex + 1, idList.len);
        }

        sprintf(LINE(3), "Press 16 exit");

        key = KeyScan();
        while (KeyScan())
            ;
        if (key == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        else if (key == 15)
        {
            stateNext = ST_MENU;
            return;
        }
        else if (key == 2)
        {
            idIndex++;
            if (idIndex >= idList.len)
            {
                idIndex = idList.len;
            }
        }
        else if (key == 1)
        {
            idIndex--;
            if (idIndex > MAXCARD)
            {
                idIndex = 0;
            }
        }
        else if (key == 3)
        {
            idList.rights[idIndex] = R_GENERIC;
        }
        else if (key == 4)
        {
            idList.rights[idIndex] = R_ADMIN;
        }
    }
}
void stateDate()
{
    uint8 key, i = 0, j, k;
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));

    i = 0;
    j = 1;
    while (1)
    {
        j = (i + 1) % 15;
        k = (i + 2) % 15;
        sprintf(LINE(0), "��  ʪ  ��%3bu/15", i);
        sprintf(LINE(1), "%2bu  %2bu %5.1f", his_info.temp[i], his_info.humi[i], his_info.light[i]);
        sprintf(LINE(2), "%2bu  %2bu %5.1f", his_info.temp[j], his_info.humi[j], his_info.light[j]);
        sprintf(LINE(3), "%2bu  %2bu %5.1f", his_info.temp[k], his_info.humi[k], his_info.light[k]);

        flush();
        key = KeyScan();
        while (KeyScan())
            ;
        if (key == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        else if (key == 1)
        {
            i = (i + 2) % 15;
        }
        else if (key == 2)
        {
            i = (i - 2) % 15;
        }
        else if (key == 15)
        {
            stateNext = ST_MENU;
            return;
        }
    }
}

void stateNew()
{
    uint8 rfstat = 1, flag = 0, key;
    uint32 card;

    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "    ��ˢ��");
    sprintf(LINE(3), "Press 16 exit");
    while (1)
    {
        flush();
        rfstat = PcdFindcard(&id);
        if (rfstat)
        {
            memset(ScreenBuf, ' ', sizeof(ScreenBuf));
            sprintf(LINE(0), "��ˢ��");
            sprintf(LINE(3), "Press 16 exit");
            flush();
        }
        else
        {
            card = id.u32;
            flag = findCard(card);

            memset(ScreenBuf, ' ', sizeof(ScreenBuf));
            sprintf(LINE(0), "%-15s", "��⵽��");
            if (flag == R_ADMIN)
            {
                sprintf(LINE(1), "%-15s", "����");
            }
            else if (flag == R_GENERIC)
            {
                sprintf(LINE(1), "%-15s", "�û���");
            }
            else if (flag == R_NONE)
            {
                sprintf(LINE(1), "%-15s", "���û���");
                // addCard(card);
            }
            sprintf(LINE(2), "ID:%lx", card);
            sprintf(LINE(3), "exit:16 new:14");
            flush();
        }

        key = KeyScan();
        if (key == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        else if (key == 14)
        {
            if (flag == R_NONE)
            {
                addCard(card);
            }
        }
        else if (key == 15)
        {
            stateNext = ST_MENU;
            return;
        }
    }
}

void stateLight()
{
    uint8 key;
    uint8 line = 0;
    uint8 select = 0;
    uint8 buf[10];
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));

    while (1)
    {
        sprintf(LINE(0), "%-8s %3bu", "������", LED_H);
        sprintf(LINE(1), "%-8s %3bu", "������", LED_M);
        sprintf(LINE(2), "%-8s %3bu", "������", LED_L);
        sprintf(LINE(3), "%-8s %5.1f", "������ֵ", light_th);
        flush();
        key = KeyScan();
        if (key == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        else if (key == 15)
        {
            stateNext = ST_MENU;
            return;
        }
        else if (key == 9)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(LINE(0), "%-15s", "������");
            flush();
            KeyInput(buf, 3, 9, 15, 5, 0);
            LED_H = atoi(buf);
        }
        else if (key == 10)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(LINE(1), "%-15s", "������");
            flush();
            KeyInput(buf, 3, 10, 15, 5, 1);
            LED_M = atoi(buf);
        }
        else if (key == 11)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(LINE(2), "%-15s", "������");
            flush();
            KeyInput(buf, 3, 11, 15, 5, 2);
            LED_L = atoi(buf);
        }
        else if (key == 12)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(LINE(3), "%-15s", "������ֵ");
            flush();
            KeyInput(buf, 5, 12, 15, 5, 3);
            light_th = (float)atoi(buf);
        }
    }
}
void stateTemp()
{
    uint8 key;
    uint8 line = 0;
    uint8 select = 0;
    uint8 buf[10];
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(2), "%-8s %bu", "������ֵ", temp_th);
    sprintf(LINE(3), "exit:16 Y:15");

    while (1)
    {
        sprintf(LINE(2), "%-8s %bu", "������ֵ", temp_th);
        flush();
        key = KeyScan();
        if (key == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        else if (key == 15)
        {
            stateNext = ST_MENU;
            return;
        }
        else if (key == 9)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(LINE(2), "%-15s", "������ֵ");
            flush();
            KeyInput(buf, 5, 9, 15, 5, 2);
            temp_th = (uint8)atoi(buf);
        }
    }
}

void stateHumi()
{
    uint8 key;
    uint8 line = 0;
    uint8 select = 0;
    uint8 buf[10];
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "    ʪ����ֵ");
    sprintf(LINE(2), "%-8s %bu", "������ֵ", humi_th);
    sprintf(LINE(3), "exit:16 Y:15");

    while (1)
    {
        sprintf(LINE(2), "%-8s %bu", "������ֵ", humi_th);
        flush();
        key = KeyScan();
        if (key == 16)
        {
            stateNext = ST_DEFAULT;
            return;
        }
        else if (key == 15)
        {
            stateNext = ST_MENU;
            return;
        }
        else if (key == 9)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(LINE(2), "%-15s", "������ֵ");
            flush();
            KeyInput(buf, 5, 9, 15, 5, 2);
            humi_th = (uint8)atoi(buf);
        }
    }
}