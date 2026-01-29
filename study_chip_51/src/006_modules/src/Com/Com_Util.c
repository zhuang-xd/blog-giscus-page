#include "Com_Util.h"

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

