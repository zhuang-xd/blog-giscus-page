#ifndef _COM_UTIL_H_
#define _COM_UTIL_H_

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

void Com_Util_Delay5Us(u8 xus);
void Com_Util_DelayMs(u8 xms);
void Com_int_to_str(int num, char *str);
void Com_sprintf_2d(char *buffer, const char *format, int value1, int value2);
void Com_get_row_col(u16 num, int *row, int *col);
int Com_my_sprintf(char* str, const char* format, ...);

#endif //_COM_UTIL_H_