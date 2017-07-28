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
	/* ��� */
	DisplayValueN( index, StartPage, StartAddr-3, 0, 3, ScreenBuffer);StartAddr+=30;
	if(fault->CardAddress!=0){
		/* ��·�� */
		if(fault->CardAddress>99)fault->CardAddress=0;
		DisplayValueN(fault->CardAddress, StartPage, StartAddr, 0, 2, ScreenBuffer);StartAddr+=12;
		//Display_Number( 12, StartPage, StartAddr,0 );StartAddr+=6;// -
		if(fault->DetectorAddress!=0){
			StringDisplay("-", StartPage, StartAddr, 0, ScreenBuffer); StartAddr+=6;
			/* ��ַ */
			DisplayValueN( fault->DetectorAddress, StartPage, StartAddr, 0, 3, ScreenBuffer);StartAddr+=18;
			//Display_Number( 12, StartPage, StartAddr,0 );StartAddr+=6;// -
			/* ͨ���� */
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
			StringDisplay("��·��������", StartPage, StartAddr, 0, ScreenBuffer);StartAddr+=48+6;
		}
	}
	else
	{
		StringDisplay("������������", StartPage, StartAddr, 0, ScreenBuffer);StartAddr+=60+6;
	}
	/*������Ϣ*/
	//StringDisplay( Fault_Device.Detector_Dsc, StartPage+2, 36,0, ScreenBuffer );
	/* ���� */
	switch(fault->Event_Type){
		case 0x08:{//̽��������
			switch(fault->Fault_Type){
#if (PROGRAM_TYPE == 1)
			case 0x01:{StringDisplay( "��������ʧ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
#endif
#if (PROGRAM_TYPE == 2)
			case 0x01:{StringDisplay( "̽������ʧ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
#endif
			case 0x02:{StringDisplay( "ά������", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x03:{StringDisplay( "�ء�����", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x04:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "���ѹ����", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x05:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "��Ƿѹ����", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x06:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "���������", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x07:{
				switch(fault->Fault_Type_Dsc){
				case 1:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 2:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				case 4:{StringDisplay( "��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(12);}break;
				}
				StringDisplay( "��ȱ�����", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(60+12);}break;
			case 0x08:{StringDisplay( "�������", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x09:{StringDisplay( "��Դ�ж�", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x11:{StartAddr -= 6;StringDisplay( "©�紫������ʧ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(84+12);}break;
			case 0x12:{
				StartAddr -= 6;
				StringDisplay( "©�籨��", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+6);
				DisplayValueN( fault->Fault_Type_Dsc,StartPage,StartAddr+1,0,3,ScreenBuffer );StartAddr+=18;
				StringDisplay("\x01", StartPage+1, StartAddr, 0, ScreenBuffer);//mA
			}break;
			case 0x13:{StartAddr -= 6;StringDisplay( "�¶ȴ�������ʧ", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(84+12);}break;
			case 0x14:{
				StartAddr -= 6;
				StringDisplay( "�¶ȱ���", StartPage, StartAddr,0, ScreenBuffer );StartAddr+=(48+6);
				DisplayValueN( fault->Fault_Type_Dsc,StartPage,StartAddr,0,3,ScreenBuffer );StartAddr+=18;
				StringDisplay("��", StartPage, StartAddr, 0, ScreenBuffer);//��
			}break;
			}
		}break;
		case 0x0A:{//��·����
			switch(fault->Fault_Type){
			case 0x01:{StringDisplay( "��·��·", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x02:{StringDisplay( "����δ��", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x03:{StringDisplay( "��·����", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x04:{StringDisplay( "��·�ӵ�", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x05:{StringDisplay( "��·��ʧ", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			}
		}break;
		case 0x0C:{//��Դ����
			switch(fault->Fault_Type){
			case 0x01:{StringDisplay( "�������", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x02:
			case 0x03:{StringDisplay( "�������", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;;
			case 0x04:{StringDisplay( "��Դ��·", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			case 0x05:{StringDisplay( "��Դ����", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
			}
		}break;
		case 0x0E: break;
		case 0x0F:{//ϵͳ
			switch(fault->Fault_Type)
			{
				case 0x01:{StringDisplay( "ϵͳ�ϵ�", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
				case 0x02:{StringDisplay( "��·����", StartPage, StartAddr, 0, ScreenBuffer );StartAddr+=(48+12);}break;
				default: ;
			}
		}break;
	}
}
