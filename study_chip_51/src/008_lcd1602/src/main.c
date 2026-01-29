#include "Int/LCD1602.h"

void main()
{
	LCD_Init();

	while (1)
	{
		LCD_ShowChar(1,1,'A');
		LCD_ShowString(2,1,"hello, lcd");
	}	
}