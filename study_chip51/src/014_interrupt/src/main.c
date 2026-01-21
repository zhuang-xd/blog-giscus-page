#include "Int/LCD1602.h"
#include <REGX52.H>
#include "Com/Com_Util.h"
#include "INTRINS.H"

#define INT0_PIN P3_2
#define INT1_PIN P3_3

void Init_int0()
{
    EA = 1; // 中断使能
    EX0 = 1;// IT0的中断使能

    IT0 = 1;// 下降沿触发
}

void Init_int1()
{
    EA = 1; // 中断使能
    EX1 = 1;// IT1的中断使能

    IT1 = 1;// 下降沿触发
}

void interrupt0_handle() interrupt 0
{
    Com_Util_DelayMs(20);
    if(INT0_PIN == 0) {
        P2_0 = ~P2_0;
    }
}

void interrupt1_handle() interrupt 2
{
    Com_Util_DelayMs(20);
    if(INT1_PIN == 0) {
        P2_0 = ~P2_0;
    }
}

void main()
{
    P2 = 0xFE;
    Init_int0();
    Init_int1();
	while (1)
	{
	}
}
