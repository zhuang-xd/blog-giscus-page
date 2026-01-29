#ifndef __DRI_TIMER1_H__
#define __DRI_TIMER1_H__

#include "../Com/Com_Util.h"

void Dri_Timer1_Init();
void Dri_Timer1_SetCounter(unsigned int value);
u16 Dri_Timer1_GetCounter();
void Dri_Timer1_Run(int flag);


#endif // __DRI_TIMER1_H__