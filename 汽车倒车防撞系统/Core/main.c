#include "main.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "adc.h"
#include "delay.h"
#include "eeprom.h"
#include "hcsr04.h"
#include "lcd12864_s.h"
#include "matkey.h"
#include "pwm.h"
#include "speedometer.h"
#include "usart.h"
typedef struct
{
    uint8 flag;
    float max;
    float min;
    float dis_th;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;
    uint8 len;
    float pos[10];

} info_t;
uint8 stateCur, statePre, stateNext;
uint8 inputMode = FromHC;
uint8_t ScreenBuf[72]; //屏幕缓存
info_t info;

uint8 pwm1, pwm2;
uint16 spdTime;
float spdFreq, distence = 1;
float pos1, pos2, speed;
uint32 timecount, ledcount, savecount;
uint8 alarmFLag;

#define N 18
#define LINE(x) ScreenBuf + x *N
#define flush() lcd12864_DDRAM_Flush(ScreenBuf, N)
#define ClearBuf() memset(ScreenBuf, ' ', sizeof(ScreenBuf))
#define WaitKey() while (KeyScan())
#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b
#define LED_ON() LED = 0, BEEP = 1
#define LED_OFF() LED = 1, BEEP = 0

void BootAnime()
{
    sprintf(LINE(0), "      H017");
    flush();
    delayms(3000);
}

void stateDefault()
{
    uint8 key, i, j;
    uint16 temp;
    stateNext = ST_Default;
    key = KeyScan();
    if (key == 12)
    {
        inputMode = (inputMode == FromHC) ? FromAD : FromHC;
        WaitKey();
    }
    if (key == 15)
        info.max = 0.f;
    if (key == 14)
        info.min = 100.f;
    if (key == 13)
    {
        WaitKey();
        pos1 = pos2;
        pos2 = distence;
        speed = (pos2 - pos1) * 1000 / timecount;
        timecount = 0;
        printf("%f %f %f\r\n", pos1, pos2, speed);
        for (i = 9; i >= 1; i--)
        {
            info.pos[i] = info.pos[i - 1];
        }
        info.pos[0] = pos2;
    }
    if (key == 1)
    {
        stateNext = ST_CLOCK;
        return;
    }
    if (key == 3)
    {
        stateNext = ST_SET;
        return;
    }
    if (key == 16)
    {
        WaitKey();
        stateNext = ST_MENU;
        return;
    }

    ClearBuf();
    if (inputMode == FromAD)
    {
        distence = GetADCResult(0) / 102.3f;
        sprintf(LINE(0), "DIS:%4.2f m(AD)", distence);
    }
    else
    {
        temp = HCSR04_GetDist();
        if (temp != 0xffff)
        {
            distence = temp / 1000.0f;
            sprintf(LINE(0), "DIS:%4.2f m(HC)", distence);
        }
        else
        {
            sprintf(LINE(0), "DIS:inf    m(HC)", distence);
        }
    }
    SpdOt_GetTimeFreq(&spdTime, &spdFreq);
    sprintf(LINE(1), "转速:%6.2f  ", spdFreq);

    info.max = MAX(distence, info.max);
    info.min = MIN(distence, info.min);
    sprintf(LINE(2), "a:%-4.2f i:%-4.2f", info.max, info.min);
    sprintf(LINE(3), "s:%4.2f p:%4.2f", speed, pos2);
    flush();
    if (distence < info.dis_th)
    {
        alarmFLag = 1;
    }
    else
    {
        alarmFLag = 0;
        LED_OFF();
    }

    if (distence > 1.2)
    {
        pwm2 = (distence - 1.2) * 20 + 50;
        PWM_SetDutyH0(0);
        PWM_SetDutyH1(pwm2);
    }
    else if (distence > 0.9)
    {
        PWM_SetDutyH0(0);
        PWM_SetDutyH1(0);
    }
    else
    {
        PWM_SetDutyH0(100);
        PWM_SetDutyH1(0);
    }
}

void stateClock()
{
    uint8 key;
    stateNext = ST_CLOCK;
    ClearBuf();
    sprintf(LINE(0), "      CLOCK     ");
    sprintf(LINE(1), "%2bu:%2bu:%2bu", info.hour, info.minute, info.second);
    flush();
    key = KeyScan();
    if (key == 2)
    {
        stateNext = ST_Default;
    }
    if (key == 3)
    {
        stateNext = ST_SET;
        return;
    }
    if (key == 16)
    {
        stateNext = ST_MENU;
    }
}

void stateSet()
{
    uint8 key;
    uint8 buf[15];
    stateNext = ST_SET;
    key = KeyScan();
    if (key == 1)
    {
        stateNext = ST_CLOCK;
        return;
    }
    if (key == 4)
    {
        stateNext = ST_Default;
        return;
    }
    if (key == 16)
    {
        stateNext = ST_MENU;
    }

    if (key == 15)
    {
        memset(buf, 0, sizeof(buf));
        KeyInput(buf, 8, 15, 14, 2, 0);
        info.dis_th = atof(buf);
    }
    ClearBuf();
    sprintf(LINE(0), "  设置报警阈值");
    sprintf(LINE(1), "当前阈值:%4.2f", info.dis_th);
    sprintf(LINE(2), "按键15输入");
    sprintf(LINE(3), "按键14确认");
    flush();
}

void stateMenu()
{
    uint8 key;
    stateNext = ST_MENU;
    ClearBuf();
    sprintf(LINE(0), "1.查看时钟");
    sprintf(LINE(1), "2.设置报警阈值");
    sprintf(LINE(2), "3.查看历史记录");
    sprintf(LINE(3), "4.返回");
    flush();
    key = KeyScan();
    WaitKey();
    if (key == 1)
    {
        stateNext = ST_CLOCK;
    }
    if (key == 2)
    {
        stateNext = ST_SET;
    }
    if (key == 3)
    {
        stateNext = ST_HIS;
    }
    if (key == 4)
    {
        stateNext = ST_Default;
    }
}

void stateHis()
{
    uint8 key;
    static uint8 i = 0;
    ClearBuf();
    sprintf(LINE(0), "历史记录, 15保存");
    sprintf(LINE(1), "%bu  %5.2f", i, info.pos[i]);
    sprintf(LINE(2), "%bu  %5.2f", i + 1, info.pos[i + 1]);
    sprintf(LINE(3), "%bu  %5.2f", i + 2, info.pos[i + 2]);
    flush();
    key = KeyScan();
    if (key == 2)
    {
        i += 2;
        if (i > 9)
            i = 0;
    }
    if (key == 1)
    {
        i -= 2;
        if (i > 9)
            i = 9;
    }
    if (key == 15)
    {
        IapEraseSector(SECTOR0_ADDRESS);
        eeprom_WriteData(SECTOR0_ADDRESS, &info, sizeof(info));
        ClearBuf();
        sprintf(LINE(0), "保存成功");
        flush();
    }
    if (key == 16)
    {
        stateNext = ST_MENU;
        return;
    }
}
void main(void)
{
    /*-------------------初始化---------------------------------*/
    P4SW = 0xff;              // P4端口均做IO（P4.7除外）
    P0M0 = 0x00, P0M1 = 0x00; // P0漏弱上拉，双向IO

    /*=============初始化=============*/
    Uart_Init();
    printf("hello\r\n");
    lcd12864_Init(); // lcd初始化
    HCSR04_Init();   //超神波初始化
    SpdOt_Init();    //测速初始化
    InitADC();
    PWM_Init();
    eeprom_ReadData(SECTOR0_ADDRESS, &info, sizeof(info));
    if (info.flag != 0x77)
    {
        info.flag = 0x77;
        info.max = 0.f;
        info.min = 100.f;
        info.hour = 0;
        info.minute = 0;
        info.second = 0;
        info.dis_th = 0.5;
    }
    else
    {
        printf("load success\r\n");
    }
    ClearBuf();

    stateCur = ST_Default;
    //  BootAnime();
    while (1)
    {

        statePre = stateCur;
        stateCur = stateNext;
        switch (stateCur)
        {
        case ST_Default:
            stateDefault();
            break;
        case ST_CLOCK:
            stateClock();
            break;
        case ST_SET:
            stateSet();
            break;
        case ST_MENU:
            stateMenu();
            break;
        case ST_HIS:
            stateHis();
            break;
        default:
            stateDefault();
            break;
        }

        if (savecount >= 4000)
        {
            IapEraseSector(SECTOR0_ADDRESS);
            eeprom_WriteData(SECTOR0_ADDRESS, &info, sizeof(info));
            savecount = 0;
        }
    }
}

/* Timer1 interrupt routine */
void tm1_isr() interrupt 3
{
    static uint8 count = 0;
    Spdot_TimIsrCallback();
    timecount += 5;
    if (++count >= 200)
    {
        count = 0;
        info.second++;
        if (info.second >= 60)
        {
            info.second = 0;
            info.minute++;
        }
        if (info.minute >= 60)
        {
            info.minute = 0;
            info.hour++;
        }
        if (info.hour >= 24)
        {
            info.hour = 0;
        }
    }
    if (alarmFLag)
    {
        if (count > 100)
        {
            LED_OFF();
        }
        else
        {
            LED_ON();
        }
    }
    ledcount++;
    savecount++;
}