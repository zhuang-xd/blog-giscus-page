#include "Dri_1wire.h"
#include "REGX52.H"
#include "../Com/Com_Util.h"

sbit DS18B20_SDA=P3^7;

u8 Dri_1wire_Start()
{
    u8 ack,i;
    // 单总线默认高电平
    DS18B20_SDA = 1;
    
    // 主机将总线拉低 480us
    DS18B20_SDA = 0;
    Com_Util_Delay5Us(100);    // Delay 500us
    
    // 释放总线
    DS18B20_SDA = 1;
    // 等待 15 ~ 60us
    Com_Util_Delay5Us(12);     //Delay 60us
    
    // 从机拉低总线 60 ~ 240 us响应主机
    ack = DS18B20_SDA;
    Com_Util_Delay5Us(100);    //Delay 500us
    
    return ack;
}

void Dri_1wire_SendBit(u8 bit1)
{
    u8 i;
    // 两位间隔时间 > 1ms，先发送高位
    DS18B20_SDA = 0;
    
    // 主机发送0：主机拉低总线60 ~ 120us，释放总线
    // 主机发送1：主机拉低总线1 ~ 15us，释放总线
    Com_Util_Delay5Us(2);     //Delay 13us
    
    // 从机拉低总线30us 读取电平
    DS18B20_SDA = bit1;
    Com_Util_Delay5Us(10);    //Delay 50us

    //释放总线
    DS18B20_SDA = 1;
}

u8 Dri_1wire_RecvBit()
{
    u8 bit1,i;
    
    // 两位间隔时间 > 1ms
    // 主机接收1位：主机拉低总线1~15us，释放总线
    DS18B20_SDA = 0;
    Com_Util_Delay5Us(1);     //Delay 5us
    DS18B20_SDA = 1;
    
    // 主机在拉低后的15us内读取电平
    Com_Util_Delay5Us(1);      //Delay 5us
    bit1 = DS18B20_SDA; 
    Com_Util_Delay5Us(100);     //Delay 500us

    return bit1;
}

void Dri_1wire_SendByte(u8 byte)
{
    u8 i = 0;
    for (i = 0; i < 8; i++)
    {
        Dri_1wire_SendBit(byte & (0x01 << i));    // 先发送低位，再发送高位
    }
}

u8 Dri_1wire_RecvByte()
{
    u8 i,byte = 0x00;
    for (i = 0; i < 8; i++)
    {
        if (Dri_1wire_RecvBit()){
            byte |= 0x01 << i;
        }
        // byte |= (Dri_1wire_RecvBit() & (0x01 << i)); // 先接收低位，再接收高位
    }
    return byte;
}