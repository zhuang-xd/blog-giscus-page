#include <REGX52.H>
#include "Com/Com_Util.h"
#include "STRING.H"
#include "Int/matrix_led.h"
#include "Int/Int_AT24C02.h"

//动画数据，code作用：将数组放到flash存储器中
unsigned char code bmp[]={
    0x00,0x7E,0x10,0x10,0x7E,0x00,0x00,0x7E,
    0x52,0x52,0x52,0x00,0x00,0x7E,0x02,0x02,
    0x00,0x00,0x7E,0x02,0x02,0x00,0x00,0x3C,
    0x42,0x42,0x42,0x42,0x3C,0x00,0x00,0x7A,
};

void main()
{
    u8 i,bmp_size, offset = 0;
    u8 buf[32];
    Int_init_matrix_led();
    bmp_size = sizeof(bmp)/sizeof(u8);

    Int_24C02_WriteBytes(0x01,bmp,bmp_size);
    Int_24C02_ReadBytes(0x01,buf,bmp_size);

    while (1) {
        for (i = 0; i < 8; i++)
        {
            Int_matrix_led_scan_col(i,buf[i+offset]); // 0011 0000 
        }
        Com_Util_DelayMs(50);
        offset++;
        if (offset > bmp_size - 8) {
            offset = 0;
        }
    }
}