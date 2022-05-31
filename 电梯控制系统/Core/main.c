#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "delay.h"
#include "lcd12864_s.h"
#include "matkey.h"
#include "stepMotor.h"
#include "usart.h"

#define StepPerFloor 4000
uint8_t ScreenBuf[72]; //��Ļ����

uint8 state_Cur, state_Next, state_Prev;

uint8 floorNow = 5;
uint8 floorList[11];

#define N 18
#define LINE(x) ScreenBuf + x *N
#define flush() lcd12864_DDRAM_Flush(ScreenBuf, N)

#define LED_ON() LED = 0, BEEP = 1
#define LED_OFF() LED = 1, BEEP = 0
#define RELAY_ON() RELAY = 0
#define RELAY_OFF() RELAY = 1
uint32 waitCount = 0, ledCount = 0;

/**
 * @brief 1ms ��ʱ�ж�
 *
 */
void tm0_isr() interrupt 1
{
    TL0 = 0xCD; //���ö�ʱ��ʼֵ
    TH0 = 0xD4; // reload timer0 high byte

    SM_MvOneStep();
    waitCount++;
    ledCount++;
}

/*��������*/
void BootAnime()
{
    uint8 i = 0;
    lcd12864_Init();
    delayms(500);
    lcd12864_DDRAM(0, 0, "     ZNDTKZQ");
    lcd12864_DDRAM(1, 0, "      0208");

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

void stateIDLE()
{
    uint8 key, i;

    state_Next = ST_IDLE; //Ĭ����һ״̬

    RELAY_OFF();
    key = KeyScan();
    while (KeyScan())
        ;
    if (key >= 1 && key <= 9)
    {
        floorList[key] = 1;
        if (floorList[5] == 2)
        {
            floorList[5] = 0;
        }
        state_Next = ST_WAIT;
    }
    floorNow = GetPos() / StepPerFloor;
    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "S:%-5s F:%2bu", "IDLE", floorNow);
    sprintf(LINE(1), "Pressed:%3bu", key);
    for (i = 1; i <= 9; i++)
    {
        ScreenBuf[2 * N + i] = i + '0';
        ScreenBuf[3 * N + i] = floorList[i] ? '+' : '-';
    }
    ScreenBuf[3 * N + floorNow] = '*';
    flush();
}

void stateUP()
{
    uint8 key, i;
    int32 aimPOS, newPOS;

    state_Next = ST_UP; //Ĭ����һ״̬

    RELAY_OFF();
    if (state_Prev == ST_WAIT)
    {
        ledCount = 0;
        LED_ON();
        while (ledCount < 1000)
            ;
        LED_OFF();
    }

    //��ⰴ�������¥�㵽����
    key = KeyScan();
    while (KeyScan())
        ;
    //�����Ŀ��
    if (key >= 1 && key <= 9)
    {
        floorList[key] = 1;
        if (floorList[5] == 2)
        {
            floorList[5] = 0;
        }
    }

    //����Ŀ��¥��
    floorNow = GetPos() / StepPerFloor;
    if (floorList[floorNow] && floorNow * StepPerFloor == GetPos())
    {
        floorList[floorNow] = 0; //ɾ��¥��
        state_Next = ST_WAIT;
        return;
    }

    //�������һ��ΪĿ��
    for (i = floorNow; i <= 9; i++)
    {
        if (floorList[i])
        {
            SetPos(i * StepPerFloor);
            break;
        }
    }

    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "S:%-5s F:%2bu", "UP", floorNow);
    sprintf(LINE(1), "Pressed:%3bu", key);
    for (i = 1; i <= 9; i++)
    {
        ScreenBuf[2 * N + i] = i + '0';
        ScreenBuf[3 * N + i] = floorList[i] ? '+' : '-';
    }
    ScreenBuf[3 * N + floorNow] = '*';
    flush();
}

void stateDOWN()
{
    uint8 key, i;
    int32 aimPOS, newPOS;

    state_Next = ST_DOWN; //Ĭ����һ״̬

    RELAY_OFF();
    if (state_Prev == ST_WAIT)
    {
        ledCount = 0;
        LED_ON();
        while (ledCount < 1000)
            ;
        LED_OFF();
    }

    //��ⰴ�������¥�㵽����
    key = KeyScan();
    while (KeyScan())
        ;
    //�����Ŀ��
    if (key >= 1 && key <= 9)
    {
        floorList[key] = 1;
        if (floorList[5] == 2)
        {
            floorList[5] = 0;
        }
    }

    //����Ŀ��¥��
    floorNow = GetPos() / StepPerFloor;
    if (floorList[floorNow] && floorNow * StepPerFloor == GetPos())
    {
        floorList[floorNow] = 0; //ɾ��¥��
        state_Next = ST_WAIT;
        return;
    }

    //�������һ��ΪĿ��
    for (i = floorNow; i >= 1; i--)
    {
        if (floorList[i])
        {
            SetPos(i * StepPerFloor);
            break;
        }
    }

    memset(ScreenBuf, ' ', sizeof(ScreenBuf));
    sprintf(LINE(0), "S:%-5s F:%2bu", "DOWN", floorNow);
    sprintf(LINE(1), "Pressed:%3bu", key);
    for (i = 1; i <= 9; i++)
    {
        ScreenBuf[2 * N + i] = i + '0';
        ScreenBuf[3 * N + i] = floorList[i] ? '+' : '-';
    }
    ScreenBuf[3 * N + floorNow] = '*';
    flush();
}
void stateWAIT()
{
    uint16 count;
    uint8 i, key;
    state_Next = state_Prev;

    waitCount = 0;
    ledCount = 0;
    LED_ON();
    RELAY_ON();
    while (waitCount < 2000)
    {

        //һ���������ѽ���
        if (ledCount >= 1000)
            LED_OFF();

        //��ⰴ�������¥�㵽����
        key = KeyScan();
        while (KeyScan())
            ;
        //�����Ŀ��
        if (key >= 1 && key <= 9)
        {
            floorList[key] = 1;
            if (floorList[5] == 2)
            {
                floorList[5] = 0;
            }
        }

        memset(ScreenBuf, ' ', sizeof(ScreenBuf));
        sprintf(LINE(0), "S:%-5s F:%2bu", "WAIT", floorNow);
        sprintf(LINE(1), "Pressed:%3bu", key);
        for (i = 1; i <= 9; i++)
        {
            ScreenBuf[2 * N + i] = i + '0';
            ScreenBuf[3 * N + i] = floorList[i] ? '+' : '-';
        }
        ScreenBuf[3 * N + floorNow] = '*';
        flush();
    }
    RELAY_OFF();

    floorNow = GetPos() / StepPerFloor;

    //����б��Ѿ���գ���ȴ����������У����а������£���������
    for (i = 1; i <= 9; i++)
    {
        if (floorList[i])
        {
            break;
        }
    }
    //�б��ѿ�
    if (i == 10)
    {
        if (floorNow == 5)
        {
            state_Next = ST_IDLE;
            return;
        }
        waitCount = 0;
        //�ȴ�3�룬�ް������¿���
        while (waitCount <= 3000 && !KeyScan())
        {
            memset(ScreenBuf, ' ', sizeof(ScreenBuf));
            sprintf(LINE(0), "S:%-5s F:%2bu", "WAIT", floorNow);
            sprintf(LINE(1), "Idle...%.3f", waitCount / 1000.f);
            for (i = 1; i <= 9; i++)
            {
                ScreenBuf[2 * N + i] = i + '0';
                ScreenBuf[3 * N + i] = floorList[i] ? '+' : '-';
            }
            ScreenBuf[3 * N + floorNow] = '*';
            flush();
        }
        key = KeyScan();
        //û�а������� ���˶��������
        if (key == 0)
        {
            //���ڵ����
            if (floorNow == 5)
            {
                state_Next = ST_IDLE;
                return;
            }
            //���ڵ����
            else
            {
                floorList[5] = 2;
            }
        }
        else
        {
            floorList[key] = 1;
        }
    }

    //��һ״̬Ϊ���������и��ϲ��Ŀ�꣬���������������½�
    if (state_Prev == ST_UP)
    {
        for (i = floorNow; i <= 9; i++)
        {
            if (floorList[i])
            {
                state_Next = ST_UP;
                return;
            }
        }
        state_Next = ST_DOWN;
    }

    //��һ״̬Ϊ�½�,���޸��²��Ŀ�꣬����
    if (state_Prev == ST_DOWN || state_Next == ST_DOWN)
    {
        for (i = floorNow; i >= 1; i--)
        {
            if (floorList[i])
            {
                state_Next = ST_DOWN;
                return;
            }
        }
        state_Next = ST_UP;
    }

    if (state_Prev == ST_IDLE)
    {
        for (i = 1; i <= 9; i++)
        {
            if (floorList[i] && i != 5)
            {
                if (i < 5)
                {
                    state_Next = ST_DOWN;
                }
                else if (i > 5)
                {
                    state_Next = ST_UP;
                }
                return;
            }
        }
        state_Next = ST_WAIT;
    }
}
void main(void)
{
    uint8 i;

    /*-------------------��ʼ��---------------------------------*/
    P4SW = 0xff;              // P4�˿ھ���IO��P4.7���⣩
    P0M0 = 0x00, P0M1 = 0x00; // P0©��������˫��IO
    P2M0 = 0x00, P2M1 = 0x00;

    /*=============��ʼ��=============*/
    tm0_init();
    Uart_Init();
    lcd12864_Init();

    printf("hello\r\n");
    state_Cur = ST_IDLE;
    state_Next = ST_IDLE;

    ResetPos(5 * StepPerFloor);
    memset(floorList, 0, sizeof(floorList));
    /*=============��������=============*/
    // BootAnime();

    /*=============��ѭ��=============*/
    while (1)
    {
        state_Prev = state_Cur;
        state_Cur = state_Next;

        switch (state_Cur)
        {
        case ST_IDLE:
            stateIDLE();
            break;
        case ST_UP:
            stateUP();
            break;
        case ST_DOWN:
            stateDOWN();
            break;
        case ST_WAIT:
            stateWAIT();
            break;
        default:
            break;
        }
    }
}
