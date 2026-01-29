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