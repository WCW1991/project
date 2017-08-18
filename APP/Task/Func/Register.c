#include "Register.h"
#include "pyinput.h"
static uint16_t AutoRegisterN=0;

static ErrorStatus InformCard( uint8_t Card_ID);
static ErrorStatus GetCard_Inf( uint8_t Card_ID );
static void RefreshDisplay(uint8_t TitleCursor,uint8_t Detector_Cfg_ID,uint8_t Type,uint8_t Channel,uint8_t DY,uint8_t *Description);



void AutoRegister(void)
{
	uint8_t err;
	uint8_t Card_ID;
	ErrorStatus Inform[8]={ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR};
	uint8_t LoseNumber=0;
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "�Զ��Ǽǣ�����", 2, 48, 1, ScreenBuffer );
	AutoRegisterN=0;
	
	OSTaskSuspend(APP_CFG_TASK_START_PRIO);
	OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_SET,&err);//ֹͣѲ��
	
	OSTimeDlyHMSM(0,0,1,0);
	
	Config.card_state=0;/*��*/
	config_store();
	
	SpiFlashEraseSector(DeviceRecord_StartAddr/0x1000);
	
	for(Card_ID=1;Card_ID<=CARD_NUM_MAX;Card_ID++){
		Inform[Card_ID-1]=InformCard( Card_ID);
	}
	OSTimeDlyHMSM(0,0,5,0);//�ȴ���·���Զ��Ǽ���
	for(Card_ID=1;Card_ID<=CARD_NUM_MAX;Card_ID++){
		if(SUCCESS==Inform[Card_ID-1]){
			LoseNumber=0;
			while(LoseNumber<10){
				if(ERROR==GetCard_Inf( Card_ID )){
					LoseNumber++;
					OSTimeDlyHMSM(0,0,3,0);//�ȴ���·���Զ��Ǽ���
				}else break;
			}
		}
	}
	Buzzer_ON();
	OSTimeDlyHMSM(0,0,0,100);
	Buzzer_OFF();
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "�Ǽ�������", 2, 48, 0, ScreenBuffer );
	DisplayValueN( AutoRegisterN, 2, 108,0, 3, ScreenBuffer);
	OSTimeDlyHMSM(0,0,3,0);
	OSTaskResume(APP_CFG_TASK_START_PRIO);
	HAL_NVIC_SystemReset();
}

static ErrorStatus InformCard( uint8_t Card_ID )
{
	if(SUCCESS != auto_info_card(Card_ID)) return ERROR;
	
	Config.card_state |= (0x01<<(Card_ID-1));/*��*/
	config_store();
	
	return SUCCESS;
}

static ErrorStatus GetCard_Inf( uint8_t Card_ID )
{
	Detector_Cfg_struct Detector_Cfg;
	uint8_t DetectorN;
	uint16_t Index=1;
	uint8_t *data;
	
	if(SUCCESS != get_info_card(Card_ID, &data)) return ERROR;
	
	AutoRegisterN+=data[0];
	for(DetectorN=0;DetectorN<data[0];DetectorN++,Index+=3)
	{
		Detector_Cfg.card_code=(Card_ID-1);
		Detector_Cfg.address=data[Index];
		Detector_Cfg.State_type=data[Index+1]|0x80u;
		Detector_Cfg.channel_num=data[Index+2];
		SpiFlashWrite((uint8_t*)&Detector_Cfg,
							 ((Card_ID-1)*200+(Detector_Cfg.address-1))*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
							 sizeof(Detector_Cfg_struct));
	}
	
	return SUCCESS;
}
#if (PROGRAM_TYPE == 1)
#define TYPE_NUMBER 4
#else
#define TYPE_NUMBER 3
#endif
static void RefreshDisplay(uint8_t TitleCursor,uint8_t Detector_Cfg_ID,uint8_t Type,uint8_t Channel,uint8_t DY,uint8_t *Description)
{
	uint8_t StartAddr=0,i=0;
	
	uint8_t *TitleStr[]={"��ַ","����","ͨ��","����","������Ϣ��"};
	struct{uint8_t *String;uint8_t Value;
	}TypeList[]={
#if (PROGRAM_TYPE == 1)
		{.String="����ɣ�",.Value=0x01},
		{.String="���ࡡ��",.Value=0x03},
		{.String="����ɣ�",.Value=0x04},
		{.String="���ࡡ��",.Value=0x05},
		
#endif
#if (PROGRAM_TYPE == 2)
		{.String="���¶ȡ�",.Value=0x11},
		{.String="��©����",.Value=0x12},
		{.String="�����ʽ",.Value=0x13},
		
#endif
	};
	
	Clean_Lcd(ScreenBuffer);
//#if MAIN_BOARD==1
//	StringDisplay( "�����л������ü�ȷ��", 4, 72, 0, ScreenBuffer );
//#endif
//#if MAIN_BOARD==2
//	StringDisplay( "��ȷ�ϼ�����", 4, 84, 0, ScreenBuffer );
//#endif
	
	//��ʾ����
	for(i=0;i<4;i++){
		uint8_t Reverse=0;
		if(i==TitleCursor)
			Reverse=1;
		else
			Reverse=0;
		StringDisplay( TitleStr[i], 0, StartAddr, Reverse, ScreenBuffer );
		StartAddr+=48;
	}
	if(TitleCursor==4)
		StringDisplay( TitleStr[i], 4, 0, 1, ScreenBuffer );
	else
		StringDisplay( TitleStr[i], 4, 0, 0, ScreenBuffer );
	
	DisplayValueN( Detector_Cfg_ID, 2, 3, 0, 3, ScreenBuffer);//��ַ
	
	if(0xFF==Type){//����
		StringDisplay( "��δ֪��", 2, 36, 0, ScreenBuffer );
	}else{
		for(i=0;i<TYPE_NUMBER;i++){
			if((Type&0x7f)==(TypeList[i].Value&0x7f)){
				StringDisplay( TypeList[i].String, 2, 36, 0, ScreenBuffer );
				break;
			}
		}
		if(i==TYPE_NUMBER){
			StringDisplay( "��δ֪��", 2, 36, 0, ScreenBuffer );
		}
	}
	DisplayValueN( Channel, 2, 96+3, 0, 3, ScreenBuffer);	//ͨ��
	
	if(DY==0xff)//����
		StringDisplay( "��", 2, 150, 0, ScreenBuffer );
	else
		StringDisplay( "��", 2, 150, 0, ScreenBuffer );
	
	if(*Description==0xff)
		StringDisplay( "��������Ϣ", 6, 24, 0, ScreenBuffer );
	else
		StringDisplay( Description, 6, 24, 0, ScreenBuffer );
}

uint8_t regist_flag;
#if MAIN_BOARD==2
uint8_t Manual_Register( uint8_t Card_ID )
{
	char Description[17]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};//�ɴ洢8������+0
	uint8_t i, err;
	uint8_t TitleCursor=0;
	uint8_t Type[10]={0x01,0x02,0x03,0x04,0x05,0x06,0x11,0x12,0x13,0xff};
	uint8_t TypeIndex=0;
	uint8_t DY=0xff;
	Detector_Cfg_struct Detector_Cfg;
	Key_enum key=Key_None;
	uint8_t Detector_Cfg_ID = 1, Detector_Cfg_ID_Prev = 1;
	SpiFlashRead((uint8_t *)&Detector_Cfg,
				 (Card_ID*200+Detector_Cfg_ID-1)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
				 (uint16_t)sizeof(Detector_Cfg_struct));//����ǰ��¼
	SpiFlashRead((uint8_t *)Description,
				 Card_ID*SPI_FLASH_SECTOR_SIZE+(Detector_Cfg_ID-1)*16+DescriptionRecord_StartAddr,
				 (uint16_t)16);//��������
	for(i=0;i<17;i++){//����������Ϣ��ȷ�Լ��
		if(Description[i]==0xff){
			for(i=0;i<17;i++)
				Description[i]=0;
			continue;
		}
	}
	if(Detector_Cfg.State_type!=0xff){
		for(i=0;i<9;i++){//�ҳ�����
			if((Detector_Cfg.State_type&0x7f)==Type[i]){
				TypeIndex=i;
				break;
			}
		}
		if(i==9)
			TypeIndex=9;
		if(Detector_Cfg.State_type&0x80)DY=0x01;
		else DY=0xff;
	}else {DY=0xff;TypeIndex=9;}
	for(;;){
		RefreshDisplay(TitleCursor,Detector_Cfg_ID,Type[TypeIndex],Detector_Cfg.channel_num,DY,(uint8_t*)Description);
		
		switch(TitleCursor)
		{
			case 0:{//��ַ
				key = input_value8( &Detector_Cfg_ID, 3, 2,3 );
				if(key == Key_Down){
					if(Detector_Cfg_ID<200)Detector_Cfg_ID++;
				}else
					if(key == Key_Up){
						if(Detector_Cfg_ID>1)Detector_Cfg_ID--;
					}

				if(Detector_Cfg_ID != Detector_Cfg_ID_Prev){
					Detector_Cfg_ID_Prev = Detector_Cfg_ID;
					SpiFlashRead((uint8_t *)&Detector_Cfg,
								 (Card_ID*200+Detector_Cfg_ID-1)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
								 (uint16_t)sizeof(Detector_Cfg_struct));//����ǰ��¼
					SpiFlashRead((uint8_t *)Description,
								 Card_ID*SPI_FLASH_SECTOR_SIZE+(Detector_Cfg_ID-1)*16+DescriptionRecord_StartAddr,
								 (uint16_t)16);//��������
					for(i=0;i<17;i++){//����������Ϣ��ȷ�Լ��
						if(Description[i]==0xff){
							for(i=0;i<17;i++)
								Description[i]=0;
							continue;
						}
					}
					if(Detector_Cfg.State_type!=0xff){
						for(i=0;i<9;i++){
							if((Detector_Cfg.State_type&0x7f)==Type[i]){
								TypeIndex=i;
								break;
							}
						}
						if(i==9)
							TypeIndex=9;
						if(Detector_Cfg.State_type&0x80)
							DY=0x01;
						else
							DY=0xff;
					}else {DY=0xff;TypeIndex=9;}
				}
				goto MyKey;
			}break;
			case 1:{//����
				key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
				if(key == Key_Down){
#if (PROGRAM_TYPE == 1)
					if(TypeIndex<TYPE_NUMBER)
						TypeIndex++;
					else
						TypeIndex=0;
#endif
#if (PROGRAM_TYPE == 2)
					if(TypeIndex<TYPE_NUMBER)
						TypeIndex=TYPE_NUMBER;
					if(TypeIndex<TYPE_NUMBER)
						TypeIndex++;
					else
						TypeIndex=TYPE_NUMBER;
#endif
					Detector_Cfg.State_type=(Detector_Cfg.State_type&0x80)|(Type[TypeIndex]&0x7f);
					continue;
				}else{
					if(key == Key_Up){
#if (PROGRAM_TYPE == 1)
						if(TypeIndex>0){
							TypeIndex--;
#endif
#if (PROGRAM_TYPE == 2)
						if(TypeIndex<TYPE_NUMBER)
							TypeIndex=TYPE_NUMBER;
						if(TypeIndex>TYPE_NUMBER){
							TypeIndex--;
#endif
							Detector_Cfg.State_type=(Detector_Cfg.State_type&0x80)|(Type[TypeIndex]&0x7f);
						}
						continue;
					}
				}
				goto MyKey;
			}break;
			case 2://ͨ��
				key = input_value8( &(Detector_Cfg.channel_num), 3, 2, 96+3 );
				goto MyKey;
				break;
			case 3:{//����
				key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
				if(key == Key_Down){
					if(DY==0xff){
						if(Detector_Cfg.State_type==0xff)Detector_Cfg.State_type=0x80;
						else Detector_Cfg.State_type|=0x80;
						DY=1;
					}else{
						DY=0xff;
						Detector_Cfg.State_type&=0x7f;
					}
					continue;
				}else goto MyKey;
			}break;
			case 4:{//������Ϣ
				StringDisplay( "��#����������Ϣ", 4, 101, 0, ScreenBuffer );
				key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
				if(key == Key_j){
					PinYin_Input(Description);
					continue;
				}else goto MyKey;
			}break;
			default:break;
		}
		key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
	MyKey:
		switch(key)
		{
			case Key_Lift:
				if(TitleCursor>0)
					TitleCursor--;
				break;
			case Key_Right:
				if(TitleCursor<4)
					TitleCursor++;
				break;
			case Key_Set:{
				regist_flag = 1;
				/*�洢̽�����Ǽ�״̬*/
				SpiFlashWrite((uint8_t*)&Detector_Cfg,
							  (Card_ID*200+Detector_Cfg_ID-1)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
							  (uint16_t)sizeof(Detector_Cfg_struct));
				SpiFlashWrite((uint8_t*)&Description,
							  Card_ID*SPI_FLASH_SECTOR_SIZE+(Detector_Cfg_ID-1)*16+DescriptionRecord_StartAddr,
							  16);
				Clean_Lcd(ScreenBuffer);
				StringDisplay( "����Ǽ����", 3, 0, 0, ScreenBuffer );
				OSTimeDlyHMSM(0,0,0,500);
				OSMboxPend(Key_Mbox_Handle,10,&err);	//������ʱ�ڼ䰴��
			}break;
			case Key_Return: return 0;
			case Key_None: return 1;
		}
	}
}
#else
uint8_t Manual_Register( uint8_t Card_ID )
{
	char Description[17]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};//�ɴ洢8������+0
	uint8_t i, err;
	uint8_t TitleCursor=0;
	uint8_t Type[9]={0x01,0x02,0x03,0x04,0x05,0x06,0x11,0x12,0x13};
	uint8_t TypeIndex=0;
	uint8_t DY=0xff;
	Detector_Cfg_struct Detector_Cfg;
	Key_enum key=Key_None;
	uint8_t Detector_Cfg_ID = 1, Detector_Cfg_ID_Prev = 1;
	SpiFlashRead((uint8_t *)&Detector_Cfg,
				 (Card_ID*200+Detector_Cfg_ID-1)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
				 (uint16_t)sizeof(Detector_Cfg_struct));//����ǰ��¼
	SpiFlashRead((uint8_t *)Description,
				 Card_ID*SPI_FLASH_SECTOR_SIZE+(Detector_Cfg_ID-1)*16+DescriptionRecord_StartAddr,
				 (uint16_t)16);//��������
	for(i=0;i<17;i++){//����������Ϣ��ȷ�Լ��
		if(Description[i]==0xff){
			for(i=0;i<17;i++)
				Description[i]=0;
			continue;
		}
	}
	if(Detector_Cfg.State_type!=0xff){
		for(i=0;i<9;i++){//�ҳ�����
			if((Detector_Cfg.State_type&0x7f)==Type[i]){
				TypeIndex=i;
				break;
			}
		}
		if(i==9)
			TypeIndex=9;
		if(Detector_Cfg.State_type&0x80)DY=0x01;
		else DY=0xff;
	}else {DY=0xff;TypeIndex=9;}
	if(Detector_Cfg.State_type!=0xff){
		for(i=0;i<9;i++){
			if(Detector_Cfg.State_type&0x7f==Type[i]){
				TypeIndex=i;
				break;
			}
		}
		DY=0xff;
	}else DY=0xff;
	for(;;){
		RefreshDisplay(TitleCursor,Detector_Cfg_ID,Type[TypeIndex],Detector_Cfg.channel_num,DY,(uint8_t*)Description);
		
		switch(TitleCursor)
		{
			case 0:{//��ַ
				key = input_value8( &Detector_Cfg_ID, 3, 2,3 );
				if(Detector_Cfg_ID != Detector_Cfg_ID_Prev){
					Detector_Cfg_ID_Prev = Detector_Cfg_ID;
					SpiFlashRead((uint8_t *)&Detector_Cfg,
								 (Card_ID*200+Detector_Cfg_ID-1)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
								 (uint16_t)sizeof(Detector_Cfg_struct));//����ǰ��¼
					SpiFlashRead((uint8_t *)Description,
								 Card_ID*SPI_FLASH_SECTOR_SIZE+(Detector_Cfg_ID-1)*16+DescriptionRecord_StartAddr,
								 (uint16_t)16);//��������
					for(i=0;i<17;i++){//����������Ϣ��ȷ�Լ��
						if(Description[i]==0xff){
							for(i=0;i<17;i++)
								Description[i]=0;
							continue;
						}
					}
					if(Detector_Cfg.State_type!=0xff){
						for(i=0;i<9;i++){
							if((Detector_Cfg.State_type&0x7f)==Type[i]){
								TypeIndex=i;
								break;
							}
						}
						if(i==9)
							TypeIndex=9;
						if(Detector_Cfg.State_type&0x80)
							DY=0x01;
						else
							DY=0xff;
					}else {DY=0xff;TypeIndex=9;}
				}
				goto MyKey;
			}break;
			case 1:{//����
				key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
				if(key == Key_8){
#if (PROGRAM_TYPE == 1)
					if(TypeIndex<6)
						TypeIndex++;
					else
						TypeIndex=0;
#endif
#if (PROGRAM_TYPE == 2)
					if(TypeIndex<6)
						TypeIndex=6;
					if(TypeIndex<8)
						TypeIndex++;
					else
						TypeIndex=6;
#endif
					Detector_Cfg.State_type=(Detector_Cfg.State_type&0x80)|(Type[TypeIndex]&0x7f);
					continue;
				}else{
					if(key == Key_2){
#if (PROGRAM_TYPE == 1)
						if(TypeIndex>0){
							TypeIndex--;
#endif
#if (PROGRAM_TYPE == 2)
						if(TypeIndex<6)
							TypeIndex=6;
						if(TypeIndex>6){
							TypeIndex--;
#endif
							Detector_Cfg.State_type=(Detector_Cfg.State_type&0x80)|(Type[TypeIndex]&0x7f);
						}
						continue;
					}
				}
				goto MyKey;
			}break;
			case 2://ͨ��
				key = input_value8( &(Detector_Cfg.channel_num), 3, 2, 96+3 );
				goto MyKey;
				break;
			case 3:{//����
				key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
				if(key == Key_8){
					if(DY==0xff){
						if(Detector_Cfg.State_type==0xff)Detector_Cfg.State_type=0x80;
						else Detector_Cfg.State_type|=0x80;
						DY=1;
					}else{
						DY=0xff;
						Detector_Cfg.State_type&=0x7f;
					}
					continue;
				}else goto MyKey;
			}break;
			case 4:{//������Ϣ
				StringDisplay( "��0����������Ϣ", 4, 101, 0, ScreenBuffer );
				key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
				if(key == Key_0){
					QuWeiInput(Description);
					continue;
				}else goto MyKey;
			}break;
			default:break;
		}
		key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,0,&err));
	MyKey:
		switch(key)
		{
			case Key_x:{
				if(TitleCursor<4)
					TitleCursor++;
				else
					TitleCursor=0;
			}break;
			case Key_Set:{
				/*�洢̽�����Ǽ�״̬*/
				SpiFlashWrite((uint8_t*)&Detector_Cfg,
							  (Card_ID*200+Detector_Cfg_ID-1)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
							  (uint16_t)sizeof(Detector_Cfg_struct));
				SpiFlashWrite((uint8_t*)&Description,
							  Card_ID*SPI_FLASH_SECTOR_SIZE+(Detector_Cfg_ID-1)*16+DescriptionRecord_StartAddr,
							  16);
				Clean_Lcd(ScreenBuffer);
				StringDisplay( "����Ǽ����", 3, 0, 0, ScreenBuffer );
				OSTimeDlyHMSM(0,0,0,500);
				OSMboxPend(Key_Mbox_Handle,10,&err);	//������ʱ�ڼ䰴��
			}break;
			case Key_Return: return 0;
			case Key_None: return 1;
		}
	}
}

#endif