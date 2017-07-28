#include "Task_RefreshScreen.h"

void Task_RefreshScreen(void)
{
	for(;;){
        //OSSemSet (ScreenBufferKey, 0, &err);//不允许其它任务更改缓冲
		BufferToLCD(ScreenBuffer);
		//OSFlagPost(FLAG_GRP,ScreenRefreshFlag,OS_FLAG_SET,&err);//允许其它任务更改缓冲
		OSTimeDlyHMSM(0,0,0,100);
	}
}