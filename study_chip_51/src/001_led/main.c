#include <REG52.H>

void Delay1ms(unsigned char xms)
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
	P2 = 0xFE; // 1111 1110
		
	while(1)
	{
		if (P2 == 0xFF) {
			P2 = 0xFE;
		}
		
		Delay1ms(1000);
		
		P2 = (P2 << 1) | 0x01; // 1111 1111
	}
}
