#ifndef __DRI_SPI_H__
#define __DRI_SPI_H__

#include "../Com/Com_Util.h"

void Dri_SPI_Start();
void Dri_SPI_End();
void Dri_SPI_SendByte(u8 byte);
u8 Dri_SPI_RecvByte();

#endif // __DRI_SPI_H__