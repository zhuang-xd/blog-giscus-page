#include "./Dri/Dri_UART.h"
#include "./Com/Com_Util.h"
#include "./Dri/Dri_Timer0.h"
#include <STRING.H>

#include <REGX52.H>

void main()
{
    char str[16];
    Dri_Timer0_Init();
    Dri_UART_Init();
    while (1) {
        if (Dri_UART_RecevieStr(str)) {
            if (strcmp(str, "on") == 0) {
                P2 = 0xFE;
                Dri_UART_SendStr("Ok: LED is on");
            } else if (strcmp(str, "off") == 0) {
                P2 = 0xFF;
                Dri_UART_SendStr("Ok: LED is off");
            } else {
                Dri_UART_SendStr("Error: Unknown command");
            }
        }
    }
}