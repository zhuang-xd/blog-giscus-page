#include "Dri_int0.h"
#include <REGX52.H>
#include "STDIO.H"
#define MAX_CALLBACK_COUNT 2
static Int0_Callback s_callbacks[MAX_CALLBACK_COUNT];

void Dri_Int0_Init()
{
	IT0=1;
	IE0=0;
	EX0=1;
	EA=1;
	PX0=1;
}

bit Dri_Int0_RegisterCallback(Int0_Callback callback)
{
    u8 i;
    // 确保不会重复注册
    for (i = 0; i < MAX_CALLBACK_COUNT; i++) {
        if (s_callbacks[i] == callback) {
            return 1;
        }
    }

    // 寻找坑位
    for (i = 0; i < MAX_CALLBACK_COUNT; i++) {
        if (s_callbacks[i] == NULL) {
            s_callbacks[i] = callback;
            return 1;
        }
    }

    return 0;
}

bit Dri_Int0_DeregisterCallback(Int0_Callback callback)
{
    u8 i;
    for (i = 0; i < MAX_CALLBACK_COUNT; i++) {
        if (s_callbacks[i] == callback) {
            s_callbacks[i] = NULL;
            return 1;
        }
    }

    return 0;
}

//下降沿触发执行
void Dri_Int0_Routine(void) interrupt 0
{
    u8 i;
    for (i = 0; i < MAX_CALLBACK_COUNT; i++) {
        if (s_callbacks[i] != NULL) {
            s_callbacks[i]();
        }
    }
}
