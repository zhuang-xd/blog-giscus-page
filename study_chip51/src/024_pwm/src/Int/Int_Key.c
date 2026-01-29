#include "Int_Key.h"
#include <REGX52.H>

sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;

u8 Int_Key_Pressed()
{
    if (KEY1 == 0) {
        Com_Util_DelayMs(10); while (KEY1 == 0); Com_Util_DelayMs(10);
        return 1;
    } else if (KEY2 == 0) {
        Com_Util_DelayMs(10); while (KEY2 == 0); Com_Util_DelayMs(10);
        return 2;
    } else if (KEY3 == 0) {
        Com_Util_DelayMs(10); while (KEY3 == 0); Com_Util_DelayMs(10);
        return 3;
    } else if (KEY4 == 0) {
        Com_Util_DelayMs(10); while (KEY4 == 0); Com_Util_DelayMs(10);
        return 4;
    } else {
        return 0;
    }
}