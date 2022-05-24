#include "main.h"

#ifndef __MAIN_H_
// DS1302引脚定义,可根据实际情况自行修改端口定义
sbit DS1302_RST = P3 ^ 5;
sbit DS1302_DAT = P3 ^ 6;
sbit DS1302_CLK = P3 ^ 7;
#endif
// DS1302地址定义
#define ds1302_sec_add 0x80      //秒数据地址
#define ds1302_min_add 0x82      //分数据地址
#define ds1302_hr_add 0x84       //时数据地址
#define ds1302_date_add 0x86     //日数据地址
#define ds1302_month_add 0x88    //月数据地址
#define ds1302_day_add 0x8a      //星期数据地址
#define ds1302_year_add 0x8c     //年数据地址
#define ds1302_control_add 0x8e  //控制数据地址
#define ds1302_charger_add 0x90
#define ds1302_clkburst_add 0xbe

typedef struct {
  uint8 year_h;
  uint8 year_l;
  uint8 month;
  uint8 day;
  uint8 hour;
  uint8 minute;
  uint8 second;
  uint8 week;
} DS1302_BCD;

typedef union {
  DS1302_BCD name;
  uint8 array[8];
} DS1302_union;
#define USE_ds1302_write_time 1
#define USE_ds1302_read_time 1
void ds1302_init(void);                     // DS1302初始化函数
void ds1302_write_time(DS1302_union *buf);  //向DS302写入时钟数据
void ds1302_read_time(DS1302_union *buf);   //从DS302读出时钟数据
