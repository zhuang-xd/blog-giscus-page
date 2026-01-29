
#include "Int_Beep.h"
#include "../Dri/Dri_Timer0.h"
#include "REGX52.H"

static void timer0_callback();

sbit BEEP = P2^5;
u8 freq_sel = 0;

static unsigned int code FreqTable[] = {0,
	63628,63731,63835,63928,64021,64103,64185,64260,64331,64400,64463,64528,
	64580,64633,64684,64732,64777,64820,64860,64898,64934,64968,65000,65030,
	65058,65085,65110,65134,65157,65178,65198,65217,65235,65252,65268,65283
};

void Int_init_beep()
{
    Dri_Timer0_Init();

    Dri_Timer0_RegisterCallback(timer0_callback);
}

void set_freq(u8 freq)
{
    freq_sel = freq;
}

static void timer0_callback() 
{
    if(FreqTable[freq_sel])
    {
        TL0 = FreqTable[freq_sel]%256;		//设置定时初值
        TH0 = FreqTable[freq_sel]/256;		//设置定时初值
        BEEP = !BEEP;		//设置蜂鸣器响的频率
    }
}