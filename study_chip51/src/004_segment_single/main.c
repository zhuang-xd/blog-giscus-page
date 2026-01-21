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
    // sbit leds[3] = {P2_3,P2_4,P2_5};
    unsigned char seg[17] = {
				// 共阴极，高电平有效
				//  7 6 5 4   3 2 1 0 
				// dp g f e   d c b a
		0x3F,	//  0 0 1 1   1 1 1 1  = 0
        0x06,	//  0 0 0 0   0 1 1 0  = 1
        0x5B, 	//  0 1 0 1   1 0 1 1  = 2
		0x4F, 	// "3" a b c d g
		0x66, 	// "4" b c f g
		0x6D, 	// "5" a c d f g
		0x7D, 	// "6" a c d e f g
		0x07, 	// "7" a b c
		0x7F, 	// "8" 全亮
		0x6F, 	// "9" a b c d f g
		0x77, 	// "A"
		0x7C, 	// "B"
		0x39, 	// "C"
		0x5E, 	// "D"
		0x79, 	// "E"
		0x71, 	// "F"
    };

	// 第4个数码管
	P2_4= 1;
	P2_3= 0;  
	P2_2= 0;
	
	while (1)
	{
		static unsigned char i = 0;
		P0 = seg[i];
		i++;
		DelayMs(1000);
		if (i > 16) {
			i = 0;
		}
	}
	
}