#include "CheckRecord.h"

typedef enum{
	Front_Add=0,
	Back_Add=1,
	Print=2,
	Now_Add=3,
}Way_enum;

static void DisplayFault(uint16_t Number, Record_struct Record, uint8_t Index )
{
	TypeDef_Fault_Display_t fault;
	
	fault.CardAddress = Record.event.card_code;
	fault.DetectorAddress = Record.event.address;
	fault.DetectorChannel = Record.event.channel;
	fault.Event_Type = Record.event.event_type;
	fault.Fault_Type = Record.event.fault_type;
	fault.Fault_Type_Dsc = Record.event.description;
	fault.time = Record.time;
	display_fault(Number, &fault, Index);
}

static enum{
	ScreenNone=0,
	ScreenOne=1,
	ScreenTwo=3
}ScreenState=ScreenNone;

static struct{
	Record_struct Record;
	uint16_t Index;//记录索引
	uint16_t Number;//翻到的编号
}Screen1,Screen2;

static uint16_t TotalFaultNum=0;

static void Fault_Record_Display_Manage( CheckRecordType_enum CheckRecordType,Way_enum Way )
{
	uint32_t FaultAddr;
	Record_State_struct *record_state;
	
	if(Way==Print)
	{
		if((CheckRecordType == FireFault && Record_State.fire.Index != 0) ||
		   (CheckRecordType == OtherFault && Record_State.fault.Index != 0))
		{
			PrintOneFault( Screen1.Record );
		}
		return;
	}
	
	if(CheckRecordType==FireFault){
		Clean_Lcd(ScreenBuffer);
#if (PROGRAM_TYPE == 1)
			StringDisplay( "故", 0, 0, 0, ScreenBuffer );
			StringDisplay( "障", 2, 0, 0, ScreenBuffer );
			StringDisplay( "数", 4, 0, 0, ScreenBuffer );
#else
			StringDisplay( "报", 0, 0, 0, ScreenBuffer );
			StringDisplay( "警", 2, 0, 0, ScreenBuffer );
			StringDisplay( "数", 4, 0, 0, ScreenBuffer );
#endif
		FaultAddr=FireRecord_StartAddr;
		record_state = &Record_State.fire;
	}else{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "故", 0, 0, 0, ScreenBuffer );
		StringDisplay( "障", 2, 0, 0, ScreenBuffer );
		StringDisplay( "数", 4, 0, 0, ScreenBuffer );
		FaultAddr=FaultRecord_Start;
		record_state = &Record_State.fault;
	}
	
	switch(ScreenState){
	case ScreenNone:{
		if(record_state->Index==0){//判断0里有没有记录
			StringDisplay( "无记录", 2, 30,0, ScreenBuffer );
			return;
		}
		
		/*显示故障总数*/
		if(record_state->FullNumber==0){
			TotalFaultNum=record_state->Index;//最后一条索引号+1就是人理解的故障总数，也就是1屏的条目号
		}else{
			if((record_state->Index%PageFault)==0)
				TotalFaultNum=TotalFault;
			else
				TotalFaultNum=TotalFault-PageFault+record_state->Index%PageFault;//擦除一个扇区256条记录后，加上再存的条数
		}
		DisplayValueN(TotalFaultNum, 6, 0, 0, 4, ScreenBuffer);
		
		Screen1.Index=record_state->Index-1;
		Screen1.Number=TotalFaultNum;
		SpiFlashRead((uint8_t *)&(Screen1.Record),
					 (Screen1.Index)*((uint16_t)sizeof(Record_struct))+FaultAddr,
					 (uint16_t)sizeof(Record_struct));//读1屏记录
		DisplayFault( Screen1.Number, Screen1.Record, 1 );//显示1屏
		
		
		if(TotalFaultNum>1){
			/*可以显示2屏*/
			Screen2.Number=Screen1.Number-1;
			if(Screen1.Index==0)
				Screen2.Index=TotalFault-1;
			else
				Screen2.Index=Screen1.Index-1;
			SpiFlashRead((uint8_t *)&(Screen2.Record),
						 (Screen2.Index)*((uint16_t)sizeof(Record_struct))+FaultAddr,
						 (uint16_t)sizeof(Record_struct));//读2屏记录
			DisplayFault( Screen2.Number, Screen2.Record, 2 );//显示2屏
			ScreenState=ScreenTwo;//屏状态为已经显示2条了
		}else{
			ScreenState=ScreenNone;
		}
	}break;
	case ScreenTwo:{
		DisplayValueN(TotalFaultNum, 6, 0, 0, 4, ScreenBuffer);
		if(Way==Front_Add)
		{
			if(Screen2.Number>1){
				Screen1=Screen2;//2屏内容调到1屏
				DisplayFault( Screen1.Number, Screen1.Record, 1 );//显示1屏
				if((Screen2.Index==0)&&record_state->FullNumber)
					Screen2.Index=TotalFault;
				Screen2.Index--;
				Screen2.Number--;
				SpiFlashRead((uint8_t *)&(Screen2.Record),
							 (Screen2.Index)*((uint16_t)sizeof(Record_struct))+FaultAddr,
							 (uint16_t)sizeof(Record_struct));//读2屏记录
				DisplayFault( Screen2.Number, Screen2.Record, 2 );//显示2屏
			}
			else{
				DisplayFault( Screen1.Number, Screen1.Record, 1 );//显示1屏
				DisplayFault( Screen2.Number, Screen2.Record, 2 );//显示2屏
			}
		}
		else if(Way==Back_Add)
		{
			if(Screen1.Number<TotalFaultNum){
				Screen2=Screen1;//1屏内容调到2屏
				DisplayFault( Screen2.Number, Screen2.Record, 2 );//显示2屏
				Screen1.Index++;
				Screen1.Number++;
				if(Screen1.Index==TotalFault)
					Screen1.Index=0;
				SpiFlashRead((uint8_t *)&(Screen1.Record),
							 (Screen1.Index)*((uint16_t)sizeof(Record_struct))+FaultAddr,
							 (uint16_t)sizeof(Record_struct));//读1屏记录
				DisplayFault( Screen1.Number, Screen1.Record, 1 );//显示1屏
			}else{
				DisplayFault( Screen1.Number, Screen1.Record, 1 );//显示1屏
				DisplayFault( Screen2.Number, Screen2.Record, 2 );//显示2屏
			}
		}
		else
		{
			DisplayFault( Screen1.Number, Screen1.Record, 1 );//显示1屏
			DisplayFault( Screen2.Number, Screen2.Record, 2 );//显示2屏
		}
	}break;
	}
}

#define RECORD_FRESH_CYC	500
uint8_t CheckRecord( CheckRecordType_enum CheckRecordType )
{
	uint8_t  err;
	uint16_t counter = 0;
	Key_enum *My_Key_Message;
	
	OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_CLR,&err);
	ScreenState=ScreenNone;
	TotalFaultNum=0;
	for(;;){
		Fault_Record_Display_Manage( CheckRecordType,Now_Add );
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,RECORD_FRESH_CYC,&err);//等待按键
		if( err==OS_ERR_NONE ){
			counter = 0;
#if MAIN_BOARD==2
			switch(*My_Key_Message){
			case Key_Up:{//向最新记录查看（向后）
				Fault_Record_Display_Manage( CheckRecordType,Back_Add );
			}break;
			case Key_Down:{//向以前记录查看（向前）
				Fault_Record_Display_Manage( CheckRecordType,Front_Add );
			}break;
			case Key_Print:{//
				Fault_Record_Display_Manage( CheckRecordType,Print );
				OSTimeDlyHMSM(0,0,0,800);
				OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
			}break;
			case Key_Return:{ScreenState=ScreenNone;return 0;}break;
			}
#endif
#if MAIN_BOARD==1
			switch(*My_Key_Message){
			case Key_2:{
				Fault_Record_Display_Manage( CheckRecordType,Back_Add );
			}break;
			case Key_8:{
				Fault_Record_Display_Manage( CheckRecordType,Front_Add );
			}break;
			case Key_7:{
				Fault_Record_Display_Manage( CheckRecordType,Print );
				OSTimeDlyHMSM(0,0,0,800);
				OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
			}break;
			case Key_Return:{ScreenState=ScreenNone;return 0;}break;
			}
#endif
		}
		else
		{
			counter++;
			if(counter >= MenuDelay/RECORD_FRESH_CYC)
			{
				return 1;
			}
		}
	}
}