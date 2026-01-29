#include "Dri_Timer1.h"
#include "REGX52.H"

void Dri_Timer1_Init()
{
    TMOD &= 0x0F;
    TMOD |= 0x10;
    TL1 = 0;
    TH1 = 0;
    TF1 = 0;		//清除TF1标志
	TR1 = 0;		//定时器1不计时
}

void Dri_Timer1_SetCounter(unsigned int value)
{
	TH1=value/256;
	TL1=value%256;
}

u16 Dri_Timer1_GetCounter()
{
    return (TH1<<8)|TL1;
}

void Dri_Timer1_Run(int flag)
{
    TR1 = flag;
}