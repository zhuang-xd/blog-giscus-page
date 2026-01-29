#include "Int/LCD1602.h"
#include <REGX52.H>
#include "Com/Com_Util.h"
#include "INTRINS.H"


#define FOSC 11059200L
#define T1MS (65536-FOSC/12/1000)

#define LED_LEFT  0
#define LED_RIGHT 1


u8 led_direction = LED_RIGHT;

u16 time = 0; //即使中断结束也要保留值

void Timer0_start()
{
    TMOD = 0x01;      // 定时器模式-位数 0000 0010
    TL0 = T1MS;       // 初始值 低8位
    TH0 = T1MS >> 8;  // 初始值 高8位
    TF0 = 1;          // 清除

    EA = 1;           // 中断总开关
    ET0 = 1;          // 定时器0中断开关
	
    TR0 = 1;          // 定时器0开始计时
	time = 0;
}


void Timer0_run () interrupt 1
{
	u16 hh = 0;
	u16 mm = 0;
	u16 ss = 0;
	static u16 cnt = 0;
	TL0 = T1MS;      // 重置初始值 低8位
    TH0 = T1MS >> 8; // 重置初始值 高8位
	cnt++;
	if (cnt > 1000) {
		cnt = 0;
		time++;

		// 7270s = 2h 1m 10s

		hh = time / 3600;
		mm = (time % 3600) / 60;
		ss = time % 60;
		LCD_ShowNum(2,1,hh,2);
		LCD_ShowNum(2,4,mm,2);
		LCD_ShowNum(2,7,ss,2);
	}
}
	
void main()
{
	P2 = 0xFE;
	
	LCD_Init();
	LCD_ShowString(1,1,"Clock:");
	LCD_ShowString(2,1,"00:00:00");
	while (1)
	{		
		if(P3_1 == 0) {
			Com_Util_DelayMs(10); while (P3_1 == 0); Com_Util_DelayMs(10);
			Timer0_start();
			P2 = 0xFE;
		}
		if(P3_0 == 0) {
			Com_Util_DelayMs(10); while (P3_0 == 0); Com_Util_DelayMs(10);
			LCD_ShowString(2,1,"00:00:00");
			EA = 0;
		}
	}
	
}
