#include <REGX52.H>
#include <intrins.h>

void DelayMs(unsigned char xms)
{
	unsigned char i,j;
	while(xms)
	{
		i = 2;
		j = 239;	
		do
		{
			while(--j);
		}while(--i);
		xms--;
	}
}


void main()
{
    P2 = 0xFF;  // 二进制 1111 1111 的共阳极表示
    
    while(1)
    {
        if (P3_1 == 0) {
            DelayMs(20); // 按键消抖
            
            if (P3_1 == 0) { // 确认按键按下
                while (P3_1 == 0); // 等待松手
                DelayMs(20); // 松手消抖
                
                P2 = ~P2; // 0000 0000
                P2++; 	  // 0000 0001
                P2 = ~P2; // 1111 1110
            }
        }
    }
}