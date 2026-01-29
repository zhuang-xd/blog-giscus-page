#include <REGX52.H>
#include "Com/Com_Util.h"
#include "Int/Int_Key.h"
#include "Int/Int_Segment.h"
#include "Int/Int_Motor.h"
#include "Int/Int_IR.h"
#include "Int/Int_LCD1602.h"

void main()
{
	u8 key,speed = 0;
	u8 cmd;
	Int_Seg_Init();
	Int_Motor_Init();
	Int_IR_Init();
	Int_LCD_Init();
	
	while (1)
	{
		key = Int_Key_Pressed(); // 检测按键按下
		if (Int_IR_GetDataFlag()) cmd = Int_IR_GetCommand();// 检测收到红外
		
		if (key || cmd) {
			speed++;
			speed %= 4;
			if (speed == 0){Int_Motor_SetSpeed(0);}
			if (speed == 1){Int_Motor_SetSpeed(35);}
			if (speed == 2){Int_Motor_SetSpeed(70);}
			if (speed == 3){Int_Motor_SetSpeed(100);}
			cmd = 0;
		}
		Int_Seg_ShowNum(0,speed);
	}
}

