#include "Int/int_DigitalTube.h"

void main()
{
	int i;

	while (1)
	{
		for (i = 0; i < 8; i++)
		{
			flash_segment(i,i); // 第1个数码管, 显示2
		}		
	}
	
}