#include "Int_Matrix_led.h"
#include <REGX52.H>
#include <STRING.H>
#include <INTRINS.H>

// 重新定义位名
sbit RCK = P3^5;
sbit SRK = P3^6;
sbit SER = P3^4;

void Int_init_matrix_led()
{
    RCK = 0; // 起始低电平
    SRK = 0; // 起始低电平
}

// 点阵屏扫描
void Int_matrix_led_scan_col(u8 col, u8 byte)
{
    Int_74hc595_send_byte(byte);
    P0=~(0x80>>col); // 1000 0000位移1位后取反 1011 1111，可以实现相同的逻辑
    // P0 = _cror_(0x7f,col); // 0111 1111
    Com_Util_DelayMs(1);	//延时
    P0=0xff;	//位清0
}

// 595发送位
void Int_74hc595_send_byte(u8 byte)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        SER = byte&(0x80 >> i); // 取得最高位的逻辑电平
        SRK = 1; // 上升沿移位
        SRK = 0; // 上升沿移位
    }
    RCK = 1; // 上升沿8位锁存
    RCK = 0; // 上升沿8位锁存
}