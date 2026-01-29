#ifndef __INT_DS1302_H__
#define __INT_DS1302_H__

#include <REGX52.H>
#include "../Com/Com_Util.h"

#define DS1302_SECOND          0x80
#define DS1302_MINUTES         0x82
#define DS1302_HOUR            0x84
#define DS1302_DAY             0x86
#define DS1302_MONTH           0x88
#define DS1302_DAY_OF_WEEK     0x8A
#define DS1302_YEAR            0x8C
#define DS1302_WP              0x8E

sbit DS1302_SCLK = P3^6;
sbit DS1302_IO = P3^4;
sbit DS1302_CE = P3^5;

typedef struct {
    u8 second;
    u8 minute;
    u8 hour;
    u8 day;     // 日（月份中的第几天）
    u8 month;
    u8 year;
    u8 day_of_week;      // 星期几
} Struct_Date;

void Int_DS1302_init();
void Int_DS1302_ReadTime(Struct_Date *p_st_date);
void Int_DS1302_WriteTime(Struct_Date *p_st_date);

#endif // __INT_DS1302_H__