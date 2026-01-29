#include "Int_AT24C02.h"
#include "../Dri/Dri_I2C.h"
#include "REGX52.h"

#define IC_ADDRESS  0xA0 // 1010 000
#define PAGE_SIZE   16


void Int_24C02_WriteByte(u8 addr, u8 byte)
{
    Dri_I2cStart();
    Dri_I2cSendByte(IC_ADDRESS);
    Dri_I2cRecvAck();
    Dri_I2cSendByte(addr);
    Dri_I2cRecvAck();
    Dri_I2cSendByte(byte);
    Dri_I2cRecvAck();
    Dri_I2cStop();
    Com_Util_DelayMs(10);
}

u8 Int_24C02_ReadByte(u8 addr)
{
    u8 byte;
    Dri_I2cStart();
    Dri_I2cSendByte(IC_ADDRESS);
    Dri_I2cRecvAck();
    Dri_I2cSendByte(addr);
    Dri_I2cRecvAck();
    Dri_I2cStart();
    Dri_I2cSendByte(IC_ADDRESS+1);
    Dri_I2cRecvAck();
    byte = Dri_I2cRecvByte();
    Dri_I2cSendAck(1);
    Dri_I2cStop();
    return byte;
}

/**
 * @brief 页写，至多缓存16页的数据
 * 
 * @param addr 地址
 * @param byte 数据
 * @param len 数据长度
 */
void Int_WritePage(u8 addr, u8 byte[], u8 len)
{
    u8 i;
    Dri_I2cStart();
    Dri_I2cSendByte(IC_ADDRESS);
    Dri_I2cRecvAck();
    Dri_I2cSendByte(addr);
    Dri_I2cRecvAck();
    for (i = 0; i < len; i++)
    {
        Dri_I2cSendByte(byte[i]);
        Dri_I2cRecvAck();
    }
    Dri_I2cStop();
    Com_Util_DelayMs(10);
}

void Int_ReadPage(u8 addr, u8 byte[], u8 len)
{
    u8 i;
    Dri_I2cStart();
    Dri_I2cSendByte(IC_ADDRESS);
    Dri_I2cRecvAck();
    Dri_I2cSendByte(addr);
    Dri_I2cRecvAck();
    Dri_I2cStart(); // 重新起始信号
    Dri_I2cSendByte(IC_ADDRESS+1); // 读取位
    Dri_I2cRecvAck();
    for (i = 0; i < len; i++)
    {
        byte[i] = Dri_I2cRecvByte();
        Dri_I2cSendAck(i == len - 1 ? 1 : 0);
    }
    Dri_I2cStop();
}

void Int_24C02_WriteBytes(u8 addr, u8 byte[], u8 len)
{
    u8 byte_remain = PAGE_SIZE - len % PAGE_SIZE;
    if (byte_remain > len) {
        // 数据数 < 最大页数
        Int_WritePage(addr,byte,len);
    } else {
        // 数据数 > 最大页数
        Int_WritePage(addr,byte,byte_remain);
        Int_24C02_WriteBytes(addr + byte_remain,byte + byte_remain, len - byte_remain); // 递归
    }
}


void Int_24C02_ReadBytes(u8 addr, u8 byte[], u8 len)
{
    u8 i;
    Dri_I2cStart();
    Dri_I2cSendByte(IC_ADDRESS);
    Dri_I2cRecvAck();
    Dri_I2cSendByte(addr);
    Dri_I2cRecvAck();
    Dri_I2cStart(); // 重新起始信号
    Dri_I2cSendByte(IC_ADDRESS+1); // 读取位
    Dri_I2cRecvAck();
    for (i = 0; i < len; i++)
    {
        byte[i] = Dri_I2cRecvByte();
        Dri_I2cSendAck(i == len - 1 ? 1 : 0);
    }
    Dri_I2cStop();
}