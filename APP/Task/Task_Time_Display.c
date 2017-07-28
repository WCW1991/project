#include "Task_Time_Display.h"

void Task_Time_Display( void )
{
	uint8_t   err;
	uint8_t ZhengDianZhi=0;
	
	for(;;){
		if(OSFlagAccept(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_WAIT_SET_ALL,&err)){
			/* 检查是否到达整点 */
			if(systime.Minute==0&&systime.Second==0){
				if(systime.Hour!=ZhengDianZhi){//小时不同执行一次同步
					ZhengDianZhi=systime.Hour;//当做标记位用
					
					record_state_store();//存储状态保存到Flash
				}
			}
			
			DisplaySystime(systime, 6, ScreenBuffer);
		}
		DS1302_GetTime(&systime);
		OSTimeDlyHMSM(0,0,0,300);
	}
	
}
