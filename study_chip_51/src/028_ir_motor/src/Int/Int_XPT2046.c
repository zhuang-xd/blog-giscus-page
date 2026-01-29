#include "Int_XPT2046.h"
#include "../Dri/Dri_SPI.h"

u8 Int_XPT2045_Read(u8 cmd)
{
    u8 byte = 0x00;
    Dri_SPI_Start();
    Dri_SPI_SendByte(cmd);
    byte = Dri_SPI_RecvByte();
    Dri_SPI_End();

    return byte;
}