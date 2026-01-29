#include "Int/LCD1602.h"
#include "Int/IntMatrixKey.h"
#include <REGX52.H>
#include "Com/Com_Util.h"

#define modules

void main()
{
#ifdef modules
	u8 i = 0;
	LCD_Init();
	while (1)
	{
		for (i = 1; i <= 4; i++)
		{
			Int_matrix_key_scan_row(i);
		}
#else
		P1_7 = 0; // 第1行
		if (P1_3 == 0) { 		// 第1列
			Com_Util_DelayMs(10);while (P1_3 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 01");
		} else if (P1_2 == 0) { // 第2列
			Com_Util_DelayMs(10);while (P1_2 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 02");
		}else if (P1_1 == 0) { 	// 第3列		
			Com_Util_DelayMs(10);while (P1_1 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 03");
		}else if (P1_0 == 0) {	// 第4列	
			Com_Util_DelayMs(10);while (P1_0 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 04");
		}

		P1 = 0xFF;
		P1_6 = 0; // 第2行
		if (P1_3 == 0) { 		// 第1列
			Com_Util_DelayMs(10);while (P1_3 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 05");
		} else if (P1_2 == 0) { // 第2列
			Com_Util_DelayMs(10);while (P1_2 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 06");
		}else if (P1_1 == 0) { 	// 第3列		
			Com_Util_DelayMs(10);while (P1_1 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 07");
		}else if (P1_0 == 0) {	// 第4列	
			Com_Util_DelayMs(10);while (P1_0 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 08");
		}

		P1 = 0xFF;
		P1_5 = 0; // 第3行
		if (P1_3 == 0) { 		// 第1列
			Com_Util_DelayMs(10);while (P1_3 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 09");
		} else if (P1_2 == 0) { // 第2列
			Com_Util_DelayMs(10);while (P1_2 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 10");
		}else if (P1_1 == 0) { 	// 第3列		
			Com_Util_DelayMs(10);while (P1_1 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 11");
		}else if (P1_0 == 0) {	// 第4列	
			Com_Util_DelayMs(10);while (P1_0 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 12");
		}
		
		P1 = 0xFF;
		P1_4 = 0; // 第4行
		if (P1_3 == 0) { 		// 第1列
			Com_Util_DelayMs(10);while (P1_3 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 13");
		} else if (P1_2 == 0) { // 第2列
			Com_Util_DelayMs(10);while (P1_2 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 14");
		}else if (P1_1 == 0) { 	// 第3列		
			Com_Util_DelayMs(10);while (P1_1 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 15");
		}else if (P1_0 == 0) {	// 第4列	
			Com_Util_DelayMs(10);while (P1_0 == 0);Com_Util_DelayMs(10);
			LCD_ShowString(2,1, "key pressed 16");
		}
#endif
	}	
}