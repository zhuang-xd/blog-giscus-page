#include "Int/LCD1602.h"
#include <REGX52.H>
#include "Com/Com_Util.h"
#include "INTRINS.H"


#define FOSC 11059200L
#define T1MS (65536-FOSC/12/1000)

#define LED_LEFT  0
#define LED_RIGHT 1


u8 led_direction = LED_RIGHT;

void Timer0_start()
{
    TMOD = 0x01;      // 定时器模式-位数 0000 0010
    TL0 = T1MS;       // 初始值 低8位
    TH0 = T1MS >> 8;  // 初始值 高8位
    TF0 = 1;          // 清除

    EA = 1;           // 中断总开关
    ET0 = 1;          // 定时器0中断开关
	
    TR0 = 1;          // 定时器0开始计时
}


void Timer0_run () interrupt 1
{
	static u16 cnt = 0;
	TL0 = T1MS;      // 重置初始值 低8位
    TH0 = T1MS >> 8; // 重置初始值 高8位
	cnt++;
	if (cnt > 500) {
		cnt = 0;

		switch (led_direction)
		{
			case LED_LEFT: P2 = _cror_(P2,1); break;
			case LED_RIGHT: P2 = _crol_(P2,1); break;
		}
	}
}
	
void main()
{
	P2 = 0xFE;
	led_direction = LED_LEFT;
	
	Timer0_start();
	while (1)
	{		
		if(P3_1 == 0) {
			Com_Util_DelayMs(10); while (P3_1 == 0); Com_Util_DelayMs(10);
			led_direction = LED_LEFT;
		}
		if(P3_0 == 0) {
			Com_Util_DelayMs(10); while (P3_0 == 0); Com_Util_DelayMs(10);
			led_direction = LED_RIGHT;
		}
	}
	
}
