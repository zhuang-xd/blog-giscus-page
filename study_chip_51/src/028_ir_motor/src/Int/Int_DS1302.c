#include "Int_DS1302.h"
void Int_DS1302_WriteByte(u8 cmd_byte, u8 data_byte);
// u8 Int_DS1302_read_byte(u8 cmd_byte);

// 十进制转 BCD 码
u8 DEC_to_BCD(u8 dec)
{
    // return ((dec / 10) << 4) | (dec % 10);
    return dec/10*16+dec%10;
}

// BCD 码转十进制
u8 BCD_to_DEC(u8 bcd)
{
    // return ((bcd >> 4) * 10) + (bcd & 0x0F);
    return bcd/16*10+bcd%16;//BCD码转十进制后读取
}

void Int_DS1302_init()
{
    // init
    DS1302_SCLK = 0;
    DS1302_CE = 0;
    DS1302_IO = 0;
}

u8 Int_DS1302_ReadByte(u8 cmd_byte)
{
    u8 i, data_byte = 0;

    DS1302_CE = 1;
    DS1302_SCLK = 0;

    // DS1302的寄存器
    for (i = 0; i < 8; i++)
    {
        if (i == 0) {
            DS1302_IO = 1; // 1读 0写
        } else {
            DS1302_IO = cmd_byte & (0x01 << i); // 取地址值第i位（先低后高）
        }
        DS1302_SCLK = 0; // 这一个下降沿已经有DS1302发送来的数据了，所以下面先读取IO口的数据
        DS1302_SCLK = 1; // 默认低电平，上升沿前准备好要发送的数据
    }
    
    // 要读取的字节
    for (i = 0; i < 8; i++)
    {        
        DS1302_SCLK = 1; // 下降沿后接收数据
        DS1302_SCLK = 0; 
        if (DS1302_IO) {
            data_byte |= (0x01 << i); // 先读低位
        }
    }
    
    DS1302_CE = 0;
    DS1302_IO = 0;

    return data_byte;
}

void Int_DS1302_WriteByte(u8 cmd_byte, u8 data_byte)
{
    u8 i;

    DS1302_CE = 1;

    // DS1302的寄存器
    for (i = 0; i < 8; i++)
    {
        if (i==0) {
            DS1302_IO = 0; // 1读 0写
        } else {
            DS1302_IO = cmd_byte & (0x01 << i); // 取地址值第i位（先低后高）
        }
        DS1302_SCLK = 0;
        DS1302_SCLK = 1; // 默认低电平，上升沿前准备好要发送的数据
    }

    // 要写入的字节
    for (i = 0; i < 8; i++)
    {
        DS1302_IO = data_byte & (0x01 << i);
        DS1302_SCLK = 0;
        DS1302_SCLK = 1; // 默认低电平，上升沿前准备好要发送的数据
    }
    DS1302_SCLK = 0;

    DS1302_CE = 0;
}

void Int_DS1302_WriteTime(Struct_Date *p_st_date)
{
    Int_DS1302_WriteByte(DS1302_WP,0x00);	//关闭写保护
	Int_DS1302_WriteByte(DS1302_YEAR,DEC_to_BCD(p_st_date->year));
	Int_DS1302_WriteByte(DS1302_MONTH,DEC_to_BCD(p_st_date->month));
	Int_DS1302_WriteByte(DS1302_DAY,DEC_to_BCD(p_st_date->day));
	Int_DS1302_WriteByte(DS1302_HOUR,DEC_to_BCD(p_st_date->hour));
	Int_DS1302_WriteByte(DS1302_MINUTES,DEC_to_BCD(p_st_date->minute));
	Int_DS1302_WriteByte(DS1302_SECOND,DEC_to_BCD(p_st_date->second));
	Int_DS1302_WriteByte(DS1302_DAY_OF_WEEK,DEC_to_BCD(p_st_date->day_of_week));
	Int_DS1302_WriteByte(DS1302_WP,0x80);	//打开写保护
}

void Int_DS1302_ReadTime(Struct_Date *p_st_date)
{
    u8 tmp;
   
    tmp = Int_DS1302_ReadByte(DS1302_SECOND);
    p_st_date->second = BCD_to_DEC(tmp);
    tmp = Int_DS1302_ReadByte(DS1302_MINUTES);
    p_st_date->minute = BCD_to_DEC(tmp);
    tmp = Int_DS1302_ReadByte(DS1302_HOUR);
    p_st_date->hour = BCD_to_DEC(tmp);
    tmp = Int_DS1302_ReadByte(DS1302_MONTH);
    p_st_date->month = BCD_to_DEC(tmp);
    tmp = Int_DS1302_ReadByte(DS1302_DAY);
    p_st_date->day = BCD_to_DEC(tmp);
    tmp = Int_DS1302_ReadByte(DS1302_YEAR);
    p_st_date->year = BCD_to_DEC(tmp);
    tmp = Int_DS1302_ReadByte(DS1302_DAY_OF_WEEK);
    p_st_date->day_of_week = BCD_to_DEC(tmp);

}