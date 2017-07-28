#include "ModifyAddr.h"

static void ModifyAddrCommunicate( uint8_t Card_ID,uint8_t OldAddr, uint8_t NewAddr );
static uint8_t ModifyDetectorAddr( uint8_t Card_ID );

uint8_t ModifyAddr(void)
{
	uint8_t  err;
	Key_enum *My_Key_Message;
	for(;;){
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "选择要修改的回路", 0, 0, 1, ScreenBuffer );
		StringDisplay( "１回路１", 2, 0, 0, ScreenBuffer );
		StringDisplay( "２回路２", 4, 0, 0, ScreenBuffer );
		StringDisplay( "３回路３", 2, 80, 0, ScreenBuffer );
		StringDisplay( "４回路４", 4, 80, 0, ScreenBuffer );
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
		if( err==OS_ERR_NONE ){
			switch(*My_Key_Message){
			case Key_1:{if(1 == ModifyDetectorAddr(1)) return 1;}break;
			case Key_2:{if(1 == ModifyDetectorAddr(2)) return 1;}break;
			case Key_3:{if(1 == ModifyDetectorAddr(3)) return 1;}break;
			case Key_4:{if(1 == ModifyDetectorAddr(4)) return 1;}break;
			case Key_Return:return 0;
			default:break;
			}
		}else
			return 1;
	}
}

static uint8_t ModifyDetectorAddr( uint8_t Card_ID )
{
	uint8_t  err;
	Key_enum key;
	uint8_t Old=0,New=0;
	enum{
		OldCursor,
		NewCursor
	}Cursor=OldCursor;
	for(;;){
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "修改探测器地址：", 0, 0, 1, ScreenBuffer );
#if MAIN_BOARD==1
		StringDisplay( "按＊切换", 2, 144, 0, ScreenBuffer );
		StringDisplay( "按设置键确定修改", 6, 0, 0, ScreenBuffer );
#endif
		StringDisplay( "待更改地址：", 2, 12, 0, ScreenBuffer );
		StringDisplay( "　　新地址：", 4, 12, 0, ScreenBuffer );
		
		DisplayValueN( Old,  2, 84, 0, 3, ScreenBuffer);
		DisplayValueN( New,  4, 84, 0, 3, ScreenBuffer);
		
		switch(Cursor){
			case OldCursor: key = input_value8(&Old, 3, 2, 84); break;
			case NewCursor: key = input_value8(&New, 3, 4, 84); break;
		}
		
		switch(key)
		{
#if MAIN_BOARD==2
			case Key_Up:
				if(Cursor==NewCursor){Cursor=OldCursor;break;}else continue;
			case Key_Down:
				if(Cursor==OldCursor){Cursor=NewCursor;break;}else continue;
#endif
#if MAIN_BOARD==1
			case Key_x:
			switch(Cursor)
			{
				case OldCursor: Cursor=NewCursor; break;
				case NewCursor: Cursor=OldCursor; break;
			}
			break;
#endif
			case Key_Set:
				OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_SET,&err);
				ModifyAddrCommunicate( Card_ID,Old,New );
				OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_CLR,&err);
				break;
			case Key_Return:return 0;
			case Key_None: return 1;
		}
		
	}
}

static void ModifyAddrCommunicate( uint8_t Card_ID,uint8_t OldAddr, uint8_t NewAddr )
{
	Detector_Cfg_struct detector;
	
	if(SUCCESS == charge_addr_card(Card_ID, OldAddr, NewAddr))
	{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "地址修改成功！", 2, 48, 0, ScreenBuffer );
		SpiFlashRead((uint8_t*)&detector,
					 ((Card_ID-1)*200+(OldAddr-1))*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
					 sizeof(Detector_Cfg_struct));
		detector.address = NewAddr;
		SpiFlashWrite((uint8_t*)"/xff/xff/xff/xff",
							 ((Card_ID-1)*200+(OldAddr-1))*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
							 sizeof(Detector_Cfg_struct));
		SpiFlashWrite((uint8_t*)&detector,
							 ((Card_ID-1)*200+(NewAddr-1))*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
							 sizeof(Detector_Cfg_struct));
		OSTimeDlyHMSM(0,0,1,0);
	}
	else
	{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "地址修改失败！", 2, 48, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
	}
}
