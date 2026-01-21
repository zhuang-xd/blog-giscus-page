#include <REGX52.H>
#include "./Com/Com_Util.h"
#include "./Dri/Dri_Timer0.h"
#include "./Int/Int_Beep.h"

#define SPEED 500

// 一剪梅
u8 code Music1[]={
    // 雪花飘飘
    L6,2, M6,2, M5,2, M6,2, M3,1,M5,1,M3,1, M5,1, M6,4,
    // 被封潇潇
    M5,2, M2,2, M2,2, M3,1, M5,1, M3,1, M3,1, M3,2 + 1,
    // 天地一片苍茫
    M1,1, L6,1, L5,1,  L6,1, L6,1, M1,1, L6,1,  L5,1, L6,1, 
    L6,2, M5,1, M6,1, M3,2, M2,2, M3,1, M3,1, M3,1+1,
	0xFF
};	


// 水星记
u8 code Music2[] = {
    // 还要多远才能进入你的心？
    M0, 4, M0, 4,       // 0 0
    M1, 4, M2, 4, M3, 4, M5, 4, // 1 2 3 5
    M6, 4, M5, 4, M4, 4, M6, 4, M5, 4, M3, 4, M6, 4, // 6 5 4 6 5 3 6
    
    // 还要多久才能和你接近？
    M6, 8, M6, 4, M5, 4, H1, 4, M3, 4, // 6 - 6 5 i 3
    M3, 4, M2, 4, M5, 4, M3, 4, M2, 4, M5, 4, M3, 4, // 3 2 5 3 2 5 3
    
    M0,4,M0,4,M0,4,M0,4,
    0xff // 结束标记
};

unsigned char FreqSelect, MusicSelect;

void main()
{
    int i = 0;
    int time_unit, actual_delay;
    Int_init_beep();
	while(1)
	{
		if(Music[MusicSelect] != 0xFF)
		{
            // 设置音调
            set_freq(Music[MusicSelect]);
            
            // 设置这个音频的持续时间
			MusicSelect++;
            time_unit = Music[MusicSelect];  // // 第一步：获取时值参数 例如值为 4、8、2 等
            actual_delay = SPEED/4 * time_unit;  // 第二步：计算实际播放时间
			Com_Util_DelayMs(actual_delay);	//时值，此时中断

            // 停止
			MusicSelect++;
			TR0 = 0;

            // 下一个音调
			Com_Util_DelayMs(5);
			TR0 = 1;	//模拟抬手
		}else {
            MusicSelect = 0;
        }
	}
}

