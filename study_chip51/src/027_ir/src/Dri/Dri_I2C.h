#ifndef __DRI_I2C_H__
#define __DRI_I2C_H__

#include "../Com/Com_Util.h"

void Dri_I2cStart();
void Dri_I2cStop();
void Dri_I2cSendByte(u8 byte);
u8 Dri_I2cRecvByte();
void Dri_I2cSendAck(u8 ack_bit);
u8 Dri_I2cRecvAck();

#endif // __DRI_I2C_H__