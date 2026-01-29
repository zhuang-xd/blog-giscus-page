#ifndef __INT_MATRIX_LED_H__
#define __INT_MATRIX_LED_H__

#include "../Com/Com_Util.h"

void Int_init_matrix_led();
void Int_matrix_led_scan_col(u8 col, u8 byte);
void Int_74hc595_send_byte(u8 byte);

#endif // __INT_MATRIX_LED_H__
