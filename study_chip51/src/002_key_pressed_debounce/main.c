#include <REGX52.H>

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

void LedOn ()
{
	P2 = 0xFE;
}

void LedOff ()
{
	P2 = 0xFF;
}

void LedToggle ()
{
	P2_0 = ~P2_0;
}

void main()
{
	P2 = 0xFE; // 1111 1110
		
	while(1)
	{
        if (P3_1 == 0) {
			DelayMs(20); // 按下消抖

			while (P3_1 == 0); // 松手消抖
			DelayMs(20); 

			LedToggle();
        } 
	}
}
