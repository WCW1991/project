#include "PrinterSet.h"

uint8_t PrinterSet(void)
{
	uint8_t  err;
	Key_enum *My_Key_Message;

	Clean_Lcd(ScreenBuffer);
	for(;;){
		StringDisplay( "��ӡ��״̬��", 0, 0, 0, ScreenBuffer );
		StringDisplay( "��ӡ���أ�", 2, 0, 0, ScreenBuffer );
		StringDisplay( "�����л�", 6, 0, 0, ScreenBuffer );
#if MAIN_BOARD==1
		StringDisplay( "�����ü�����", 6, 72, 0, ScreenBuffer );
#endif
#if MAIN_BOARD==2
		StringDisplay( "��ȷ�ϼ�����", 6, 72, 0, ScreenBuffer );
#endif
		if(SET==GetPrinterState( NoPaper ))
			StringDisplay( "ȱֽ", 0, 72, 0, ScreenBuffer );
		else
			StringDisplay( "����", 0, 72, 0, ScreenBuffer );
	
		if(Config.print_state){
			StringDisplay( "��", 2, 60, 0, ScreenBuffer );
		}else{
			StringDisplay( "��", 2, 60, 0, ScreenBuffer );
		}
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,5000,&err);//�ȴ�����
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
	Position_Typedef Position={"���123"};
	/*��ӡʱ��*/
	printf("20%.2d-%.2d-%.2d %.2d:%.2d:%.2d\n",Record.time.year,Record.time.month,Record.time.day,Record.time.hour,Record.time.minute,Record.time.second);
	/*̽������������*/
	if(Record.event.card_code!=0){
		SpiFlashRead((uint8_t *)&Position,Record.event.card_code*SPI_FLASH_SECTOR_SIZE+(Record.event.address)*sizeof(Position_Typedef)+DescriptionRecord_StartAddr,(uint16_t)sizeof(Position_Typedef));//������������
		if(Position[0]==0xFF){printf("λ�ã���\n");
		}else{printf("λ�ã�%s\n",Position);}
	}else{
		printf("λ�ã�����\n");
	}
	/*��ӡλ��*/
	if(Record.event.card_code == 0)
	{
		printf("��ַ������\n");
	}
	else if(Record.event.address == 0)
	{
		printf("��ַ��%.2d��·\n",Record.event.card_code);
	}
	else if(Record.event.channel == 0)
	{
		printf("��ַ��%.2d��· %.2d��\n",Record.event.card_code,Record.event.address);
	}
	else
	{
		printf("��ַ��%.2d��· %.2d�� %.2dͨ��\n",Record.event.card_code,Record.event.address,Record.event.channel);
	}
	/*��ӡ��������*/
	switch(Record.event.event_type){
	case 0x08:{//̽��������
		switch(Record.event.fault_type){
		case 0x01:{printf("���ͣ�̽������ʧ\n");}break;
		case 0x02:{printf("���ͣ�ά��\n");}break;
		case 0x03:{printf("���ͣ�����\n");}break;
		case 0x04:{
			switch(Record.event.description){
			case 1:{printf("���ͣ������ѹ\n");}break;
			case 2:{printf("���ͣ������ѹ\n");}break;
			case 4:{printf("���ͣ������ѹ\n");}break;
			}
		}break;
		case 0x05:{
			switch(Record.event.description){
			case 1:{printf("���ͣ�����Ƿѹ\n");}break;
			case 2:{printf("���ͣ�����Ƿѹ\n");}break;
			case 4:{printf("���ͣ�����Ƿѹ\n");}break;
			}
		}break;
		case 0x06:{
			switch(Record.event.description){
			case 1:{printf("���ͣ��������\n");}break;
			case 2:{printf("���ͣ��������\n");}break;
			case 4:{printf("���ͣ��������\n");}break;
			}
			}break;
		case 0x07:{
			switch(Record.event.description){
			case 1:{printf("���ͣ�����ȱ��\n");}break;
			case 2:{printf("���ͣ�����ȱ��\n");}break;
			case 4:{printf("���ͣ�����ȱ��\n");}break;
			}
			}break;
		case 0x08:{printf("���ͣ��������\n");}break;
		case 0x09:{printf("���ͣ���Դ�ж�\n");}break;
		case 0x11:{printf("���ͣ�©�紫������ʧ\n");}break;
		case 0x12:{printf("���ͣ�©�籨��\n");}break;
		case 0x13:{printf("���ͣ��¶ȴ�������ʧ\n");}break;
		case 0x14:{printf("���ͣ��¶ȱ���\n");}break;
		}
	}break;
	case 0x0A:{//��·����
		switch(Record.event.fault_type){
		case 0x01:{printf("���ͣ���·��·\n");}break;
		case 0x02:{printf("���ͣ�����δ��\n");}break;
		case 0x03:{printf("���ͣ���·����\n");}break;
		case 0x04:{printf("���ͣ���·�ӵ�\n");}break;
		case 0x05:{printf("���ͣ���·��ʧ\n");}break;
		}
	}break;
	case 0x0C:{//��Դ����
		switch(Record.event.fault_type){
		case 0x01:{printf("���ͣ��������\n");}break;
		case 0x02:
		case 0x03:{printf("���ͣ��������\n");}break;
		case 0x04:{printf("���ͣ���Դ��·\n");}break;
		case 0x05:{printf("���ͣ���Դ����\n");}break;
		}
	}break;
	case 0x0E:{
	}break;
	case 0x0F:{//ϵͳ
		switch(Record.event.fault_type)
		{
			case 0x01:{printf("���ͣ�ϵͳ�ϵ�\n");}break;
		}
	}break;
	}
	Move_Paper( 35 );
}
