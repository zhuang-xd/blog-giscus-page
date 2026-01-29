#ifndef __INT_LCD1602_H__
#define __INT_LCD1602_H__

#include "../Com/Com_Util.h"

void Int_LCD_Init();
void Int_LCD_Clear();
void Int_LCD_ShowChar(u8 line, u8 pos, u8 ch);
void Int_LCD_ShowString(u8 line, u8 pos, u8 str[]);
void Int_LCD_ShowNum(u8 line, u8 pos, int num, u8 digit);
void Int_LCD_ShowSignedNum(u8 line, u8 pos, int num, u8 digit);
void Int_LCD_ShowHexNum(u8 line, u8 pos, int num, u8 digit);
void Int_LCD_ShowBinNum(u8 line, u8 pos, int num, u8 digit);

#endif // __INT_LCD1602_H__