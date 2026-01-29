#include <REGX52.H>
#include "Com/Com_Util.h"
#include "Int/Int_Key.h"
#include "Int/Int_Segment.h"
#include "Dri/Dri_Timer0.h"

void time0_handle();

sbit MOTOR = P1^0;
sbit LED = P2^0;
int Counter = 0,compare = 0;

void main()
{
	u8 key,speed = 0;
	Int_Seg_Init();
	Dri_Timer0_Init();
	Dri_Timer0_RegisterCallback(time0_handle);
	
	while (1)
	{
		key = Int_Key_Pressed();
		if (key == 1) {
			speed++;
			speed %= 4;
			if (speed == 0){compare = 0;}
			if (speed == 1){compare = 35;}
			if (speed == 2){compare = 70;}
			if (speed == 3){compare = 101;}
		}
		Int_Seg_ShowNum(7,speed);
	}
}

void time0_handle()
{
	// 100us
	TL0 = 0x9C;		//设置定时初值 
	TH0 = 0xFF;		//设置定时初值
	Counter++;
	Counter%=100;	//计数值变化范围限制在0~99
	if(Counter<compare)	//计数值小于比较值
	{
		LED=0;		//输出0
		MOTOR=1;		//输出0
	}
	else				//计数值大于比较值
	{
		MOTOR=0;		//输出0
		LED=1;		//输出1
	}
}