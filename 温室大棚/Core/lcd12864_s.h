#ifndef __12864DISPLAY_H_
#define __12864DISPLAY_H_

#include "delay.h"
#include "main.h"

//��������������:RS = CS  RW = SID_lcd  E = SCLK_lcd

/*****LCD�ӿڶ���*****/
#define LCD12864_SID LCD_SID
#define LCD12864_SCLK LCD_SCLK

#define USEPSB 0 // PSB�ӵؾͲ���Ҫ��������
#define USECS 0

#define USE_MACRO 1
#define USE_lcd12864_DDRAM 1
#define USE_lcd12864_DDRAM_Flush 1
#define USE_lcd12864_GDRAM 0
#define USE_lcd12864_HCDisp 0
#define USE_lcd12864_HCDisp2 1
extern uint8_t code DDRAM_POS[8];

#if USEPSB

sbit PSB = P2 ^ 3; //������ѡ�� 1:����ģʽ; 0:����ģʽ;
#endif
#if USECS
sbit CS = P4 ^ 0; //Ƭѡ�ź�
#endif

/*****LCD���ܳ�ʼ��ָ��*****/
#define CLEAR_SCREEN 0x01 //����ָ�������ACֵΪ00H
#define AC_INIT 0x02      //��AC����Ϊ00H�����α��Ƶ�ԭ��λ��
#define CURSE_ADD 0x06 //�趨�α��Ƶ�����ͼ�������ƶ�����Ĭ���α����ƣ�ͼ�����岻����
#define FUN_MODE 0x30  //����ģʽ��8λ����ָ�
#define DISPLAY_ON 0x0c  //��ʾ��,��ʾ�α꣬���α�λ�÷���
#define DISPLAY_OFF 0x08 //��ʾ��
#define CURSE_DIR 0x14   //�α������ƶ�:AC=AC+1
#define SET_CG_AC 0x40   //����AC����ΧΪ��00H~3FH
#define SET_DD_AC 0x80

#define LCDOPT_CMD 0
#define LCDOPT_DATA 1
/*���⺯����꺯��*/
void lcd12864_Init();     //��ʼ��
void lcd12864_Wait(void); //�ȴ�lcd12864�˳�busy״̬
void lcd12864_WriteOpt(uint8 option,
                       uint8 byte); //������Ϣ ָ�opt=0 ���ݣ�opt=1

#if USE_MACRO
#define lcd12864_WriteCmd(byte) lcd12864_WriteOpt(LCDOPT_CMD, byte)   //дָ��
#define lcd12864_WriteData(byte) lcd12864_WriteOpt(LCDOPT_DATA, byte) //д����
#else
void lcd12864_WriteCmd(uint8_t Cbyte);  //дָ��
void lcd12864_WriteData(uint8_t Dbyte); //д����
#endif

/*����ָ��*/
#define lcd12864_SetAC(x, y) lcd12864_WriteCmd(DDRAM_POS[y] + x)
#define lcd12864_CursorOn() lcd12864_WriteCmd(0x0f)
#define lcd12864_CursorOff() lcd12864_WriteCmd(0x0c)
#define lcd12864_CursorACC() lcd12864_WriteCmd(CURSE_DIR)

/*��Ļ��ʾ*/
void lcd12864_DDRAM(uint8_t m, uint8_t n, uint8_t *s);
void lcd12864_DDRAM_plus(uint8_t m, uint8_t n, uint8_t *s, uint8 len);
void lcd12864_DDRAM_Flush(uint8_t *str, uint8_t lineLength); //ˢд������Ļ
void lcd12864_GDRAM(uint8_t *pic);
void lcd12864_HCDisp2(unsigned char *buf, char *str1, char *str2, char len, char pos_begin1, char pos_end1,
                      char pos_begin2, char pos_end2,
                      unsigned int ms); //���������ʾһ���ַ�
#endif