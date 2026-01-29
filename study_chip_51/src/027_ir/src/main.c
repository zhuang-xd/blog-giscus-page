#include <REGX52.H>
#include "Com/Com_Util.h"
#include "Int/Int_LCD1602.h"
#include "Dri/Dri_int0.h"
#include "Int/Int_IR.h"
#include "Dri/Dri_Timer1.h"
#include "INTRINS.H"

unsigned char Num;
unsigned char Address;
unsigned char Command;

u16 time;

void main()
{
	Dri_Int0_Init();
	Int_LCD_Init();
	Int_IR_Init();	
	Int_LCD_ShowString(0,0,"ADDR  CMD  NUM");
	Int_LCD_ShowString(1,0,"000   000   000");

	while(1)
	{
		if(Int_IR_GetDataFlag() || Int_IR_GetRepeatFlag())	//如果收到数据帧或者收到连发帧
		{
			Address=Int_IR_GetAddress();		//获取遥控器地址码
			Command=Int_IR_GetCommand();		//获取遥控器命令码
			
			Int_LCD_ShowHexNum(1,0,Address,2);	//显示遥控器地址码
			Int_LCD_ShowHexNum(1,6,Command,2);	//显示遥控器命令码
			
			if(Command==IR_VOL_MINUS)		//如果遥控器VOL-按键按下
			{
				Num--;						//Num自减
			}
			if(Command==IR_VOL_ADD)			//如果遥控器VOL+按键按下
			{
				Num++;						//Num自增
			}
			
			Int_LCD_ShowNum(1,11,Num,3);		//显示Num
		}
	}
}
