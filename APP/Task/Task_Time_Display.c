#include "Task_Time_Display.h"

void Task_Time_Display( void )
{
	uint8_t   err;
	uint8_t ZhengDianZhi=0;
	
	for(;;){
		if(OSFlagAccept(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_WAIT_SET_ALL,&err)){
			/* ����Ƿ񵽴����� */
			if(systime.Minute==0&&systime.Second==0){
				if(systime.Hour!=ZhengDianZhi){//Сʱ��ִͬ��һ��ͬ��
					ZhengDianZhi=systime.Hour;//�������λ��
					
					record_state_store();//�洢״̬���浽Flash
				}
			}
			
			DisplaySystime(systime, 6, ScreenBuffer);
		}
		DS1302_GetTime(&systime);
		OSTimeDlyHMSM(0,0,0,300);
	}
	
}
