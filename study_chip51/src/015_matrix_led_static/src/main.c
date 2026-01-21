#include "./Dri/Dri_UART.h"
#include "./Com/Com_Util.h"
#include "./Dri/Dri_Timer0.h"
#include <STRING.H>
#include <INTRINS.H>

#include <REGX52.H>

// 重新定义位名
sbit RCK = P3^5;
sbit SRK = P3^6;
sbit SER = P3^4;


// 595发送位
void Hc_595_send_byte(u8 byte)
{
    u8 i;
    RCK = 0; // 起始低电平
    for (i = 0; i < 8; i++)
    {
        SRK = 0; // 起始低电平
        Com_Util_DelayMs(1);
        SER = byte&(0x80 >> i); // 取得最高位的逻辑电平
        Com_Util_DelayMs(1);
        SRK = 1; // 上升沿移位
    }
    Com_Util_DelayMs(1);
    RCK = 1; // 上升沿8位锁存
}

// 点阵屏扫描
void Matrix_led_scan_col(u8 col, u8 byte)
{
    Hc_595_send_byte(byte);
    // MATRIX_LED_PORT=~(0x80>>Column); // 1000 0000位移1位后取反 1011 1111，可以实现相同的逻辑
    P0 = _cror_(0x7f,col); // 0111 1111
    Com_Util_DelayMs(1);	//延时
    P0=0xff;	//位清0
}


void main()
{
    u8 i;
    u8 offset = 0;
    P0=0xff;	//位清0
    while (1) {
        Matrix_led_scan_col(0,0x30); // 0011 0000 
        Matrix_led_scan_col(1,0x48); // 0100 1000 
        Matrix_led_scan_col(2,0x44); // 0100 0100 
        Matrix_led_scan_col(3,0x22); // 0010 0010 
        Matrix_led_scan_col(4,0x22); // 0010 0010 
        Matrix_led_scan_col(5,0x44); // 0100 0100 
        Matrix_led_scan_col(6,0x48); // 0100 1000
        Matrix_led_scan_col(7,0x30); // 0011 0000
    }
}

