#include "EnterMenu.h"

extern TIM_HandleTypeDef htim1;

static uint8_t HuiLuBianChen(void);
static uint8_t MachineSet( void );
void EnterMenu(void)
{
	uint8_t  err;
	OS_FLAGS MyFlags;
	//Send_Receive_struct Send,Receive;
	Key_enum *My_Key_Message;
	enum 
	{
		Main_Menu = 1,
		Self_Test,
		Operation,
		Record,
		Set_Menu,
		Code_Menu,
		Exit
	}GUI=Main_Menu;
	for(;;){
		if(GUI==Exit)break;
		switch(GUI){
		case Main_Menu:{
			Clean_Lcd(ScreenBuffer);
			OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_SET,&err);
			StringDisplay( "１系统自检", 0, 0,0, ScreenBuffer );
			StringDisplay( "２操作应用", 2, 0,0, ScreenBuffer );
			StringDisplay( "３事件记录", 4, 0,0, ScreenBuffer );
			StringDisplay( "４设　　置", 0, 80,0, ScreenBuffer );
			StringDisplay( "５登　　记", 2, 80,0, ScreenBuffer );
			//StringDisplay( "安吉斯智能设备", 6, 50,0, ScreenBuffer);
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
			if( err==OS_ERR_NONE ){
				switch(*My_Key_Message){
				case Key_1:{if( SUCCESS!=Password_Check())continue;GUI=Self_Test;}break;
				case Key_2:{if( SUCCESS!=Password_Check())continue;GUI=Operation;}break;
				case Key_3:{GUI=Record;}break;
				case Key_4:{if( SUCCESS!=Password_Check())continue;GUI=Set_Menu;}break;
				case Key_5:{if( SUCCESS!=Password_Check())continue;GUI=Code_Menu;}break;
				case Key_Return:{GUI=Exit;}break;
				default:break;
				}
			}else{
				return;
			}
		}break;
		case Self_Test :{
			MyFlags=OSFlagQuery (FLAG_GRP, &err);
			OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_SET,&err);
			OSFlagPend(FLAG_GRP,PollTaskEndFlag,OS_FLAG_WAIT_SET_ALL,0,&err);//等待巡检完成
			OSTimeDlyHMSM(0,0,0,50);
			OSFlagPost(FLAG_GRP,XiaoYinFlag,OS_FLAG_SET,&err);
			
			OSTaskSuspend(Task_Time_Display_PRIO);
			OSTaskSuspend(APP_CFG_TASK_START_PRIO);
			send_cmd_common(Terminal1, 0x72, 0x00);
			OSTaskSuspend(Task_Status_Indicator_PRIO);
			
			BSP_LED_All_Off();
			Clean_Lcd(ScreenBuffer);
			Buzzer_ON();
			OSTimeDlyHMSM(0,0,1,0);//---------------------
			Set_Lcd(ScreenBuffer);
			BSP_LED_All_On();
			OSTimeDlyHMSM(0,0,2,0);
			OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
			OSTaskResume(Task_Status_Indicator_PRIO);
			OSTaskResume(APP_CFG_TASK_START_PRIO);
			OSTaskResume(Task_Time_Display_PRIO);
			OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_CLR,&err);
			OSFlagPost(FLAG_GRP,0xffff,OS_FLAG_CLR,&err);
			OSFlagPost(FLAG_GRP,MyFlags,OS_FLAG_SET,&err);
			Buzzer_OFF();
			GUI=Main_Menu;
		}break;
		case Operation :{
			Clean_Lcd(ScreenBuffer);
			OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_SET,&err);
			StringDisplay( "１更改地址", 0, 0,0, ScreenBuffer );
			StringDisplay( "２数据采样", 2, 0,0, ScreenBuffer );
			StringDisplay( "３部件统计", 4, 0,0, ScreenBuffer );
			StringDisplay( "４编　　程", 0, 80,0, ScreenBuffer );
#if (PROGRAM_TYPE == 1)
			StringDisplay( "５传感器设置", 2, 80,0, ScreenBuffer );
#endif
			//StringDisplay( "安吉斯智能设备", 6, 50,0, ScreenBuffer );
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
			if( err==OS_ERR_NONE ){
				switch(*My_Key_Message){
				case Key_1:{if(1 == ModifyAddr()) return ;}break;
				case Key_2:{if(1 == Sampling()) return ;}break;
				case Key_3:{if(1 == Bujiantongji()) return ;}break;
				case Key_4:{if(1 == MainBianChen()) return ;}break;
#if (PROGRAM_TYPE == 1)
				case Key_5:{if(1 == P_SetThreshold()) return;}break;
#endif
				case Key_Return:{GUI=Main_Menu;}break;
				default:break;
				}
			}else{
				return;
			}
		}break;
		case Record :{
			Clean_Lcd(ScreenBuffer);
			OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_SET,&err);
			StringDisplay( "１紧急记录", 0, 0,0, ScreenBuffer );
			StringDisplay( "２其它记录", 2, 0,0, ScreenBuffer );
			//StringDisplay( "安吉斯智能设备", 6, 50,0, ScreenBuffer );
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
			if( err==OS_ERR_NONE ){
				switch(*My_Key_Message){
				case Key_1:{if(1 == CheckRecord(FireFault)) return;}break;
				case Key_2:{if(1 == CheckRecord(OtherFault)) return;}break;
				case Key_Return:{GUI=Main_Menu;}break;
				default:break;
				}
			}else{
				return;
			}
		}break;
		case Set_Menu :{
			Clean_Lcd(ScreenBuffer);
			OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_SET,&err);
			StringDisplay( "１密码设置", 0, 0,0, ScreenBuffer );
			StringDisplay( "２时间设置", 2, 0,0, ScreenBuffer );
			StringDisplay( "３打印设置", 4, 0,0, ScreenBuffer );
			StringDisplay( "４机号设置", 0, 80,0, ScreenBuffer );
#if (PROGRAM_TYPE == 2)
			StringDisplay( "４探测器设置", 0, 80,0, ScreenBuffer );
			StringDisplay( "５机号设置", 2, 80,0, ScreenBuffer );
#endif

			//StringDisplay( "安吉斯智能设备", 6, 50,0, ScreenBuffer );
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
			if( err==OS_ERR_NONE ){
				switch(*My_Key_Message){
				case Key_1:{if(1 == ModifyPassword()) return;}break;
				case Key_2:{if(1 == SetTime()) return;}break;
				case Key_3:{if(1 == PrinterSet()) return;}break;
				case Key_4:{if(1 == MachineSet()) return;}break;

#if (PROGRAM_TYPE == 2)
				case Key_4:{if(1 == SetThreshold()) return;}break;
				case Key_5:{if(1 == MachineSet()) return;}break;
#endif

				case Key_Return:{GUI=Main_Menu;}break;
				default:break;
				}
			}else{
				return;
			}
		}break;
		case Code_Menu :{
			Clean_Lcd(ScreenBuffer);
			OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_SET,&err);
			StringDisplay( "１手动登记", 0, 0,0, ScreenBuffer );
			StringDisplay( "２自动登记", 2, 0,0, ScreenBuffer );
			StringDisplay( "３导入描述信息", 4, 0,0, ScreenBuffer );
			//StringDisplay( "安吉斯智能设备", 6, 50,0, ScreenBuffer );
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
			if( err==OS_ERR_NONE ){
				switch(*My_Key_Message){
				case Key_1:{if(1 == HuiLuBianChen()) return;}break;
				case Key_2:{AutoRegister();}break;
				case Key_3:{if(1 == Describe()) return;}break;
				case Key_Return:{GUI=Main_Menu;}break;
				default:break;
				}
			}else{
				return;
			}
		}break;
		default:break;
		}
	}
}

extern uint8_t regist_flag;
static uint8_t HuiLuBianChen(void)
{
	uint8_t  err;
	Key_enum *My_Key_Message;
	
	regist_flag = 0;
	for(;;){
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "探测器登记", 0, 0,1, ScreenBuffer );
		StringDisplay( "１回路１", 2, 0,0, ScreenBuffer );
		StringDisplay( "２回路２", 4, 0,0, ScreenBuffer );
		StringDisplay( "３回路３", 2, 80,0, ScreenBuffer );
		StringDisplay( "４回路４", 4, 80,0, ScreenBuffer );
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
		if( err==OS_ERR_NONE ){
			switch(*My_Key_Message){
			case Key_1:{Manual_Register(0);}break;
			case Key_2:{Manual_Register(1);}break;
			case Key_3:{Manual_Register(2);}break;
			case Key_4:{Manual_Register(3);}break;
			case Key_Return:
				if(regist_flag == 1) HAL_NVIC_SystemReset();
				else return 0;
			default:break;
			}
		}
		else
		{
			if(regist_flag == 1) HAL_NVIC_SystemReset();
			else  return 1;
		}
	}
}
static uint8_t MachineSet( void )
{
	Key_enum key;
	enum{
		area_code_Cursor=0,
		area_machine_Cursor
	}Cursor=area_code_Cursor;
	uint16_t crc16;
	Config_Store p;
	
	SpiFlashRead((uint8_t *)&p, CfgRecord_StartAddr, sizeof(Config_Store));
	crc16 = crc_16((uint8_t *)&p, sizeof(Config_TypeDef));
	if(crc16 != p.crc16||p.crc16==0xff)
	{
		StringDisplay( "机号读取失败！", 0, 0,1, ScreenBuffer );
		OSTimeDlyHMSM(0,0,3,0);
	}
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "机号设置", 0, 0,1, ScreenBuffer );
	StringDisplay( "本机区号:", 2, 0,0, ScreenBuffer );
	StringDisplay( "本机机号:", 4, 0,0, ScreenBuffer );
	DisplayValueN( p.config.machine_cfg.area_code, 2, 56, 0, 3,ScreenBuffer );
	DisplayValueN( p.config.machine_cfg.area_machine, 4, 56, 0, 3,ScreenBuffer );
	for(;;){
		DisplayValueN( p.config.machine_cfg.area_code, 2, 56, 0, 3,ScreenBuffer );
		DisplayValueN( p.config.machine_cfg.area_machine, 4, 56, 0, 3,ScreenBuffer );
		switch(Cursor){
			case area_code_Cursor:key = input_value8(&p.config.machine_cfg.area_code, 3, 2, 56); break;
			case area_machine_Cursor:key = input_value8(&p.config.machine_cfg.area_machine, 3, 4, 56); break;
		}
		switch(key)
		{
#if MAIN_BOARD==1
			case Key_x:
				if(Cursor==area_code_Cursor)Cursor=area_machine_Cursor;
				else Cursor=area_code_Cursor;
				break;
#endif
#if MAIN_BOARD==2
			case Key_Up:
				if(Cursor==area_machine_Cursor)Cursor=area_code_Cursor;break;
			case Key_Down:
				if(Cursor==area_code_Cursor)Cursor=area_machine_Cursor;break;
#endif
			case Key_Set:
				Config = p.config;
				config_store();
				break;
			case Key_Return: return 0;
			case Key_None:  return 1;
		}
	}
}









