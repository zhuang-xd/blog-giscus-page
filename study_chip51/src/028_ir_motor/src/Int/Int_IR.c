#include "Int_IR.h"
#include <REGX52.H>
#include "../Dri/Dri_int0.h"
#include "../Dri/Dri_Timer1.h"
#include "../Dri/Dri_int0.h"
#include "../Int/Int_LCD1602.h"

// void Int0_handle();

enum ir_state {
    IR_STATE_IDLE = 0,
    IR_STATE_WAIT,    //等待Start信号或Repeat信号
    IR_STATE_DECODE
};

unsigned int IR_Time;		//计时时间
unsigned char IR_State;		//状态

unsigned char IR_Data[4];	//数据
unsigned char IR_pData;		//数据存到第几位，0~31

unsigned char IR_DataFlag;	//数据收到标志位
unsigned char IR_RepeatFlag;//数据重发标志位
unsigned char IR_Address;	//地址
unsigned char IR_Command;	//命令

void Int0_handle();

void Int_IR_Init(void)
{
	Dri_Timer1_Init();
	Dri_Int0_Init();
	Int_LCD_Init();
	Dri_Int0_RegisterCallback(Int0_handle);
}

unsigned char Int_IR_GetDataFlag(void)
{
	if(IR_DataFlag)
	{
		IR_DataFlag=0;
		return 1;
	}
	return 0;
}

unsigned char Int_IR_GetRepeatFlag(void)
{
	if(IR_RepeatFlag)
	{
		IR_RepeatFlag=0;
		return 1;
	}
	return 0;
}


unsigned char Int_IR_GetAddress(void)
{
	return IR_Address;
}


unsigned char Int_IR_GetCommand(void)
{
	return IR_Command;
}

int cnt;

void Int0_handle()
{
	if(IR_State == IR_STATE_IDLE)
	{
		// Int_LCD_ShowString(1,9,"0");
		// Int_LCD_ShowString(1,0,"000");
		Dri_Timer1_SetCounter(0);	//定时计数器清0
		Dri_Timer1_Run(1);			//定时器启动
		IR_State = IR_STATE_WAIT;
	}
	else if(IR_State== IR_STATE_WAIT) //
	{
		// Int_LCD_ShowString(1,9,"1");
		IR_Time=Dri_Timer1_GetCounter();	//获取上一次中断到此次中断的时间
		Dri_Timer1_SetCounter(0);	//定时计数器清0
		
		// Int_LCD_ShowNum(1,0,IR_Time,5);
		// 如果计时为13.5ms，则接收到了Start信号（判定值在12MHz晶振下为13500，在11.0592MHz晶振下为12442）
		if(IR_Time>12442-500 && IR_Time<12442+500)
		{
			P2 = 0x00;
			IR_State=IR_STATE_DECODE;
		}
		// 如果计时为11.25ms，则接收到了Repeat信号（判定值在12MHz晶振下为11250，在11.0592MHz晶振下为10368）
		else if(IR_Time>11250-500 && IR_Time<11250+500)
		{
			IR_RepeatFlag=1;	//置收到连发帧标志位为1
			Dri_Timer1_Run(0);		//定时器停止
			IR_State=IR_STATE_IDLE;
		}
		else					//接收出错
		{
			IR_State=IR_STATE_WAIT;			//置状态为1
		}
	}
	else if(IR_State==IR_STATE_DECODE)		//状态2，接收数据
	{
		// Int_LCD_ShowString(1,9,"1");
		// cnt++;
		// Int_LCD_ShowNum(1,13,cnt,3);
		IR_Time=Dri_Timer1_GetCounter();	//获取上一次中断到此次中断的时间
		Dri_Timer1_SetCounter(0);	//定时计数器清0
		//如果计时为1120us，则接收到了数据0（判定值在12MHz晶振下为1120，在11.0592MHz晶振下为1032）
		if(IR_Time>1032-50 && IR_Time<1032+50)
		{
			IR_Data[IR_pData/8]&=~(0x01<<(IR_pData%8));	//数据对应位清0
			IR_pData++;			//数据位置指针自增
		}
		//如果计时为2250us，则接收到了数据1（判定值在12MHz晶振下为2250，在11.0592MHz晶振下为2074）
		else if(IR_Time>2074-500 && IR_Time<2074+500)
		{
			IR_Data[IR_pData/8]|=(0x01<<(IR_pData%8));	//数据对应位置1
			IR_pData++;			//数据位置指针自增
		}
		else					//接收出错
		{
			IR_pData=0;			//数据位置指针清0
			// IR_State=IR_STATE_WAIT;			//置状态为1
		}
		if(IR_pData>=32)		//如果接收到了32位数据
		{
			IR_pData=0;			//数据位置指针清0
			if((IR_Data[0]==~IR_Data[1]) && (IR_Data[2]==~IR_Data[3]))	//数据验证
			{
				IR_Address=IR_Data[0];	//转存数据
				IR_Command=IR_Data[2];
				IR_DataFlag=1;	//置收到连发帧标志位为1
			}
			Dri_Timer1_Run(0);		//定时器停止
			IR_State=IR_STATE_IDLE;			//置状态为0
		}
	}
}
