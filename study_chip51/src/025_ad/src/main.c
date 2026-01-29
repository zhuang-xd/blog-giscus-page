#include <REGX52.H>
#include "Com/Com_Util.h"
#include "Int/Int_LCD1602.h"
#include "Int/Int_XPT2046.h"

void main()
{
	u8 var1, var2, var3;

	Int_LCD_Init();

	Int_LCD_ShowString(0,0,"AD");
	Int_LCD_ShowString(0,4,"GR");
	Int_LCD_ShowString(0,8,"NTC");
	
	while (1)
	{
		var1 = Int_XPT2045_Read(XPT_2045_AD1);
		var2 = Int_XPT2045_Read(XPT_2045_GR1);
		var3 = Int_XPT2045_Read(XPT_2045_NTC1);
		Int_LCD_ShowNum(1,0,var1,3);
		Int_LCD_ShowNum(1,4,var2,3);
		Int_LCD_ShowNum(1,8,var3,3);
		Com_Util_DelayMs(1000);
	}
}