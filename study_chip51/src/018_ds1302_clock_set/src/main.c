#include <REGX52.H>
#include "Int/Int_DS1302.h"
#include "Int/LCD1602.h"
#include "Int/IntMatrixKey.h"

// 修改后的主函数
// 定义独立按键引脚
sbit KEY_MOD = P3^1;    // 模式
sbit KEY_OK  = P3^0;    // 设置/确认键
sbit KEY_ADD = P3^2;    // 增加键
sbit KEY_SUB = P3^3;    // 减少键

// 定义闪烁控制变量
bit blink_flag = 0;     // 闪烁标志位
u16 blink_timer = 0;    // 闪烁计时器
u8 edit_mode = 0;       // 0:显示模式，1编辑模式(对应各个时间单位)

// 声明函数
void System_Init(void);
void Display_Time(Struct_Date *date);
void Handle_Keys(void);
void Adjust_Time(u8 mode, u8 add);
void Blink_Control(void);
void Show_Blink(u8 line, u8 col, u8 num);

// 主函数
void main()
{
    Struct_Date st_date = {0,};
    Struct_Date st_date_display = {0,};
    
    // 系统初始化
    System_Init();
    
    // 设置初始时间
    st_date.year        = 26;    // 2026年
    st_date.month       = 1;     // 1月
    st_date.day         = 20;    // 20日
    st_date.hour        = 17;    // 17时
    st_date.minute      = 50;    // 50分
    st_date.second      = 0;     // 0秒
    st_date.day_of_week = 2;     // 星期二
    
    // 写入初始时间到DS1302
    Int_DS1302_WriteTime(&st_date);
    
    // 显示静态字符
    LCD_ShowString(1, 1, "  -  -  ");
    LCD_ShowString(2, 1, "  :  :  ");
    
    while(1)
    {
        // 读取当前时间
        Int_DS1302_ReadTime(&st_date_display);
        
        // 处理按键
        Handle_Keys();
        
        // 闪烁控制
        Blink_Control();
        
        // 显示时间（考虑闪烁效果）
        Display_Time(&st_date_display);

        LCD_ShowNum(1,10,edit_mode,1);
        LCD_ShowNum(2,10,blink_flag,1);
        
        // 短暂延时
        Com_Util_DelayMs(50);
    }
}

// 系统初始化
void System_Init(void)
{
    LCD_Init();                 // LCD初始化
    Int_DS1302_init();          // DS1302初始化
    // Int_KeyMatrix_Init();       // 矩阵键盘初始化（如果函数存在）
    
    // 设置独立按键引脚为上拉输入
    KEY_MOD = 1;
    KEY_OK = 1;
    KEY_ADD = 1;
    KEY_SUB = 1;
}

// 显示时间
void Display_Time(Struct_Date *date)
{
    if(!date) return;
    
    // 显示日期部分（根据编辑模式决定是否闪烁）
    if(edit_mode != 1 || blink_flag)
        LCD_ShowNum(1, 1, date->year, 2);
    else
        LCD_ShowString(1, 1, "  ");
    
    if(edit_mode != 2 || blink_flag)
        LCD_ShowNum(1, 4, date->month, 2);
    else
        LCD_ShowString(1, 4, "  ");
    
    if(edit_mode != 3 || blink_flag)
        LCD_ShowNum(1, 7, date->day, 2);
    else
        LCD_ShowString(1, 7, "  ");
    
    // 显示时间部分
    if(edit_mode != 4 || blink_flag)
        LCD_ShowNum(2, 1, date->hour, 2);
    else
        LCD_ShowString(2, 1, "  ");
    
    if(edit_mode != 5 || blink_flag)
        LCD_ShowNum(2, 4, date->minute, 2);
    else
        LCD_ShowString(2, 4, "  ");
    
    if(edit_mode != 6 || blink_flag)
        LCD_ShowNum(2, 7, date->second, 2);
    else
        LCD_ShowString(2, 7, "  ");
}

// 处理按键
void Handle_Keys(void)
{
    if (KEY_MOD == 0) 
    {
        Com_Util_DelayMs(10); while(KEY_MOD == 0);Com_Util_DelayMs(10);
        edit_mode++;
        if (edit_mode > 6) edit_mode = 1;
        return;
    }

    // 确认键
    if(KEY_OK == 0)
    {
        Com_Util_DelayMs(10); while(KEY_OK == 0); Com_Util_DelayMs(10);
        if (edit_mode > 0) {
            edit_mode = 0;
        }
        return;
    }

    // 如果在编辑模式下，处理加减键
    if(edit_mode > 0)
    {
        if(KEY_ADD == 0)
        {
            Com_Util_DelayMs(10);while(KEY_ADD == 0);Com_Util_DelayMs(10);
            Adjust_Time(edit_mode, 1);  // 增加
        }
        else if(KEY_SUB == 0)
        {
            Com_Util_DelayMs(10);while(KEY_SUB == 0);Com_Util_DelayMs(10);
            Adjust_Time(edit_mode, 0);  // 减少
        }
    }
}

// 调整时间
void Adjust_Time(u8 mode, u8 add)
{
    Struct_Date current_time;
    
    // 读取当前时间
    Int_DS1302_ReadTime(&current_time);
    
    // 根据模式调整对应的时间单位
    switch(mode)
    {
        case 1:  // 年
            if(add) current_time.year = (current_time.year + 1) % 100;
            else current_time.year = (current_time.year + 99) % 100;
            break;
            
        case 2:  // 月
            if(add) 
                current_time.month = (current_time.month % 12) + 1;
            else
                current_time.month = (current_time.month + 10) % 12 + 1;
            break;
            
        case 3:  // 日
            {
                u8 days_in_month;
                // 计算当月天数（简化版，不考虑闰年）
                switch(current_time.month)
                {
                    case 2: days_in_month = 28; break;
                    case 4: case 6: case 9: case 11: days_in_month = 30; break;
                    default: days_in_month = 31; break;
                }
                
                if(add)
                    current_time.day = (current_time.day % days_in_month) + 1;
                else
                    current_time.day = (current_time.day + days_in_month - 2) % days_in_month + 1;
            }
            break;
            
        case 4:  // 时
            if(add) current_time.hour = (current_time.hour + 1) % 24;
            else current_time.hour = (current_time.hour + 23) % 24;
            break;
            
        case 5:  // 分
            if(add) current_time.minute = (current_time.minute + 1) % 60;
            else current_time.minute = (current_time.minute + 59) % 60;
            break;
            
        case 6:  // 秒
            if(add) current_time.second = (current_time.second + 1) % 60;
            else current_time.second = (current_time.second + 59) % 60;
            break;
    }
    
    // 写入调整后的时间
    Int_DS1302_WriteTime(&current_time);
}

// 闪烁控制
void Blink_Control(void)
{
    blink_timer++;
    if(blink_timer >= 5)  // 每500ms切换一次（50ms * 10）
    {
        blink_timer = 0;
        blink_flag = ~blink_flag;
    }
}