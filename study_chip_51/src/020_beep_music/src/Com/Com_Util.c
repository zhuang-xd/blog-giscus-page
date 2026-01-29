#include "Com_Util.h"
#include <stdarg.h>  // 需要包含这个头文件

void Com_Util_DelayMs(unsigned char xms)
{
	unsigned char i,j;
	while(xms)
	{
		i = 2;
		j = 239;	
		do
		{
			while(--j);
		}while(--i);
		xms--;
	}
}

// 手动转换数字为字符串
void Com_int_to_str(int num, char *str) 
{
    int i = 0, j = 0;
    char temp[10];
    
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    // 处理负数
    if (num < 0) {
        str[j++] = '-';
        num = -num;
    }
    
    // 提取各位数字
    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    // 反转字符串
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

// 支持两个 %d 的sprintf
void Com_sprintf_2d(char *buffer, const char *format, int value1, int value2) 
{
    char num_str1[10], num_str2[10];
    int i = 0, j = 0, k = 0;
    int param_count = 0;
    
    // 转换第一个数字
    Com_int_to_str(value1, num_str1);
    // 转换第二个数字  
    Com_int_to_str(value2, num_str2);
    
    // 格式化输出
    i = 0;
    while (format[i] != '\0') {
        if (format[i] == '%' && format[i+1] == 'd') {
            // 插入数字
            if (param_count == 0) {
                j = 0;
                while (num_str1[j] != '\0') {
                    buffer[k++] = num_str1[j++];
                }
            } else {
                j = 0;
                while (num_str2[j] != '\0') {
                    buffer[k++] = num_str2[j++];
                }
            }
            param_count++;
            i += 2;  // 跳过%d
        } else {
            buffer[k++] = format[i++];
        }
    }
    buffer[k] = '\0';
}


void Com_get_row_col(u16 num, int *row, int *col) 
{
    int r, c;
    
    // 遍历所有可能的位置
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            // 假设按键编号从左到右、从上到下
            // 第0行: 位0-3, 第1行: 位4-7, 第2行: 位8-11, 第3行: 位12-15
            int bit_pos = r * 4 + c;
            
            if (num & (1 << bit_pos)) {
                *row = r;
                *col = c;
                return;  // 找到第一个按下的键
            }
        }
    }
    
    // 没有按键按下
    *row = -1;
    *col = -1;
}


// 简化版 sprintf 实现
int Com_my_sprintf(char* str, const char* format, ...)
{
    char* p = str;              // 输出字符串指针
    const char* fmt = format;   // 格式字符串指针
    va_list args;               // 可变参数列表
    int int_temp;               // 整数临时变量
    char char_temp;             // 字符临时变量
    char* str_temp;             // 字符串临时变量
    char num_buf[16];           // 数字转换缓冲区
    int i, len;
    
    va_start(args, format);     // 初始化可变参数
    
    while (*fmt)
    {
        if (*fmt != '%')
        {
            // 普通字符直接复制
            *p++ = *fmt++;
            continue;
        }
        
        fmt++;  // 跳过 '%'
        
        switch (*fmt)
        {
            case 'c':  // %c: 字符
                char_temp = (char)va_arg(args, int);
                *p++ = char_temp;
                fmt++;
                break;
                
            case 'd':  // %d: 十进制整数
                int_temp = va_arg(args, int);
                
                // 处理负数
                if (int_temp < 0)
                {
                    *p++ = '-';
                    int_temp = -int_temp;
                }
                
                // 转换数字为字符串（逆序）
                i = 0;
                do
                {
                    num_buf[i++] = (int_temp % 10) + '0';
                    int_temp /= 10;
                } while (int_temp > 0);
                
                // 逆序复制到输出
                while (i > 0)
                {
                    *p++ = num_buf[--i];
                }
                fmt++;
                break;
                
            case 'u':  // %u: 无符号十进制整数
                int_temp = va_arg(args, int);
                
                // 转换数字为字符串（逆序）
                i = 0;
                do
                {
                    num_buf[i++] = (int_temp % 10) + '0';
                    int_temp /= 10;
                } while (int_temp > 0);
                
                // 逆序复制到输出
                while (i > 0)
                {
                    *p++ = num_buf[--i];
                }
                fmt++;
                break;
                
            case 'x':  // %x: 十六进制整数（小写）
                int_temp = va_arg(args, int);
                
                // 转换数字为十六进制字符串（逆序）
                i = 0;
                do
                {
                    int digit = int_temp % 16;
                    if (digit < 10)
                        num_buf[i++] = digit + '0';
                    else
                        num_buf[i++] = digit - 10 + 'a';
                    int_temp /= 16;
                } while (int_temp > 0);
                
                // 添加 "0x" 前缀（可选）
                // *p++ = '0';
                // *p++ = 'x';
                
                // 逆序复制到输出
                while (i > 0)
                {
                    *p++ = num_buf[--i];
                }
                fmt++;
                break;
                
            case 'X':  // %X: 十六进制整数（大写）
                int_temp = va_arg(args, int);
                
                // 转换数字为十六进制字符串（逆序）
                i = 0;
                do
                {
                    int digit = int_temp % 16;
                    if (digit < 10)
                        num_buf[i++] = digit + '0';
                    else
                        num_buf[i++] = digit - 10 + 'A';
                    int_temp /= 16;
                } while (int_temp > 0);
                
                // 逆序复制到输出
                while (i > 0)
                {
                    *p++ = num_buf[--i];
                }
                fmt++;
                break;
                
            case 's':  // %s: 字符串
                str_temp = va_arg(args, char*);
                while (*str_temp)
                {
                    *p++ = *str_temp++;
                }
                fmt++;
                break;
                
            case '%':  // %%: 百分号
                *p++ = '%';
                fmt++;
                break;
                
            default:   // 不支持格式，原样输出
                *p++ = '%';
                *p++ = *fmt++;
                break;
        }
    }
    
    *p = '\0';  // 添加字符串结束符
    
    va_end(args);  // 清理可变参数
    
    return (p - str);  // 返回写入的字符数（不包括结束符）
}

// 辅助函数：整数转换为字符串
void int_to_str(int num, char* str)
{
    char temp[16];
    int i = 0, j = 0;
    
    // 处理负数
    if (num < 0)
    {
        str[j++] = '-';
        num = -num;
    }
    
    // 转换数字（逆序）
    do
    {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);
    
    // 逆序复制到输出
    while (i > 0)
    {
        str[j++] = temp[--i];
    }
    
    str[j] = '\0';
}