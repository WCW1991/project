#include "Task_RefreshScreen.h"

void Task_RefreshScreen(void)
{
	for(;;){
        //OSSemSet (ScreenBufferKey, 0, &err);//����������������Ļ���
		BufferToLCD(ScreenBuffer);
		//OSFlagPost(FLAG_GRP,ScreenRefreshFlag,OS_FLAG_SET,&err);//��������������Ļ���
		OSTimeDlyHMSM(0,0,0,100);
	}
}