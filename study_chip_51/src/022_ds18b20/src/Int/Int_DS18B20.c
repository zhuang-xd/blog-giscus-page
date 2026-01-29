#include "Int_DS18B20.h"
#include "../Dri/Dri_1wire.h"

// ROM命令
#define DS18B20_ROM_SEARCH  0xf0
#define DS18B20_ROM_READ    0x33
#define DS18B20_ROM_MATCH   0x55
#define DS18B20_ROM_SKIP    0xcc
#define DS18B20_ROM_ALARM   0xec

// 功能指令
#define DS18B20_FUNC_CONVERT                0x44
#define DS18B20_FUNC_SCRATCHPAD_WRITE       0x4E
#define DS18B20_FUNC_SCRATCHPAD_READ        0xBE
#define DS18B20_FUNC_SCRATCHPAD_COPY        0x48
#define DS18B20_FUNC_SCRATCHPAD_RECALL_E2   0xB8
#define DS18B20_FUNC_READ_POWER_SUPPLY      0xB4

// 温度变换
void Int_DS18B20_Run()
{
    // 初始化
    Dri_1wire_Start();

    // 跳过ROM
    Dri_1wire_SendByte(DS18B20_ROM_SKIP);
    
    // 温度变换
    Dri_1wire_SendByte(DS18B20_FUNC_CONVERT);
}

// 读取温度
float Int_DS18B20_ReadTemp()
{
    int temp;
    u8 tlsb, tmsb;

    // 初始化
    Dri_1wire_Start();

    // 跳过ROM
    Dri_1wire_SendByte(DS18B20_ROM_SKIP);
    
    // 读取暂存器
    Dri_1wire_SendByte(DS18B20_FUNC_SCRATCHPAD_READ);

    // 开始接收温度数据
    tlsb = Dri_1wire_RecvByte();
    tmsb = Dri_1wire_RecvByte();

    temp = (tmsb << 8) | tlsb;

    return temp / 16.0;
}
