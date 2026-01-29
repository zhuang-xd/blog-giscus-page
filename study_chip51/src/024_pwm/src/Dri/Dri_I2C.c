#include "Dri_I2C.h"
#include <REGX52.H>

sbit I2C_SCL = P2^1;
sbit I2C_SDA = P2^0;

/*
    释放总线
    - SDA = 1
    - SCL = 0
*/


/*
    起始信号(两低)
*/ 
void Dri_I2cStart()
{
    // 在scl电平的时候准备好数据
    I2C_SCL = 1;
    I2C_SDA = 1; 
    I2C_SDA = 0;  // 下降沿
    I2C_SCL = 0; // 时钟下降沿准备接收数据
}

/*
    截止信号(两高)
*/ 
void Dri_I2cStop()
{
    I2C_SDA = 0; 
    I2C_SCL = 1;
    I2C_SDA = 1; // 上升沿
}

/*
    发送数据包
*/ 
void Dri_I2cSendByte(u8 byte)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        I2C_SDA = byte & (0x80 >> i); // 先写入高位，高电平前准备数据
        I2C_SCL = 1; // SDA在SCL高电平时有效
        I2C_SCL = 0;
    }
}


/*
    接收数据包
*/ 
u8 Dri_I2cRecvByte()
{
    u8 i, byte = 0;

    I2C_SDA = 1; // 释放总线

    for (i = 0; i < 8; i++)
    {
        I2C_SCL = 1; // SDA在SCL高电平时有效
        if (I2C_SDA) {
            byte |= (0x80 >> i); // 先读取高位
        }
        I2C_SCL = 0;
    }

    return byte;
}


/**
 * @brief i2c发送ack
 * 
 * @param ack_bit 
 */
void Dri_I2cSendAck(u8 ack_bit)
{
    I2C_SDA = ack_bit; // 0=ACK, 1=NACK
    I2C_SCL = 1; 
    I2C_SCL = 0;
}

/**
 * @brief I2c接收ack
 * 
 * @return u8 
 */
u8 Dri_I2cRecvAck()
{
    bit ack;
    I2C_SDA = 1; // 释放数据线
    I2C_SCL = 1;
    ack = I2C_SDA;
    I2C_SCL = 0;
    return ack;
}