#include "IntMatrixKey.h"
#include <REGX52.H>
#include "LCD1602.h"
#include "STRING.H"

void Int_matrix_key_scan(u16 num)
{
    u16 i,row,col;
    Com_get_row_col(num,&row,&col)
    P1 = 0xFF;                // 清空选中
    switch (row)
    {
        case 1 : P1_7 = 0;Int_matrix_key_scan_col(col);break; // 第1行
        case 2 : P1_6 = 0;Int_matrix_key_scan_col(col);break; // 第2行
        // case 3 : P1_5 = 0;Int_matrix_key_scan_col(col);break; // 第3行
        // case 4 : P1_4 = 0;Int_matrix_key_scan_col(col);break; // 第4行
    }
}

void Int_matrix_key_scan_col(u16 col)
{
    char buf[32];
    
    switch (col)
    {
        case 1 : P1_7 = 0;Int_matrix_key_scan_col(col);break; // 第1行
        case 2 : P1_6 = 0;Int_matrix_key_scan_col(col);break; // 第2行
        case 3 : P1_5 = 0;Int_matrix_key_scan_col(col);break; // 第3行
        case 4 : P1_4 = 0;Int_matrix_key_scan_col(col);break; // 第4行
    }

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

