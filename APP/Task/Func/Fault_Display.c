#include "Fault_Display.h"

void display_fault(uint16_t index, TypeDef_Fault_Display_t *fault, uint8_t page)
{
	uint8_t StartAddr=0;
	uint8_t StartPage=0;
	
	if(page==1){
		DisplaySystime(*((SYSTEMTIME *)(&fault->time)), 2, ScreenBuffer);
		StartPage=0;
	}
	if(page==2){
		DisplaySystime(*((SYSTEMTIME *)(&fault->time)), 6, ScreenBuffer);
		StartPage=4;
	}
	StartAddr=18;
	/* 编号 */
	DisplayValueN( index, StartPage, StartAddr-3, 0, 3, ScreenBuffer);StartAddr+=30;
	if(fault->CardAddress!=0){
		/* 回路号 */
		if(fault->CardAddress>99)fault->CardAddress=0;
		DisplayValueN(fault->CardAddress, StartPage, StartAddr, 0, 2, ScreenBuffer);StartAddr+=12;
		//Display_Number( 12, StartPage, StartAddr,0 );StartAddr+=6;// -
		if(fault->DetectorAddress!=0){
			StringDisplay("-", StartPage, StartAddr, 0, ScreenBuffer); StartAddr+=6;
			/* 地址 */
			DisplayValueN( fault->DetectorAddress, StartPage, StartAddr, 0, 3, ScreenBuffer);StartAddr+=18;
			//Display_Number( 12, StartPage, StartAddr,0 );StartAddr+=6;// -
			/* 通道号 */
			if(fault->DetectorChannel>99)fault->DetectorChannel=0;
			if(fault->DetectorChannel!=0)
			{
				StringDisplay("-", StartPage, StartAddr, 0, ScreenBuffer); StartAddr+=6;
				DisplayValueN(fault->DetectorChannel, StartPage, StartAddr, 0, 2, ScreenBuffer);StartAddr+=12+12;
			}
			else
			{
				StartAddr+=24+6;
			}
		}
		else
		{
			StringDisplay("回路　　　　", StartPage, StartAddr, 0, ScreenBuffer);StartAddr+=48+6;
		}
	}
	else
	{
		StringDisplay("　本机　　　", StartPage, StartAddr, 0, ScreenBuffer);StartAddr+=60+6;
	}
	/*描述信息*/
	//StringDisplay( Fault_Device.Detector_Dsc, StartPage+2, 36,0, ScreenBuffer );
	/* 类型 */
	switch(fault->Event_Type){
		case 0x08:{//探测器故障
			switch(fault->Fault_Type){
#if (PROGRAM_TYPE == 1)
			case 0x01:{StringDisplay( "传感器丢失", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
#endif
#if (PROGRAM_TYPE == 2)
			case 0x01:{StringDisplay( "探测器丢失", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
#endif
			case 0x02:{StringDisplay( "维　　护", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x03:{StringDisplay( "重　　码", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x04:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "Ａ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "Ｂ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "Ｃ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "相过压故障", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x05:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "Ａ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "Ｂ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "Ｃ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "相欠压故障", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x06:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "Ａ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "Ｂ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "Ｃ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "相过流故障", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x07:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "Ａ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "Ｂ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "Ｃ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "相缺相故障", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x08:{StringDisplay( "错相故障", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x09:{StringDisplay( "电源中断", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x11:{StartAddr -= 6;StringDisplay( "漏电传感器丢失", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(84+12);}break;
			case 0x12:{
				StartAddr -= 6;
				StringDisplay( "漏电报警", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+6);
				DisplayValueN( fault->Fault_Type_Dsc,StartPage,StartAddr+1,0,3,ScreenBuffer );StartAddr+=18;
				StringDisplay("\x01", StartPage+1, StartAddr, 0, ScreenBuffer);//mA
			}break;
			case 0x13:{StartAddr -= 6;StringDisplay( "温度传感器丢失", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(84+12);}break;
			case 0x14:{
				StartAddr -= 6;
				StringDisplay( "温度报警", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+6);
				DisplayValueN( fault->Fault_Type_Dsc,StartPage,StartAddr,0,3,ScreenBuffer );StartAddr+=18;
				StringDisplay("℃", StartPage, StartAddr, 0, ScreenBuffer);//℃
			}break;
			}
		}break;
		case 0x0A:{//回路故障
			switch(fault->Fault_Type){
			case 0x01:{StringDisplay( "回路短路", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x02:{StringDisplay( "负载未接", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x03:{StringDisplay( "回路超载", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x04:{StringDisplay( "回路接地", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x05:{StringDisplay( "回路丢失", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			}
		}break;
		case 0x0C:{//电源故障
			switch(fault->Fault_Type){
			case 0x01:{StringDisplay( "主电故障", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x02:
			case 0x03:{StringDisplay( "备电故障", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;;
			case 0x04:{StringDisplay( "电源短路", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x05:{StringDisplay( "电源断线", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			}
		}break;
		case 0x0E: break;
		case 0x0F:{//系统
			switch(fault->Fault_Type)
			{
				case 0x01:{StringDisplay( "系统上电", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
				case 0x02:{StringDisplay( "回路请求", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
				default: ;
			}
		}break;
	}
}
