#ifndef __DRI_1WIRE_H__
#define __DRI_1WIRE_H__

#include "../Com/Com_Util.h"

u8 Dri_1wire_Start();

void Dri_1wire_SendBit(u8 bit1);
u8 Dri_1wire_RecvBit();

void Dri_1wire_SendByte(u8 byte);
u8 Dri_1wire_RecvByte();

#endif // __DRI_1WIRE_H__