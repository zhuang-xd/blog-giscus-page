#include "Dri_SPI.h"
#include "../Com/Com_Util.h"
#include "REGX52.H"

sbit SPI_CS    = P3^5;
sbit SPI_DCLK  = P3^6;
sbit SPI_DIN   = P3^4;
sbit SPI_DOUT  = P3^7;

void Dri_SPI_Start()
{
    SPI_CS = 0;
    SPI_DCLK = 0;
}

// SPI结束传输
void Dri_SPI_End()
{
    SPI_CS = 1;         // 释放设备
    SPI_DCLK = 0;       // 时钟保持低电平
}

void Dri_SPI_SendByte(u8 byte)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        SPI_DCLK = 0;   // 上升沿发送
        SPI_DIN = byte & (0x80 >> i); // 由高到低发送
        SPI_DCLK = 1;
    }    
    SPI_DCLK = 0;
}

u8 Dri_SPI_RecvByte()
{
    u8 i,res = 0x00; 
    for (i = 0; i < 8; i++)
    {
        SPI_DCLK = 1;   // 下降沿读取
        if (SPI_DOUT) {
            res |= (0x80 >> i); // 由高到低读取
        }
        SPI_DCLK = 0;
    }

    return res;
}
