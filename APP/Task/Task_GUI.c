#include "Task_GUI.h"

#define SWITCH_FIRE_CYC	100	//SWITCH_FIRE_CYC/10s

static void Switch_Display_Fire(void *ptmr, void *parg);

static OS_TMR *Timer_Switch_Precedence_Fault;

void Task_GUI( void )
{
	uint8_t  err;
	uint8_t del_times=0;
	uint8_t test_mode = 0;
	uint8_t alarm_flag = 0;
	Key_enum *My_Key_Message;
	
	
	Timer_Switch_Precedence_Fault = OSTmrCreate(SWITCH_FIRE_CYC,
												SWITCH_FIRE_CYC,
												OS_TMR_OPT_PERIODIC,
												Switch_Display_Fire,
												(void *)NULL,
												"Switch",
												&err);
	OSTmrStart(Timer_Switch_Precedence_Fault,&err);
	
	for(;;){
		if(OSFlagPend(FLAG_GRP,StopGuiFlag,OS_FLAG_WAIT_CLR_ALL,0,&err))
		{
			if((alarm_flag == 0) && (Precedence_Fault_Total_Number != 0))
			{
				alarm_flag = 1;
				Switch_Display_Fire(NULL, NULL);
			}
			else if((alarm_flag == 1) && (Precedence_Fault_Total_Number == 0))
			{
				alarm_flag = 0;
			}
			
			Standy_Display();
			
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,200,&err);//等待按键
			if( err==OS_ERR_NONE ){
				OSTmrStart(Timer_Switch_Precedence_Fault,&err);	//有按键则重新开始计时
				/* 待机操作 */
				switch(*My_Key_Message){
#if MAIN_BOARD==1
					case Key_2://上翻
#endif
#if MAIN_BOARD==2
					case Key_Up://上翻
#endif
						switch(DisplayType){
						case StandyFire:
							if(Precedence_Fault_Display_Index != 0) Precedence_Fault_Display_Index--;
							else Precedence_Fault_Display_Index = Precedence_Fault_Total_Number-1;
							break;
						case StandyFault:
							if(Fault_Display_Index != 0) Fault_Display_Index--;
							else Fault_Display_Index = Fault_Total_Number-1;
							break;
						}
						break;
#if MAIN_BOARD==1
						case Key_8://下翻
#endif
#if MAIN_BOARD==2
					case Key_Down://下翻
#endif
						switch(DisplayType){
							case StandyFire:
								Precedence_Fault_Display_Index++;
								if(Precedence_Fault_Display_Index >= Precedence_Fault_Total_Number) Precedence_Fault_Display_Index = 0;
								break;
							case StandyFault:
								Fault_Display_Index++;
								if(Fault_Display_Index >= Fault_Total_Number) Fault_Display_Index = 0;
								break;
						}
						break;
					case Key_x://切换优先故障和普通故障
						switch(DisplayType){
							case StandyFire:
								if(Fault_Total_Number) DisplayType=StandyFault; break;
							case StandyFault:
								if(Precedence_Fault_Total_Number) DisplayType=StandyFire; break;
						}
						break;
					case Key_Set:
						EnterMenu();//进入菜单
						Switch_Display_Fire(NULL, NULL);
						break;
#if MAIN_BOARD==2
					case Key_F1:
						CheckRecord(FireFault);break;//紧急记录
					case Key_F2:
						CheckRecord(OtherFault);break;//普通记录
					case Key_F3:
						F3();break;//统计
					case Key_F4://版本
						while(1){
						uint8_t s[6];
						Clean_Lcd(ScreenBuffer);
#if(PROGRAM_TYPE==1)
							StringDisplay( "ＣＡ７０００", 0, 0, 0, ScreenBuffer );
#else
							StringDisplay( "ＣＡ６５００", 0, 0, 0, ScreenBuffer );
#endif
						StringDisplay( "硬件版本：", 4, 0, 0, ScreenBuffer );
						sprintf((char*)s,"V%d.01",MAIN_BOARD);
						StringDisplay( s, 4, 60, 0, ScreenBuffer );
						
						StringDisplay( "软件版本：", 6, 0, 0, ScreenBuffer );
						sprintf((char*)s,"V%d.%02d",MainVersion,SubVersion);
						StringDisplay( s, 6, 60, 0, ScreenBuffer );
						
						sprintf((char*)s,"ID:%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
								((uint8_t *)0x1FFFF7E8)[0],((uint8_t *)0x1FFFF7E8)[1],
								((uint8_t *)0x1FFFF7E8)[2],((uint8_t *)0x1FFFF7E8)[3],
								((uint8_t *)0x1FFFF7E8)[4],((uint8_t *)0x1FFFF7E8)[5],
								((uint8_t *)0x1FFFF7E8)[6],((uint8_t *)0x1FFFF7E8)[7],
								((uint8_t *)0x1FFFF7E8)[8],((uint8_t *)0x1FFFF7E8)[9],
								((uint8_t *)0x1FFFF7E8)[10],((uint8_t *)0x1FFFF7E8)[11]);
						StringDisplay( s, 2, 0, 0, ScreenBuffer );
						
						
						My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
						if( err==OS_ERR_NONE ){
							if(*My_Key_Message==Key_Return)break;
						 }else break;
						}break;
#endif
					case Key_Delete:
						if(Precedence_Fault_Total_Number == 0 && Fault_Total_Number == 0)
						{
							del_times++;
							if((del_times > 4) && (test_mode == 0)){
								test_mode = 1;
								BSP_LED_All_On();
								BSP_LED_On(SysBreak_LED8);
								
								OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_SET,&err);//停止巡检
								
								send_cmd_common(Terminal1, 0x75, 0x00);

								OSTimeDlyHMSM(0,0,3,0);
								BSP_LED_All_Off();
								BSP_LED_Off(SysBreak_LED8);
							}
						}
						break;
					default: ;
				}
				if(*My_Key_Message != Key_Delete) del_times = 0;
			}
		}
	}
}
static void Switch_Display_Fire(void *ptmr, void *parg)
{
#if (PROGRAM_TYPE == 1)
	if(Precedence_Fault_Total_Number)
	{
		if(DisplayType != StandyFire) DisplayType = StandyFire;
		else
		{
			Precedence_Fault_Display_Index++;
		}
	}
	else if(Fault_Total_Number)
	{
		Fault_Display_Index++;
	}
#endif
#if (PROGRAM_TYPE == 2)
	if(Precedence_Fault_Total_Number) DisplayType = StandyFire;
	Precedence_Fault_Display_Index = 0;
#endif
}