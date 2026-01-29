
#include "Com/Com_Util.h"
#include "Int/Int_Beep.h"
#include "Int/Int_Segment.h"
#include <REGX52.H>

sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;

void main()
{
    int i;
    Int_init_segment();
    Int_init_beep();

    while (1) {

        if (KEY1 == 0) {
            Com_Util_DelayMs(1); while (KEY1 == 0); Com_Util_DelayMs(1);
            Int_show_num(7,1);
            Int_beep_set(1);
        }
        if (KEY2 == 0) {
            Com_Util_DelayMs(1); while (KEY2 == 0); Com_Util_DelayMs(1);
            Int_show_num(7,2);
            Int_beep_set(1);
        }
        if (KEY3 == 0) {
            Com_Util_DelayMs(1); while (KEY3 == 0); Com_Util_DelayMs(1);
            Int_show_num(7,3);
            Int_beep_set(1);
        }
        if (KEY4 == 0) {
            Com_Util_DelayMs(1); while (KEY4 == 0); Com_Util_DelayMs(1);
            Int_show_num(7,4);
            Int_beep_set(1);
        }
    }
}

