#include "IntMatrixKey.h"
#include <REGX52.H>
#include "LCD1602.h"
#include "STRING.H"

void Int_matrix_key_scan_row(u16 row)
{
    u16 i = 0;
    P1 = 0xFF;                // 清空选中
    switch (row)
    {
        case 1 : P1_7 = 0;Int_matrix_key_scan_col(row);break; // 第1行
        case 2 : P1_6 = 0;Int_matrix_key_scan_col(row);break; // 第2行
        case 3 : P1_5 = 0;Int_matrix_key_scan_col(row);break; // 第3行
        case 4 : P1_4 = 0;Int_matrix_key_scan_col(row);break; // 第4行
    }
}

void Int_matrix_key_scan_col(u16 row)
{
    char buf[32];
    
    if (P1_3 == 0) { 		// 第1列
        Com_Util_DelayMs(10);while (P1_3 == 0);Com_Util_DelayMs(10);
        Com_sprintf_2d(buf, "key pressed %d*%d",row, 1);
        LCD_ShowString(2,1, buf);
    } else if (P1_2 == 0) { // 第2列
        Com_Util_DelayMs(10);while (P1_2 == 0);Com_Util_DelayMs(10);
        Com_sprintf_2d(buf, "key pressed %d*%d",row, 2);
        LCD_ShowString(2,1, buf);
    }else if (P1_1 == 0) { 	// 第3列		
        Com_Util_DelayMs(10);while (P1_1 == 0);Com_Util_DelayMs(10);
        Com_sprintf_2d(buf, "key pressed %d*%d",row, 3);
        LCD_ShowString(2,1, buf);
    }else if (P1_0 == 0) {	// 第4列	
        Com_Util_DelayMs(10);while (P1_0 == 0);Com_Util_DelayMs(10);
        Com_sprintf_2d(buf, "key pressed %d*%d",row, 4);
        LCD_ShowString(2,1, buf);
    }
}

u8 Int_KeyMatrix_CheckSW()
{
    u8 i, j;
    u8 lines[4]   = {0xFE, 0xFD, 0xFB, 0xF7};
    u8 columns[4] = {0x10, 0x20, 0x40, 0x80};
    for (i = 0; i < 4; i++) {
        P1 = lines[i];
        for (j = 0; j < 4; j++) {
            if ((P1 & columns[j]) == 0x00) {
                Com_Util_DelayMs(10);
                if ((P1 & columns[j]) == 0x00) {
                    while ((P1 & columns[j]) == 0x00);
                    return j + 4 * i + 1;
                }
            }
        }
    }
    return 0;
}