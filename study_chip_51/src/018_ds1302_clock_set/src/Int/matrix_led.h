#ifndef _MATRIX_LED_H_
#define _MATRIX_LED_H_
#include "../Com/Com_Util.h"

void Int_init_matrix_led();
void Int_matrix_led_scan_col(u8 col, u8 byte);
void Int_74hc595_send_byte(u8 byte);

#endif // _MATRIX_LED_H_