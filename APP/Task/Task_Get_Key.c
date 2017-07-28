#include "Task_Get_Key.h"

static uint8_t backlight_flag = 1;

static void turnoff_backlight(void *ptmr, void *parg);
static void one_hour(void *ptmr, void *parg);

void Task_Get_Key(void *p_arg)
{
	uint8_t	err;
	FlagStatus LongPushFlag=RESET;
	Key_enum SendKey=Key_None;
	Key_enum LastKey=Key_None;
	uint8_t PushTimes=0;
	OS_TMR *Timer_BackLight;
	OS_TMR *Timer_One_Hour;
	
	Timer_BackLight = OSTmrCreate(BACK_LIGHT_TIME*10,0,OS_TMR_OPT_ONE_SHOT,turnoff_backlight,(void *)NULL,"TurnOff_Backlight",&err);
	Timer_One_Hour = OSTmrCreate(ONE_HOUR_TIME*10,0,OS_TMR_OPT_ONE_SHOT,one_hour,(void *)NULL,"One_Hour",&err);
	OSTmrStart(Timer_BackLight,&err);
	Standy_State = 0x01;
	OSTmrStart(Timer_One_Hour,&err);
	Geg_Key();//去除第一次读74HC165不正常现象
	for(;;){
		SendKey = Geg_Key();
		if(Key_None != SendKey){
			if(SendKey==Key_XiaoYin){
				if((TIM1->CR1)&(TIM_CR1_CEN))//判断声音是否已开启
					OSFlagPost(FLAG_GRP,XiaoYinFlag,OS_FLAG_SET,&err);
			}
			
			OSTmrStart(Timer_BackLight,&err);//背光计时
			Standy_State = 0x01;
			OSTmrStart(Timer_One_Hour,&err);//一个小时无操作计时
			if(backlight_flag == 0)
			{//背光亮时才能操作
				TurnOn_Backlight();
				backlight_flag = 1;
				OSTimeDlyHMSM(0,0,0,500);
				continue;
			}
			
			if(SendKey==Key_Reset)
				OSMboxPostOpt(FuWei_Handle,&LastKey,OS_POST_OPT_BROADCAST);
			
			if(LastKey!=SendKey){//常按判断
				LastKey=SendKey;
				LongPushFlag=RESET;
				PushTimes=0;
			}else{
				PushTimes++;
				if(PushTimes>6)
					LongPushFlag=SET;
			}
			if((LastKey!=Key_XiaoYin)&&(LastKey!=Key_Reset)){
				OSMboxPostOpt(Key_Mbox_Handle,&LastKey,OS_POST_OPT_BROADCAST);
				if(LongPushFlag==RESET){
					OSTimeDlyHMSM(0,0,0,300);
				}else{
					OSTimeDlyHMSM(0,0,0,50);
				}
			}
		}else{
			LongPushFlag=RESET;
			PushTimes=0;
		}
		OSTimeDlyHMSM(0,0,0,100);
	}
}

static void turnoff_backlight(void *ptmr, void *parg)
{
	TurnOff_Backlight();
	backlight_flag = 0;
}

void one_hour(void *ptmr, void *parg)
{
	Standy_State = 0x02;
}
