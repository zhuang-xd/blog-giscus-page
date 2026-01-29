#include "Int_Segment.h"
#include "REGX52.H"

#define SEG P0
static void Int_chip_sel(u8 chip_sel);

// 共阴极数码管的编码表
u8 segment_table[17] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 
    0x6d, 0x7d, 0x07, 0x7f, 0x6f,
    0x77, 0x7c, 0x39, 0x5e, 0x79, 
    0x71,
};

void Int_Seg_Init()
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        Int_chip_sel(i);
        SEG = 0x00;
    }
    
}

void Int_Seg_ShowNum(u8 chip_sel, u8 seg_sel)
{
    /*
        6  =  0110
    */
    Int_chip_sel(chip_sel);

    SEG = segment_table[seg_sel];
}

static void Int_chip_sel(u8 chip_sel)
{
   switch (chip_sel)
   {
    case 0: P2_2 = 0; P2_3 = 0; P2_4 = 0; break;
    case 1: P2_2 = 0; P2_3 = 0; P2_4 = 1; break;
    case 2: P2_2 = 0; P2_3 = 1; P2_4 = 0; break;
    case 3: P2_2 = 0; P2_3 = 1; P2_4 = 1; break;
    case 4: P2_2 = 1; P2_3 = 0; P2_4 = 0; break;
    case 5: P2_2 = 1; P2_3 = 0; P2_4 = 1; break;
    case 6: P2_2 = 1; P2_3 = 1; P2_4 = 0; break;
    case 7: P2_2 = 1; P2_3 = 1; P2_4 = 1; break;   
   }
}