#include "ds1302.h"

static void ds1302_SPI_SendByte(uint8 byte) {
  uint8 i;

  GLOBAL_IT_CLOSE();

  for (i = 0; i < 8; i++) {
    DS1302_DAT = byte & 0x01;
    DS1302_CLK = 1;
    DS1302_CLK = 0;
    byte >>= 1;
  }

  GLOBAL_IT_OPEN();
}

//从DS1302读出一字节数据
static uint8 ds1302_read_byte(uint8 addr) {
  uint8 i, temp;
  DS1302_RST = 1;  //启动DS1302总线
  //写入目标地址：addr
  addr = addr | 0x01;  //最低位置高，寄存器0位为0时写，为1时读
#if 1
  ds1302_SPI_SendByte(addr);
#else

  GLOBAL_IT_CLOSE();
  for (i = 0; i < 8; i++) {
    if (addr & 0x01) {
      IO_ds1302 = 1;
    } else {
      IO_ds1302 = 0;
    }
    SCK_ds1302 = 1;
    SCK_ds1302 = 0;
    addr = addr >> 1;
  }

#endif
  //输出数据：temp
  GLOBAL_IT_CLOSE();
  DS1302_RST = 1;
  for (i = 0; i < 8; i++) {
    temp = temp >> 1;
    if (DS1302_DAT) {
      temp |= 0x80;
    } else {
      temp &= 0x7F;
    }
    DS1302_CLK = 1;
    DS1302_CLK = 0;
  }
  DS1302_RST = 0;  //停止DS1302总线
  GLOBAL_IT_OPEN();
  return temp;
}

// DS1302初始化函数
void ds1302_init(void) {
  DS1302_RST = 0;  // RST脚置低
  DS1302_CLK = 0;  // SCK脚置低
}
#if USE_ds1302_write_time

//向DS1302写入一字节数据
static void ds1302_write_byte(uint8 addr, uint8 d) {
  DS1302_RST = 1;  //启动DS1302总线
  //写入目标地址：addr
  addr = addr & 0xFE;  //最低位置零，寄存器0位为0时写，为1时读
  GLOBAL_IT_CLOSE();
#if 1
  ds1302_SPI_SendByte(addr);
  ds1302_SPI_SendByte(d);
#else
  uint8 i;
  for (i = 0; i < 8; i++) {
    if (addr & 0x01) {
      IO_ds1302 = 1;
    } else {
      IO_ds1302 = 0;
    }
    SCK_ds1302 = 1;  //产生时钟
    SCK_ds1302 = 0;
    addr = addr >> 1;
  }

  //写入数据：d
  for (i = 0; i < 8; i++) {
    if (d & 0x01) {
      IO_ds1302 = 1;
    } else {
      IO_ds1302 = 0;
    }
    SCK_ds1302 = 1;  //产生时钟
    SCK_ds1302 = 0;
    d = d >> 1;
  }

#endif
  DS1302_RST = 0;  //停止DS1302总线
  GLOBAL_IT_OPEN();
}

//向DS302写入时钟数据
void ds1302_write_time(DS1302_union *buf) {
  ds1302_write_byte(ds1302_control_add, 0x00);  //关闭写保护
  ds1302_write_byte(ds1302_sec_add, 0x80);      //暂停时钟

  ds1302_write_byte(ds1302_year_add, buf->name.year_l);  //年
  ds1302_write_byte(ds1302_month_add, buf->name.month);  //月
  ds1302_write_byte(ds1302_date_add, buf->name.day);     //日
  ds1302_write_byte(ds1302_hr_add, buf->name.hour);      //时
  ds1302_write_byte(ds1302_min_add, buf->name.minute);   //分
  ds1302_write_byte(ds1302_sec_add, buf->name.second);   //秒
  ds1302_write_byte(ds1302_day_add, buf->name.week);     //周
  ds1302_write_byte(ds1302_control_add, 0x80);           //打开写保护
}
#endif
#if USE_ds1302_read_time
//从DS302读出时钟数据
void ds1302_read_time(DS1302_union *buf) {
  buf->name.year_l = ds1302_read_byte(ds1302_year_add);  //年
  buf->name.month = ds1302_read_byte(ds1302_month_add);  //月
  buf->name.day = ds1302_read_byte(ds1302_date_add);     //日
  buf->name.hour = ds1302_read_byte(ds1302_hr_add);      //时
  buf->name.minute = ds1302_read_byte(ds1302_min_add);   //分
  buf->name.second = (ds1302_read_byte(ds1302_sec_add)) &
                     0x7f;  //秒，屏蔽秒的第7位，避免超出59
  buf->name.week = ds1302_read_byte(ds1302_day_add);  //周
}
#endif
