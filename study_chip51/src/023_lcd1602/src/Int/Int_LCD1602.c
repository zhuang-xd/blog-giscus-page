#include "Int_LCD1602.h"
#include "REGX52.H"
#include "STRING.H"

#define LCD1602_SDA P0
sbit LCD1602_EN = P2^7; // 1读  下降沿执行指令
sbit LCD1602_RS = P2^6; // 0指令 1数据
sbit LCD1602_RW = P2^5; // 0写   1读

void Int_LCD_WriteCmd(u8 byte);
void Int_LCD_WriteData(u8 byte);

void Int_LCD_Init()
{
    Int_LCD_WriteCmd(0x38); // 设置8位格式
    Int_LCD_WriteCmd(0x0c); // 整体写入
    Int_LCD_WriteCmd(0x06); // 设置输入
    Int_LCD_WriteCmd(0x01); // 清屏
    Com_Util_DelayMs(1);
}

void Int_LCD_WriteCmd(u8 byte)
{
    LCD1602_SDA=byte; 
    LCD1602_RS=0;               //指令
    LCD1602_RW=0;               //写入
    LCD1602_EN=1;
    Com_Util_DelayMs(1);
    LCD1602_EN=0;               //下降沿
    Com_Util_DelayMs(1);
}

void Int_LCD_WriteData(u8 byte)
{
    LCD1602_SDA=byte; 
    LCD1602_RS=1;               //数据
    LCD1602_RW=0;               //写入
    LCD1602_EN=1;
    Com_Util_DelayMs(1);
    LCD1602_EN=0;               //下降沿
    Com_Util_DelayMs(1);
}

void Int_LCD_ShowChar(u8 line, u8 pos, u8 ch)
{
    u8 addr;
    
    addr = (line == 0) ? 0x80+pos : 0xc0+pos; // 0xc0 = 0x80 + 0x40
    Int_LCD_WriteCmd(addr);
    Int_LCD_WriteData(ch);
}


void Int_LCD_ShowString(u8 line, u8 pos, u8 str[])
{
    u8 i;
    u8 len = strlen(str);
    for (i = 0; i < len; i++)
    {
        Int_LCD_ShowChar(line,pos+i,str[i]);
    }
}

void Int_LCD_ShowNum(u8 line, u8 pos, int num, u8 digit)
{
    u8 i;
    for (i = 0; i < digit; i++)
    {
        // digit - 1因为pos的偏移从0，而位数从1开始
        Int_LCD_ShowChar(line,pos + digit - 1 - i,num % 10+'0');
        num /= 10;
    }
}

void Int_LCD_ShowSignedNum(u8 line, u8 pos, int num, u8 digit)
{
    if (num < 0) {
        Int_LCD_ShowChar(line,pos, '-'); // -
        num = -num;
    }else {
        Int_LCD_ShowChar(line,pos, '+'); // +
    }
    Int_LCD_ShowNum(line,pos+1,num,digit);
}

void Int_LCD_ShowHexNum(u8 line, u8 pos, int num, u8 digit)
{
    u8 i;
    u8 temp;

    Int_LCD_ShowChar(line,pos,'0');
    Int_LCD_ShowChar(line,pos+1,'x');

    for (i = 0; i < digit; i++)
    {
        temp = num % 16;
        // digit - 1因为pos的偏移从0，而位数从1开始
        Int_LCD_ShowChar(line,pos + digit - 1 - i + 2,temp+'0');
            
        num /= 16;
    }
}

void Int_LCD_ShowBinNum(u8 line, u8 pos, int num, u8 digit)
{
    u8 i;
    u8 temp;

    for (i = 0; i < digit; i++)
    {
        temp = num % 2;
        // digit - 1因为pos的偏移从0，而位数从1开始
        Int_LCD_ShowChar(line,pos + digit - 1 - i,temp+'0');
            
        num /= 2;
    }
}