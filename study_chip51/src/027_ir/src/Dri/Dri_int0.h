#ifndef __DRI_INT0_H__
#define __DRI_INT0_H__
#include "../Com/Com_Util.h"
typedef void (*Int0_Callback)(void);

void Dri_Int0_Init();
bit Dri_Int0_RegisterCallback(Int0_Callback callback);
bit Dri_Int0_DeregisterCallback(Int0_Callback callback);


#endif // __DRI_INT0_H__