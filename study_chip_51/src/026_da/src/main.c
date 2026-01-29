#include <REGX52.H>
#include "Dri/Dri_Timer0.h"
#include "Com/Com_Util.h"

u8 compare = 30, count;
sbit LED = P2^1;

void Int_LM358_LedBlink();

void main()
{
	Dri_Timer0_Init();
	Dri_Timer0_RegisterCallback(Int_LM358_LedBlink);

	while (1)
	{
		compare = 70;
		Com_Util_DelayMs(200);
		compare = 0;
		Com_Util_DelayMs(200);
	}
}

void Int_LM358_LedBlink()
{
    // 100us
	TL0 = 0x9C;		//设置定时初值 
	TH0 = 0xFF;		//设置定时初值

    count++;
    count %= 100;
    if (count > compare) {
        LED = 1;
	}else {
        LED = 0;
    }
}