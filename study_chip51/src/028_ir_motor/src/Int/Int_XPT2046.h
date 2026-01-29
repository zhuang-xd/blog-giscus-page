#ifndef __INT_XPT2046_H__
#define __INT_XPT2046_H__

#include "../Com/Com_Util.h"

#define XPT_2045_AD1  0x9C // AIN0 = XP    1001 1100
#define XPT_2045_NTC1 0xCC // AIN1 = YP    1100 1100
#define XPT_2045_GR1  0xAC // AIN2 = VBAT  1010 1100

u8 Int_XPT2045_Read(u8 cmd);

#endif // __INT_XPT2046_H__