#include "Int_Motor.h"
#include "../Dri/Dri_Timer0.h"
#include "REGX52.H"

void time0_handle();

sbit MOTOR = P1^0;
sbit LED = P2^0;
int Counter = 0,compare = 0;

void Int_Motor_Init()
{
	Dri_Timer0_Init();
	Dri_Timer0_RegisterCallback(time0_handle);
}

void Int_Motor_SetSpeed(u8 speed)
{
    compare = speed;
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