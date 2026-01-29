#include <REGX52.H>
#include "Int/Int_DS1302.h"
#include "Int/LCD1602.h"




// 修改后的主函数
void main()
{
	Struct_Date st_date = {0,};
	Struct_Date st_date_read = {0,};
    u8 write_success = 0;
    
    // 初始化时间（修正后）
    st_date.year    	= 26;    // 2026年
    st_date.month   	= 1;     // 1月
    st_date.day    		= 20;    // 20日（月份中的第几天）
    st_date.hour    	= 17;    // 15时
    st_date.minute  	= 50;    // 10分
    st_date.second  	= 25;    // 59秒
    st_date.day_of_week = 2;     // 星期二
    
    LCD_Init();
    Int_DS1302_init();
    
    // 写入时间
    Int_DS1302_WriteTime(&st_date);

    // 静态字符显示
    LCD_ShowString(1, 1, "  -  -  ");
    LCD_ShowString(2, 1, "  :  :  ");

	while(1)
	{
		Int_DS1302_ReadTime(&st_date_read);//读取时间

		// 显示日期
        LCD_ShowNum(1, 1, st_date_read.year, 2);
        LCD_ShowNum(1, 4, st_date_read.month, 2);
        LCD_ShowNum(1, 7, st_date_read.day, 2);
        LCD_ShowNum(2, 1, st_date_read.hour, 2);
        LCD_ShowNum(2, 4, st_date_read.minute, 2);
        LCD_ShowNum(2, 7, st_date_read.second, 2);
	}
}