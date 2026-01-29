#include "Int/LCD1602.h"
#include "Int/IntMatrixKey.h"
#include <REGX52.H>
#include "Com/Com_Util.h"

#define modules

/*
	- 4位数密码1~9
	- 按下13 ok
	- 按下14 取消
*/

void main()
{
#ifdef modules
	u8 i = 0;
	LCD_Init();
	LCD_ShowString(1,1"Password: ");

	u8 code = "1234";

	while (1)
	{
		for (i = 1; i <= 4; i++)
		{
			Int_matrix_key_scan_row(4);
		}
	}	
}