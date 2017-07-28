#include "SetThreshold.h"

uint8_t SetThreshold(void)
{
	uint8_t  err;
	Key_enum *My_Key_Message;
	Key_enum key;
	uint8_t CardNum=1,Addr=1,Channel=1;
	uint16_t Value=100;
	uint8_t state = 0;
	enum{
		Set_Cursor=0,
		Read_Cursor=1,
		Calibrate_Cursor=2
	}SetRead_Cursor=Set_Cursor;
	enum{
		Card_Cursor=0,
		Addr_Cursor=1,
		Channel_Cursor = 2,
		Value_Cursor=3
	}Cursor=Card_Cursor;
	
menu_threshold:
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "１设置报警线", 0, 0, 0, ScreenBuffer );
	StringDisplay( "２读取报警线", 2, 0, 0, ScreenBuffer );
	StringDisplay( "３校准当前值", 4, 0, 0, ScreenBuffer );
wait_key:
	My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
	if( err==OS_ERR_NONE ){
		switch(*My_Key_Message){
			case Key_1:{SetRead_Cursor=Set_Cursor;Value=100;}break;
			case Key_2:{SetRead_Cursor=Read_Cursor;Value=0;}break;
			case Key_3:{SetRead_Cursor=Calibrate_Cursor;Value=100;}break;
			case Key_Return: return 0;
			default: goto wait_key;
		}
	}
	else return 1;
	
	Cursor=Card_Cursor;
	for(;;){
		Clean_Lcd(ScreenBuffer);
		if(SetRead_Cursor==Read_Cursor){
			StringDisplay( "读取报警线", 0, 0, 1, ScreenBuffer );
		}else if(SetRead_Cursor==Set_Cursor){
			StringDisplay( "设置报警线", 0, 0, 1, ScreenBuffer );
		}else{
			StringDisplay( "输入当前值", 0, 0, 1, ScreenBuffer );
		}
		StringDisplay( "回路：", 2, 24, 0, ScreenBuffer );
		StringDisplay( "地址：", 2, 96, 0, ScreenBuffer );
		StringDisplay( "通道：", 4, 24, 0, ScreenBuffer );
		StringDisplay( "限值：", 4, 96, 0, ScreenBuffer );
#if MAIN_BOARD==1
		StringDisplay( "按＊切换", 6, 0, 0, ScreenBuffer );
#endif
		DisplayValueN( CardNum, 2, 60, 0, 3, ScreenBuffer);
		DisplayValueN( Addr, 2, 132, 0, 3, ScreenBuffer);
		DisplayValueN( Channel, 4, 60, 0, 3, ScreenBuffer);
		DisplayValueN( Value, 4, 132,0, 4, ScreenBuffer);
		
		switch(Cursor){
			case Card_Cursor: key = input_value8(&CardNum, 3, 2, 60); break;
			case Addr_Cursor: key = input_value8(&Addr, 3, 2, 132); break;
			case Channel_Cursor: key = input_value8(&Channel, 3, 4, 60); break;
			case Value_Cursor: key = input_value16(&Value, 4, 4, 132); break;
		}

		switch(key)
		{
#if MAIN_BOARD==1
			case Key_x:
				if(Cursor==Card_Cursor) Cursor=Addr_Cursor;
				else if(Cursor==Addr_Cursor) Cursor=Channel_Cursor;
				else if(Cursor==Channel_Cursor && SetRead_Cursor!=Read_Cursor) Cursor=Value_Cursor;
				else if(Cursor==Channel_Cursor && SetRead_Cursor==Read_Cursor) Cursor=Card_Cursor;
				else Cursor=Card_Cursor;
				break;
#endif
#if MAIN_BOARD==2
			case Key_Up:
				if(Cursor==Channel_Cursor) Cursor=Card_Cursor;
				else if(Cursor==Value_Cursor) Cursor=Addr_Cursor;
				break;
			case Key_Down:
				if(Cursor==Card_Cursor) Cursor=Channel_Cursor;
				else if(Cursor==Addr_Cursor) Cursor=Channel_Cursor;
				else if(Cursor==Addr_Cursor && SetRead_Cursor!=Read_Cursor) Cursor=Value_Cursor;
				break;
			case Key_Lift:
				if(Cursor==Addr_Cursor) Cursor=Card_Cursor;
				else if(Cursor==Channel_Cursor) Cursor=Addr_Cursor;
				else if(Cursor==Value_Cursor) Cursor=Channel_Cursor;
				break;
			case Key_Right:
				if(Cursor==Card_Cursor) Cursor=Addr_Cursor;
				else if(Cursor==Addr_Cursor) Cursor=Channel_Cursor;
				else if(Cursor==Channel_Cursor && SetRead_Cursor!=Read_Cursor) Cursor=Value_Cursor;
				break;
#endif
			case Key_Set:
				if(SetRead_Cursor==Set_Cursor)
				{
					if(SUCCESS != set_threshold_detector(CardNum, Addr, Channel-1, Value, &state))
					{
						Clean_Lcd(ScreenBuffer);
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
					}
					else
					{
						Clean_Lcd(ScreenBuffer);
						if(state == 0x01) StringDisplay( "设置成功", 2, 72, 0, ScreenBuffer );
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "设置失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						if(state == 0x01) StringDisplay( "设置成功", 2, 72, 0, ScreenBuffer );
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "设置失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						if(state == 0x01) StringDisplay( "设置成功", 2, 72, 0, ScreenBuffer );
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "设置失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
					}
				}
				else if(SetRead_Cursor==Read_Cursor)
				{
					if(SUCCESS != get_threshold_detector(CardNum, Addr, Channel-1, &Value, &state))
					{
						Clean_Lcd(ScreenBuffer);
						Value = 0;
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
					}
					else if(state != 0x01)
					{
						Clean_Lcd(ScreenBuffer);
						Value = 0;
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "读取失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "读取失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "读取失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
					}
				}
				else
				{
					if(SUCCESS != calibration_detector(CardNum, Addr, Channel-1, Value, &state))
					{
						Clean_Lcd(ScreenBuffer);
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
					}
					else
					{
						Clean_Lcd(ScreenBuffer);
						if(state == 0x01) StringDisplay( "设置成功", 2, 72, 0, ScreenBuffer );
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "校准失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						if(state == 0x01) StringDisplay( "设置成功", 2, 72, 0, ScreenBuffer );
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "校准失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						if(state == 0x01) StringDisplay( "设置成功", 2, 72, 0, ScreenBuffer );
						if(state == 0x02) StringDisplay( "地址错误", 2, 72, 0, ScreenBuffer );
						if(state == 0x03) StringDisplay( "校准失败", 2, 72, 0, ScreenBuffer );
						if(state == 0x04) StringDisplay( "通道错误", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,1,0);
						OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
					}
				}
				break;
			case Key_Return: goto menu_threshold;
			case Key_None: return 1;
		}
	}
}
//消防电源的设置
uint8_t P_SetThreshold(void)
{
	uint8_t  err;
	Key_enum *My_Key_Message;
	Key_enum key;
	uint8_t CardNum=1,Addr=1;
	uint16_t ValueL=187,ValueH=242;
	uint8_t state = 0;
	uint32_t current = 0;
	uint8_t unit = 3;
	enum{
		Read_Voltage_Cursor=0,
		Read_Current_Cursor=1,
		Set_Voltage_Cursor=2,
		Set_Current_Cursor=3,
		Calibrate_Voltage_Cursor=4,
		Calibrate_Current_Cursor=5,
	}Set_Cursor=Set_Voltage_Cursor;
	enum{
		Card_Cursor=0,
		Addr_Cursor=1,
		ValueL_Cursor = 2,
		ValueH_Cursor=3,
		Unit_Cursor=4
	}Cursor=Card_Cursor;

menu_threshold:
	CardNum = 1;
	Addr = 1;
	unit = 3;
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "１读取电压阈值", 0, 0, 0, ScreenBuffer );
	StringDisplay( "２读取电流阈值", 2, 0, 0, ScreenBuffer );
	StringDisplay( "３设置电压阈值", 4, 0, 0, ScreenBuffer );
	StringDisplay( "４设置电流阈值", 6, 0, 0, ScreenBuffer );
	StringDisplay( "５校准电压值", 0, 108, 0, ScreenBuffer );
	StringDisplay( "６校准电流值", 2, 108, 0, ScreenBuffer );
wait_key:
	My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
	if( err==OS_ERR_NONE ){
		switch(*My_Key_Message){
			case Key_1:{Set_Cursor=Read_Voltage_Cursor;ValueL=0;ValueH=0;}break;
			case Key_2:{Set_Cursor=Read_Current_Cursor;ValueL=0;}break;
			case Key_3:{Set_Cursor=Set_Voltage_Cursor;ValueL=187;ValueH=242;}break;
			case Key_4:{Set_Cursor=Set_Current_Cursor;ValueL=5500;}break;
			case Key_5:{Set_Cursor=Calibrate_Voltage_Cursor;ValueL=100;}break;
			case Key_6:{Set_Cursor=Calibrate_Current_Cursor;ValueL=300;}break;
			case Key_Return: return 0;
			default: goto wait_key;
		}
	}
	else return 1;
	
	Cursor=Card_Cursor;
	for(;;){
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "回路：", 2, 24, 0, ScreenBuffer );
		StringDisplay( "地址：", 2, 96, 0, ScreenBuffer );
		switch(Set_Cursor)
		{
			case Read_Voltage_Cursor:
			{
				StringDisplay( "读取电压阈值", 0, 0, 1, ScreenBuffer );
				StringDisplay( "下限：", 4, 24, 0, ScreenBuffer );
				StringDisplay( "上限：", 4, 96, 0, ScreenBuffer );
				break;
			}
			case Read_Current_Cursor:
			{
				StringDisplay( "读取电流阈值", 0, 0, 1, ScreenBuffer );
				StringDisplay( "阈值：", 4, 24, 0, ScreenBuffer );
				break;
			}
			case Set_Voltage_Cursor:
			{
				StringDisplay( "设置电压阈值", 0, 0, 1, ScreenBuffer );
				StringDisplay( "下限：", 4, 24, 0, ScreenBuffer );
				StringDisplay( "上限：", 4, 96, 0, ScreenBuffer );
				break;
			}
			case Set_Current_Cursor:
			{
				StringDisplay( "设置电流阈值", 0, 0, 1, ScreenBuffer );
				StringDisplay( "阈值：", 4, 24, 0, ScreenBuffer );
				break;
			}
			case Calibrate_Voltage_Cursor:
			{
				StringDisplay( "校准电压值", 0, 0, 1, ScreenBuffer );
				StringDisplay( "校准：", 4, 24, 0, ScreenBuffer );
				break;
			}
			case Calibrate_Current_Cursor:
			{
				StringDisplay( "校准电流值", 0, 0, 1, ScreenBuffer );
				StringDisplay( "校准：", 4, 24, 0, ScreenBuffer );
				break;
			}
		}

#if MAIN_BOARD==1
		StringDisplay( "按＊切换", 6, 0, 0, ScreenBuffer );
#endif
		DisplayValueN( CardNum, 2, 60, 0, 3, ScreenBuffer);
		DisplayValueN( Addr, 2, 132, 0, 3, ScreenBuffer);
		if(Set_Cursor == Set_Current_Cursor || Set_Cursor == Calibrate_Current_Cursor || Set_Cursor == Read_Current_Cursor)
		{
			DisplayValueN( ValueL, 4, 60, 0, 4, ScreenBuffer);
			if(Cursor == Unit_Cursor && unit == 0) StringDisplay("A", 4, 84, 1, ScreenBuffer);
			else if(Cursor != Unit_Cursor && unit == 0) StringDisplay("A", 4, 84, 0, ScreenBuffer);
			else if(Cursor == Unit_Cursor && unit != 0) StringDisplay("mA", 4, 84, 1, ScreenBuffer);
			else if(Cursor != Unit_Cursor && unit != 0) StringDisplay("mA", 4, 84, 0, ScreenBuffer);
		}
		else DisplayValueN( ValueL, 4, 60, 0, 3, ScreenBuffer);
		if(Set_Cursor == Set_Voltage_Cursor || Set_Cursor == Read_Voltage_Cursor)
		{
			DisplayValueN( ValueH, 4, 132,0, 3, ScreenBuffer);
		}
		
		switch(Cursor){
			case Card_Cursor: key = input_value8(&CardNum, 3, 2, 60); break;
			case Addr_Cursor: key = input_value8(&Addr, 3, 2, 132); break;
			case ValueL_Cursor:
				if((Set_Cursor == Set_Current_Cursor) ||
				   (Set_Cursor == Calibrate_Current_Cursor)) key = input_value16(&ValueL, 4, 4, 60);
				else key = input_value16(&ValueL, 3, 4, 60);
				break;
			case ValueH_Cursor: key = input_value16(&ValueH, 3, 4, 132); break;
			case Unit_Cursor: key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,MenuDelay,&err)); break;
		}

		switch(key)
		{
#if MAIN_BOARD==1
			case Key_x:
				if(Cursor==Card_Cursor) Cursor=Addr_Cursor;
				else if(Cursor==Addr_Cursor)
				{
					if(Set_Cursor == Read_Voltage_Cursor || Set_Cursor == Read_Current_Cursor) Cursor=Card_Cursor;
					else Cursor=ValueL_Cursor;
				}
				else if(Cursor==ValueL_Cursor)
				{
					if(Set_Cursor == Set_Voltage_Cursor) Cursor=ValueH_Cursor;
					else if(Set_Cursor == Calibrate_Voltage_Cursor) Cursor=Card_Cursor;
					else Cursor=Unit_Cursor;
				}
				else if(Cursor==ValueH_Cursor) Cursor=Card_Cursor;
				else if(Cursor==Unit_Cursor) Cursor=Card_Cursor;
				break;
			case Key_2:
			case Key_8:
				if(Cursor == Unit_Cursor)
				{
					if(unit == 0) unit = 3;
					else unit = 0;
				}
				break;
#endif
#if MAIN_BOARD==2
			case Key_Lift:
				if(Cursor==Card_Cursor)
				{
					if(Set_Cursor == Set_Voltage_Cursor) Cursor=ValueH_Cursor;
					else if(Set_Cursor == Calibrate_Voltage_Cursor) Cursor=ValueL_Cursor;
					if(Set_Cursor == Read_Voltage_Cursor || Set_Cursor == Read_Current_Cursor) Cursor=Addr_Cursor;
					else Cursor=Unit_Cursor;
				}
				else if(Cursor==Addr_Cursor) Cursor=Card_Cursor;
				else if(Cursor==ValueL_Cursor) Cursor=Addr_Cursor;
				else if(Cursor==ValueH_Cursor) Cursor=ValueL_Cursor;
				else if(Cursor==Unit_Cursor) Cursor=ValueL_Cursor;
				break;
			case Key_Right:
				if(Cursor==Card_Cursor) Cursor=Addr_Cursor;
				else if(Cursor==Addr_Cursor)
				{
					if(Set_Cursor == Read_Voltage_Cursor || Set_Cursor == Read_Current_Cursor) Cursor=Card_Cursor;
					else Cursor=ValueL_Cursor;
				}
				else if(Cursor==ValueL_Cursor)
				{
					if(Set_Cursor == Set_Voltage_Cursor) Cursor=ValueH_Cursor;
					else if(Set_Cursor == Calibrate_Voltage_Cursor) Cursor=Card_Cursor;
					else Cursor=Unit_Cursor;
				}
				else if(Cursor==ValueH_Cursor) Cursor=Card_Cursor;
				else if(Cursor==Unit_Cursor) Cursor=Card_Cursor;
				break;
			case Key_Up:
			case Key_Down:
				if(Cursor == Unit_Cursor)
				{
					if(unit == 0) unit = 3;
					else unit = 0;
				}
				break;
#endif
			case Key_Set:
				switch(Set_Cursor)
				{
					case Read_Voltage_Cursor: err = P_read_threshold_detector(CardNum, Addr, 0x11, &ValueL, &ValueH, &state); break;
					case Read_Current_Cursor: err = P_read_threshold_detector(CardNum, Addr, 0x22, &ValueL, &ValueH, &state); break;
					case Set_Voltage_Cursor: err = P_set_threshold_detector(CardNum, Addr, 0x11, ValueL, ValueH, &state); break;
					case Set_Current_Cursor: err = P_set_threshold_detector(CardNum, Addr, 0x22, ValueL|(unit<<14), 0, &state); break;
					case Calibrate_Voltage_Cursor: err = calibration_detector(CardNum, Addr, 0x11, ValueL, &state); break;
					case Calibrate_Current_Cursor: err = calibration_detector(CardNum, Addr, 0x22, ValueL|(unit<<14), &state); break;
				}
				if(err != SUCCESS)
				{
					Clean_Lcd(ScreenBuffer);
					StringDisplay( "通讯失败", 2, 72, 0, ScreenBuffer );
					OSTimeDlyHMSM(0,0,3,0);
					OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
				}
				else
				{
					Clean_Lcd(ScreenBuffer);
					if(Set_Cursor == Read_Voltage_Cursor || Set_Cursor == Read_Current_Cursor)
					{
						if(state != 0x01)
						{
							StringDisplay( "读取失败", 2, 72, 0, ScreenBuffer );
							OSTimeDlyHMSM(0,0,3,0);
						}
						else
						{
							if(Set_Cursor == Read_Current_Cursor)
							{
								if((ValueL>>14) == 3) current = ValueL&0x3fff;
								if((ValueL>>14) == 2) current = (ValueL&0x3fff)*10;
								if((ValueL>>14) == 1) current = (ValueL&0x3fff)*100;
								if((ValueL>>14) == 0) current = (ValueL&0x3fff)*1000;
								if(current <= 9999){ValueL = current; unit = 3;}
								else {ValueL = current/1000; unit = 0;}
							}
						}
					}
					else
					{
						if(state == 0x01) StringDisplay( "设置成功", 2, 72, 0, ScreenBuffer );
						else	StringDisplay( "设置失败", 2, 72, 0, ScreenBuffer );
						OSTimeDlyHMSM(0,0,3,0);
					}
					OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
				}
				break;
			case Key_Return: goto menu_threshold;
			case Key_None: return 1;
		}
	}
}
