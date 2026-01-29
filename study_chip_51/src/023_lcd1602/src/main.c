#include <REGX52.H>
#include "Com/Com_Util.h"
#include "Int/Int_LCD1602.h"

void main()
{
	Int_LCD_Init();
	Int_LCD_ShowString(0,0,"hello");
	Int_LCD_ShowChar(0,6,'A');
	Int_LCD_ShowNum(0,8,118,3);
	Int_LCD_ShowSignedNum(0,12,-118,3);
	Int_LCD_ShowHexNum(1,0,'a',2); // 97 = 0x61
	Int_LCD_ShowBinNum(1,5,'a',8); // 97 = 0110 0001

	while (1)
	{
		/* code */
	}
	
}

