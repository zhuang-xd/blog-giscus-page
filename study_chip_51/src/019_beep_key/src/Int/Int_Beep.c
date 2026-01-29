
#include "Int_Beep.h"
#include "../Dri/Dri_Timer0.h"
#include "REGX52.H"

static void timer0_callback();

sbit BEEP = P2^5;
sbit LED = P2^0;

u8 is_open = 0;

void Int_init_beep()
{
    Dri_Timer0_Init();

    Dri_Timer0_RegisterCallback(timer0_callback);
}


void Int_beep_set(u8 flag)
{
    is_open = flag;
}

static void timer0_callback()
{
    static u16 cnt = 0;
    cnt++;
    if (cnt >= 1000 && is_open) {
        BEEP = !BEEP;
        cnt = 0;
        is_open = 0;
    }
}