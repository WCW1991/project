#include "Task_Status_Indicator.h"

extern TIM_HandleTypeDef htim1;
typedef enum{
	NoneVoice,
	FireVoice,
	FaultVoice
}VoiceType_enum;
static OS_TMR *FireVoiceTimer;
static OS_TMR *FaultVoiceTimer;

static void SwitchVoiceOutput(void *ptmr, void *parg);
static void VoiceContral(VoiceType_enum VoiceType);

void Task_Status_Indicator(void *p_arg)
{
	uint8_t err;
	OS_FLAGS MyFlags;
	
	FireVoiceTimer = OSTmrCreate(0,2,OS_TMR_OPT_PERIODIC,SwitchVoiceOutput,(void *)NULL,"SwitchVoiceOutput",&err);
	FaultVoiceTimer = OSTmrCreate(0,4,OS_TMR_OPT_PERIODIC,SwitchVoiceOutput,(void *)NULL,"SwitchVoiceOutput",&err);
	for(;;){
		MyFlags=OSFlagQuery (FLAG_GRP, &err);
		if( OS_ERR_NONE == err ){
			if(!(MyFlags&SelfTestFlag)){
				if(MyFlags&ZhuDianFlag)BSP_LED_On( zhudian_LED1 );
				else BSP_LED_Off( zhudian_LED1 );
				
				if(MyFlags&YunXingFlag){
					BSP_LED_Toggle( yunxing_LED2 );
				}
				else BSP_LED_Off( yunxing_LED2 );
				
				if(MyFlags&BaoJingFlag){//±¨¾¯µÆ
					BSP_LED_On( baojing_LED3 );
					if(MyFlags&XiaoYinFlag)
						VoiceContral(NoneVoice);
					else
						VoiceContral(FireVoice);
					if(MyFlags&GuZhangFlag)//¹ÊÕÏµÆ
						BSP_LED_On( guzhang_LED4 );
					else
						BSP_LED_Off( guzhang_LED4 );
				}else{
					BSP_LED_Off( baojing_LED3 );
					if(MyFlags&GuZhangFlag){//¹ÊÕÏµÆ
						BSP_LED_On( guzhang_LED4 );
						if(MyFlags&XiaoYinFlag)
							VoiceContral(NoneVoice);
						else
							VoiceContral(FaultVoice);
					}else{
						BSP_LED_Off( guzhang_LED4 );
						VoiceContral(NoneVoice);
					}
				}
				
				if(MyFlags&BeiDianFlag)BSP_LED_On( beidian_LED5 );
				else BSP_LED_Off( beidian_LED5 );

				if(MyFlags&TongXinFlag)BSP_LED_On( OutPut_LED7 );
				else BSP_LED_Off( OutPut_LED7 );
				
				if(MyFlags&XiaoYinFlag)BSP_LED_On( xiaoyin_LED6 );
				else BSP_LED_Off( xiaoyin_LED6 );
				
				if(MyFlags&XiTongFultFlag)PBout(12)=1;
				else PBout(12)=0;
				
			}
		}
		//GetPrinterState( NoPaper );
		OSTimeDlyHMSM(0,0,0,500);
	}
}

static void SwitchVoiceOutput(void *ptmr, void *parg)
{
	if(*(VoiceType_enum *)parg==NoneVoice)
		Buzzer_OFF();
	else{
		if(TIM1->CCER&0x00000400){
			TIM1->CCER &= ~0x00000400;
			PBout(15)=0;
			//Buzzer_OFF();
		}else
			TIM1->CCER |= 0x00000400;
			//Buzzer_ON();
	}
}
static void VoiceContral(VoiceType_enum VoiceType)
{
	uint8_t err;
	static VoiceType_enum ToCallBack=NoneVoice;
	
	if(ToCallBack==VoiceType)
		return;
	else
		ToCallBack=VoiceType;
	
	if(ToCallBack==FireVoice){
		Buzzer_ON();
		OSTmrStart(FireVoiceTimer,&err);
		OSTmrStop(FaultVoiceTimer,OS_TMR_OPT_NONE,&ToCallBack,&err);
	}else
		if(ToCallBack==FaultVoice){
			Buzzer_ON();
			OSTmrStart(FaultVoiceTimer,&err);
			OSTmrStop(FireVoiceTimer,OS_TMR_OPT_NONE,&ToCallBack,&err);
		}else{
			Buzzer_OFF();
			OSTmrStop(FireVoiceTimer,OS_TMR_OPT_NONE,&ToCallBack,&err);
			OSTmrStop(FaultVoiceTimer,OS_TMR_OPT_NONE,&ToCallBack,&err);
		}
}













