#include "PrinterSet.h"

uint8_t PrinterSet(void)
{
	uint8_t  err;
	Key_enum *My_Key_Message;

	Clean_Lcd(ScreenBuffer);
	for(;;){
		StringDisplay( "打印机状态：", 0, 0, 0, ScreenBuffer );
		StringDisplay( "打印开关：", 2, 0, 0, ScreenBuffer );
		StringDisplay( "按＊切换", 6, 0, 0, ScreenBuffer );
#if MAIN_BOARD==1
		StringDisplay( "按设置键保存", 6, 72, 0, ScreenBuffer );
#endif
#if MAIN_BOARD==2
		StringDisplay( "按确认键保存", 6, 72, 0, ScreenBuffer );
#endif
		if(SET==GetPrinterState( NoPaper ))
			StringDisplay( "缺纸", 0, 72, 0, ScreenBuffer );
		else
			StringDisplay( "正常", 0, 72, 0, ScreenBuffer );
	
		if(Config.print_state){
			StringDisplay( "开", 2, 60, 0, ScreenBuffer );
		}else{
			StringDisplay( "关", 2, 60, 0, ScreenBuffer );
		}
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,5000,&err);//等待按键
		if( err==OS_ERR_NONE ){
			switch(*My_Key_Message){
			case Key_x:{
				Config.print_state = !Config.print_state;
			}break;
			case Key_Set:{
				config_store();
				return 0;
			}break;
			case Key_Return:return 0;
			default:break;
			}
		}
		else return 1;
	}
}

void PrintOneFault( Record_struct Record )
{
	Position_Typedef Position={"你好123"};
	/*打印时间*/
	printf("20%.2d-%.2d-%.2d %.2d:%.2d:%.2d\n",Record.time.year,Record.time.month,Record.time.day,Record.time.hour,Record.time.minute,Record.time.second);
	/*探测器中文描述*/
	if(Record.event.card_code!=0){
		SpiFlashRead((uint8_t *)&Position,Record.event.card_code*SPI_FLASH_SECTOR_SIZE+(Record.event.address)*sizeof(Position_Typedef)+DescriptionRecord_StartAddr,(uint16_t)sizeof(Position_Typedef));//读当中文描述
		if(Position[0]==0xFF){printf("位置：无\n");
		}else{printf("位置：%s\n",Position);}
	}else{
		printf("位置：本机\n");
	}
	/*打印位置*/
	if(Record.event.card_code == 0)
	{
		printf("地址：本机\n");
	}
	else if(Record.event.address == 0)
	{
		printf("地址：%.2d回路\n",Record.event.card_code);
	}
	else if(Record.event.channel == 0)
	{
		printf("地址：%.2d回路 %.2d号\n",Record.event.card_code,Record.event.address);
	}
	else
	{
		printf("地址：%.2d回路 %.2d号 %.2d通道\n",Record.event.card_code,Record.event.address,Record.event.channel);
	}
	/*打印故障类型*/
	switch(Record.event.event_type){
	case 0x08:{//探测器故障
		switch(Record.event.fault_type){
		case 0x01:{printf("类型：探测器丢失\n");}break;
		case 0x02:{printf("类型：维护\n");}break;
		case 0x03:{printf("类型：重码\n");}break;
		case 0x04:{
			switch(Record.event.description){
			case 1:{printf("类型：Ａ相过压\n");}break;
			case 2:{printf("类型：Ｂ相过压\n");}break;
			case 4:{printf("类型：Ｃ相过压\n");}break;
			}
		}break;
		case 0x05:{
			switch(Record.event.description){
			case 1:{printf("类型：Ａ相欠压\n");}break;
			case 2:{printf("类型：Ｂ相欠压\n");}break;
			case 4:{printf("类型：Ｃ相欠压\n");}break;
			}
		}break;
		case 0x06:{
			switch(Record.event.description){
			case 1:{printf("类型：Ａ相过流\n");}break;
			case 2:{printf("类型：Ｂ相过流\n");}break;
			case 4:{printf("类型：Ｃ相过流\n");}break;
			}
			}break;
		case 0x07:{
			switch(Record.event.description){
			case 1:{printf("类型：Ａ相缺相\n");}break;
			case 2:{printf("类型：Ｂ相缺相\n");}break;
			case 4:{printf("类型：Ｃ相缺相\n");}break;
			}
			}break;
		case 0x08:{printf("类型：错相故障\n");}break;
		case 0x09:{printf("类型：电源中断\n");}break;
		case 0x11:{printf("类型：漏电传感器丢失\n");}break;
		case 0x12:{printf("类型：漏电报警\n");}break;
		case 0x13:{printf("类型：温度传感器丢失\n");}break;
		case 0x14:{printf("类型：温度报警\n");}break;
		}
	}break;
	case 0x0A:{//回路故障
		switch(Record.event.fault_type){
		case 0x01:{printf("类型：回路短路\n");}break;
		case 0x02:{printf("类型：负载未接\n");}break;
		case 0x03:{printf("类型：回路超载\n");}break;
		case 0x04:{printf("类型：回路接地\n");}break;
		case 0x05:{printf("类型：回路丢失\n");}break;
		}
	}break;
	case 0x0C:{//电源故障
		switch(Record.event.fault_type){
		case 0x01:{printf("类型：主电故障\n");}break;
		case 0x02:
		case 0x03:{printf("类型：备电故障\n");}break;
		case 0x04:{printf("类型：电源短路\n");}break;
		case 0x05:{printf("类型：电源断线\n");}break;
		}
	}break;
	case 0x0E:{
	}break;
	case 0x0F:{//系统
		switch(Record.event.fault_type)
		{
			case 0x01:{printf("类型：系统上电\n");}break;
		}
	}break;
	}
	Move_Paper( 35 );
}
