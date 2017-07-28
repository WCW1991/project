#include "Bujiantongji.h"

uint8_t Bujiantongji( void )
{
	uint8_t  err,Card_ID=0;
	uint8_t Detector_Cfg_ID=0;
	Key_enum *My_Key_Message;
	Detector_Cfg_struct Detector_Cfg;
	uint8_t RegisterNum[6];
	enum{
		sanxiangsixian=0x01,
		sanxiangsanxian=0x02,
		danxiangdianya=0x03,
		danxiangdianliu=0x04,
		sanxiandianya=0x05,
		sixiandianya=0x06,
		loudian=0x12,
		wendu=0x11,
		zuhe=0x13,
	};
	OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_CLR,&err);
	for(;;){
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "��·0��", 0, 0,0, ScreenBuffer );
		DisplayValueN(Card_ID+1, 0, 24, 0, 1, ScreenBuffer);
		if(Config.card_state&(0x01<<Card_ID)){
			StringDisplay( "�ѵǼ�", 0, 48,0, ScreenBuffer );
		}else{
			StringDisplay( "δ�Ǽ�", 0, 48,0, ScreenBuffer );
		}
		for(uint8_t i=0;i<6;i++)RegisterNum[i]=0;
		for(uint8_t Detector_Cfg_ID=0;Detector_Cfg_ID<200;Detector_Cfg_ID++){
			SpiFlashRead((uint8_t *)&Detector_Cfg,
						 (Card_ID*200+Detector_Cfg_ID)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
						 (uint16_t)sizeof(Detector_Cfg_struct));
			if(Detector_Cfg.State_type!=0xff&&Detector_Cfg.State_type&0x80){
#if (PROGRAM_TYPE == 1)
				StringDisplay( "��̽����������", 0, 84,0, ScreenBuffer );
				StringDisplay( "�����ѹ��", 2, 0,0, ScreenBuffer );
				StringDisplay( "���������", 4, 0,0, ScreenBuffer );
				StringDisplay( "�����ѹ��", 6, 0,0, ScreenBuffer );
				StringDisplay( "�����ѹ������", 2, 84,0, ScreenBuffer );
				StringDisplay( "����δ֪������", 4, 84,0, ScreenBuffer );
				RegisterNum[0]++;
				switch(Detector_Cfg.State_type&0x7f){
					case sanxiangsanxian:
					case sanxiangsixian:
						RegisterNum[1]++;
						break;
					case sanxiandianya:
					case sixiandianya:
						RegisterNum[2]++;
						break;
					case danxiangdianya:
						RegisterNum[3]++;
						break;
					case danxiangdianliu:
						RegisterNum[4]++;
						break;
					default: RegisterNum[5]++;
				}
				/*����*/
				DisplayValueN( RegisterNum[0], 0, 168, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[1], 2, 168, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[2], 2, 60, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[3], 6, 60, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[4], 4, 60, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[5], 4, 168, 0, 3, ScreenBuffer);
#endif
#if (PROGRAM_TYPE == 2)
				StringDisplay( "̽����������", 0, 96,0, ScreenBuffer );
				StringDisplay( "��©���磺", 2, 0,0, ScreenBuffer );
				StringDisplay( "���¡��ȣ�", 2, 96,0, ScreenBuffer );
				StringDisplay( "�����ʽ��", 4, 0,0, ScreenBuffer );
				StringDisplay( "��δ��֪��", 4, 96,0, ScreenBuffer );
				RegisterNum[0]++;
				switch(Detector_Cfg.State_type&0x7f){
					case loudian:{
						RegisterNum[1]++;
					}break;
					case wendu:{
						RegisterNum[2]++;
					}break;
					case zuhe:{
						RegisterNum[3]++;
					}break;
					default:{
						RegisterNum[5]++;
					}break;
				}
				/*����*/
				DisplayValueN( RegisterNum[0], 0, 168, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[1], 2, 60, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[2], 2, 160, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[3], 4, 60, 0, 3, ScreenBuffer);
				DisplayValueN( RegisterNum[5], 4, 160, 0, 3, ScreenBuffer);
#endif
			}
		}
		if(RegisterNum[0] == 0) StringDisplay( "��̽����", 2, 24,0, ScreenBuffer );
		
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//�ȴ�����
		if( err==OS_ERR_NONE ){
			switch(*My_Key_Message){
#if MAIN_BOARD==2
			case Key_Up:{
				if(Card_ID>0)Card_ID--;
			}break;
			case Key_Down:{
				if(Card_ID<3)Card_ID++;
			}break;
#endif
#if MAIN_BOARD==1
			case Key_2:{
				if(Card_ID>0)Card_ID--;
			}break;
			case Key_8:{

				if(Card_ID<3)Card_ID++;
			}break;
#endif
			case Key_Set:{
				uint8_t page = 0;
				while(1)
				{
					Clean_Lcd(ScreenBuffer);
					uint8_t StartPage=0,StartAddr=0;
					uint8_t tmp = 0;
					for(Detector_Cfg_ID=0;Detector_Cfg_ID<200;Detector_Cfg_ID++){
						SpiFlashRead((uint8_t *)&Detector_Cfg,(Card_ID*200+Detector_Cfg_ID)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,(uint16_t)sizeof(Detector_Cfg_struct));
						if(Detector_Cfg.State_type!=0xff&&Detector_Cfg.State_type&0x80)
						{
							DisplayValueN( Detector_Cfg_ID+1, StartPage, StartAddr,0, 3, ScreenBuffer);
							StartAddr+=21;//��ʾһ��̽������ַ+21
							if(StartAddr+18>=192){//һ����ʾ��
								StartPage+=2;//����һ��
								if(StartPage>=8)//����
								{
									if(tmp >= page) break;
									tmp++;//ҳ+1
									Clean_Lcd(ScreenBuffer);
									StartPage=0;
								}
								StartAddr=0;
							}
						}
					}
					page = tmp;
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);
					if( err==OS_ERR_NONE )
					{
						switch(*My_Key_Message)
						{
#if MAIN_BOARD==1
							case Key_2:
								if(page != 0) page--;
								break;
							case Key_8:
								page++;
								break;
#endif
#if MAIN_BOARD==2
							case Key_Upage:
								if(page != 0) page--;
								break;
							case Key_Dpage:
								page++;
								break;
#endif
							case Key_Set:
							case Key_Return: goto end_check;
						}
					}
					else return 1;
				}
		end_check: ;
			}break;
			case Key_Return:{return 0;}break;
			}
		}else return 1;
	}
}
void F3(void)
{
	uint8_t  err;
	Key_enum *My_Key_Message;
	Detector_Cfg_struct Detector_Cfg;
	int NUM=0;
	while(1){
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "��·��Ϣ", 0, 0, 0, ScreenBuffer );
		StringDisplay( "��·��", 2, 24, 0, ScreenBuffer );
		StringDisplay( "��·��", 2, 112, 0, ScreenBuffer );
		StringDisplay( "��·��", 4, 24, 0, ScreenBuffer );
		StringDisplay( "��·��", 4, 112, 0, ScreenBuffer );
		for(int Card_ID=0;Card_ID<CARD_NUM_MAX;Card_ID++){
			if(Config.card_state&(0x01<<Card_ID)){
				for(uint8_t Detector_Cfg_ID=0;Detector_Cfg_ID<200;Detector_Cfg_ID++){
					SpiFlashRead((uint8_t *)&Detector_Cfg,
								 (Card_ID*200+Detector_Cfg_ID)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
								 (uint16_t)sizeof(Detector_Cfg_struct));
					if(Detector_Cfg.State_type!=0xff&&Detector_Cfg.State_type&0x80)
						NUM++;
				}
				if(Card_ID<2)
					if(Card_ID==0)
						DisplayValueN( NUM, 2, 24+36, 0, 3, ScreenBuffer );
					else
						DisplayValueN( NUM, 2, 112+36, 0, 3, ScreenBuffer );
				else
					if(Card_ID==2)
						DisplayValueN( NUM, 4, 24+36, 0, 3, ScreenBuffer );
					else
						DisplayValueN( NUM, 4, 112+36, 0, 3, ScreenBuffer );
			}else{
				if(Card_ID<2)
					if(Card_ID==0)
						StringDisplay( "δ�Ǽ�", 2, 24+36,0, ScreenBuffer );
					else
						StringDisplay( "δ�Ǽ�", 2, 112+36,0, ScreenBuffer );
				else
					if(Card_ID==2)
						StringDisplay( "δ�Ǽ�", 4, 24+36,0, ScreenBuffer );
					else
						StringDisplay( "δ�Ǽ�", 4, 112+36,0, ScreenBuffer );
				
				continue;
			}
			NUM=0;
		}
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//�ȴ�����
		if( err==OS_ERR_NONE ){
			if(*My_Key_Message==Key_Return)break;
		}
		else break;
	}
}