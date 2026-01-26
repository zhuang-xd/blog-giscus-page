#include <REGX52.H>
#include "Com/Com_Util.h"
#include "Int/Int_DS18B20.h"
#include "Int/LCD1602.h"

float temp;

void main()
{
	Int_DS18B20_Run();		//上电先转换一次温度，防止第一次读数据错误
	Com_Util_DelayMs(1000);	//等待转换完成
	LCD_Init();
	LCD_ShowString(1,1,"Temperature:");
	while(1)
	{
		Int_DS18B20_Run();	//转换温度
		temp=Int_DS18B20_ReadTemp();	//读取温度
		LCD_ShowChar(2,1,temp >= 0 ? '+' : '-');	//显示正号
		LCD_ShowNum(2,2,temp,3);		//显示温度整数部分
		LCD_ShowChar(2,5,'.');		//显示小数点
		LCD_ShowNum(2,6,(unsigned long)(temp*10000)%10000,4);//显示温度小数部分
	}
}

