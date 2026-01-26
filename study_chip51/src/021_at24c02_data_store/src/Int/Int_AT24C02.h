#ifndef __INT_AT24C02_H__
#define __INT_AT24C02_H__

#include "../Com/Com_Util.h"


void Int_24C02_WriteByte(u8 addr, u8 byte);
u8 Int_24C02_ReadByte(u8 addr);
void Int_WritePage(u8 addr, u8 str[], u8 str_len);
void Int_ReadPage(u8 addr, u8 str[], u8 str_len);
void Int_24C02_WriteBytes(u8 addr, u8 str[], u8 str_len);
void Int_24C02_ReadBytes(u8 addr, u8 str[], u8 str_len);

#endif // __INT_AT24C02_H__