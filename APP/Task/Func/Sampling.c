#include "Sampling.h"
void I(uint8_t *data, uint8_t tmp[]);
static void Get_Detector( uint8_t CardNum, uint8_t Addr );
void I(uint8_t *data, uint8_t tmp[])
{
	uint32_t current = 0;
	switch(data[0]>>6){
		case 0:{current = (((0x3F&data[0])<<8)|data[1])*1000;}break;
		case 1:{current = (((0x3F&data[0])<<8)|data[1])*100;}break;
		case 2:{current = (((0x3F&data[0])<<8)|data[1])*10;}break;
		case 3:{current = (((0x3F&data[0])<<8)|data[1]);}break;
	}
	if(current < 10000) sprintf((char *)tmp, "%01d.%03d", current/1000, current%1000);
	if(current >= 10000 && current < 100000) sprintf((char *)tmp, "%02d.%02d", current/1000, current%1000/10);
	if(current >= 100000 && current < 1000000) sprintf((char *)tmp, "%03d.%01d", current/1000, current%1000/100);
	if(current >= 1000000) sprintf((char *)tmp, "%04d", current/1000);
}
uint8_t Sampling( void )
{
	Key_enum key;
	uint8_t CardNum=1,Addr=1;
	enum{
		Card_Cursor=0,
		Addr_Cursor
	}Cursor=Card_Cursor;
	
	for(;;){
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "输入采样地址：", 0, 0, 1, ScreenBuffer );
#if MAIN_BOARD==1
		StringDisplay( "按＊切换", 2, 144, 0, ScreenBuffer );
		StringDisplay( "按设置键进入采样", 6, 0, 0, ScreenBuffer );
#endif
		StringDisplay( "回路：", 2, 24, 0, ScreenBuffer );
		StringDisplay( "地址：", 4, 24, 0, ScreenBuffer );
		
		
		DisplayValueN( CardNum, 2, 60, 0, 3, ScreenBuffer);
		DisplayValueN( Addr, 4, 60, 0, 3, ScreenBuffer);
		
		switch(Cursor){
			case Card_Cursor:key = input_value8(&CardNum, 3, 2, 60); break;
			case Addr_Cursor:key = input_value8(&Addr, 3, 4, 60); break;
		}
		switch(key)
		{
#if MAIN_BOARD==1
			case Key_x:
				if(Cursor==Addr_Cursor)Cursor=Card_Cursor;
				else Cursor=Addr_Cursor;
				break;
#endif
#if MAIN_BOARD==2
			case Key_Up:
				if(Cursor==Addr_Cursor)Cursor=Card_Cursor;break;
			case Key_Down:
				if(Cursor==Card_Cursor)Cursor=Addr_Cursor;break;
#endif
			case Key_Set:
				Get_Detector( CardNum,Addr );
				break;
			case Key_Return: return 0;
			case Key_None:  return 1;
		}
	}
}

static void Get_Detector( uint8_t Card_ID, uint8_t Detector_Cfg_ID )
{
	uint8_t  err;
	uint16_t SendTimes=0,ErrorTimes=0;
	Key_enum *My_Key_Message;
	//Send_Receive_struct Send,Receive;
	Detector_Cfg_struct Detector_Cfg;
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
	uint8_t StartAddr=0;
	uint8_t *data;
#if (PROGRAM_TYPE == 1)
	uint8_t tmp[8];
#endif
	
	if(!Card_ID||!Detector_Cfg_ID)return;
	Clean_Lcd(ScreenBuffer);
	OSFlagPost(FLAG_GRP,DisplayDateTimeFlag,OS_FLAG_CLR,&err);
	SpiFlashRead((uint8_t *)&Detector_Cfg,
				 ((Card_ID-1)*200+(Detector_Cfg_ID-1))*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
				 (uint16_t)sizeof(Detector_Cfg_struct));
	if(Detector_Cfg.State_type!=0xff&&Detector_Cfg.State_type&0x80){
		switch(Detector_Cfg.State_type&0x7f){
#if (PROGRAM_TYPE == 1)
			case sanxiangsixian:
			case sanxiangsanxian:
				for(;;){
					StartAddr=0;
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+9;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+15;
					StringDisplay( "主A:", 2, 0 , 0, ScreenBuffer );
					StringDisplay( "主B:", 4, 0 , 0, ScreenBuffer );
					StringDisplay( "主C:", 6, 0 , 0, ScreenBuffer );
					StringDisplay( "备A:", 2, 48, 0, ScreenBuffer );
					StringDisplay( "备B:", 4, 48, 0, ScreenBuffer );
					StringDisplay( "备C:", 6, 48, 0, ScreenBuffer );
					StringDisplay( "I1:", 2, 98, 0, ScreenBuffer );
					StringDisplay( "I2:", 4, 98, 0, ScreenBuffer );
					StringDisplay( "I3:", 6, 98, 0, ScreenBuffer );
					StringDisplay( "S1:", 2, 150, 0, ScreenBuffer );
					StringDisplay( "S2:", 4, 150, 0, ScreenBuffer );
					SendTimes++;
					if(SUCCESS == get_data_detector(Card_ID, Detector_Cfg_ID, &data))
					{
						if( data[0] == 0x01 ){
							switch(data[2]){
								case 0x00:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x01:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x02:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
								case 0x03:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
							}
							DisplayValueN( data[3] <<8|data[4] ,2, 24, 0, 3, ScreenBuffer);//主A
							DisplayValueN( data[5] <<8|data[6] ,4, 24, 0, 3, ScreenBuffer);//主B
							DisplayValueN( data[7] <<8|data[8] ,6, 24, 0, 3, ScreenBuffer);//主C
							if(Detector_Cfg.channel_num==2){
								DisplayValueN( data[9]<<8|data[10],2, 72,0, 3, ScreenBuffer);//备A
								DisplayValueN( data[11]<<8|data[12],4, 72,0, 3, ScreenBuffer);//备B
								DisplayValueN( data[13]<<8|data[14],6, 72,0, 3, ScreenBuffer);//备C
								I(&data[15], tmp);
								StringDisplay(tmp, 2, 115, 0, ScreenBuffer);//IA
								I(&data[17], tmp);
								StringDisplay(tmp, 4, 115, 0, ScreenBuffer);//IB
								I(&data[19], tmp);
								StringDisplay(tmp, 6, 115, 0, ScreenBuffer);//IC
							}else{
								StringDisplay("-", 2, 72, 0, ScreenBuffer);//备A
								StringDisplay("-", 4, 72, 0, ScreenBuffer);//备B
								StringDisplay("-", 6, 72, 0, ScreenBuffer);//备C
								I(&data[9], tmp);
								StringDisplay(tmp, 2, 115, 0, ScreenBuffer);//IA
								I(&data[11], tmp);
								StringDisplay(tmp, 4, 115, 0, ScreenBuffer);//IB
								I(&data[13], tmp);
								StringDisplay(tmp, 6, 115, 0, ScreenBuffer);//IC
							}
						}else{
							ErrorTimes++;
						}
					}else{
						ErrorTimes++;
					}
					if(SendTimes >= 10000) SendTimes = 0;
					if(ErrorTimes >= 10000) ErrorTimes = 0;
//					DisplayValue(SendTimes,6,168,0, 4, ScreenBuffer);
//					DisplayValue(ErrorTimes,7,168,1, 4, ScreenBuffer);
					
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
				break;
			case danxiangdianya:
				while(1)
				{
					StartAddr=0;
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+9;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+15;
					StringDisplay( "S1:", 2, 150, 0, ScreenBuffer );
					StringDisplay( "S2:", 4, 150, 0, ScreenBuffer );
					StringDisplay( "V1:", 2, 0 , 0, ScreenBuffer );
					if(Detector_Cfg.channel_num==2||Detector_Cfg.channel_num==4){
						StringDisplay( "V2:", 2, 48 , 0, ScreenBuffer );
						if(Detector_Cfg.channel_num==4){
							StringDisplay( "V3:", 4, 0 , 0, ScreenBuffer );
							StringDisplay( "V4:", 4, 48, 0, ScreenBuffer );
						}
					}
					SendTimes++;
					if(SUCCESS == get_data_detector(Card_ID, Detector_Cfg_ID, &data))
					{
						if( data[0] == 0x01 ){
							switch(data[2]){
								case 0x00:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x01:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x02:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
								case 0x03:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
							}
							DisplayValueN( data[3] <<8|data[4] ,2, 24, 0, 3, ScreenBuffer);//V1
							if(Detector_Cfg.channel_num==2||Detector_Cfg.channel_num==4){
								DisplayValueN( data[5] <<8|data[6] ,2, 72, 0, 3, ScreenBuffer);//V2
								if(Detector_Cfg.channel_num==4){
									DisplayValueN( data[7] <<8|data[8] ,4, 24, 0, 3, ScreenBuffer);//V3
									DisplayValueN( data[9] <<8|data[10] ,4, 72, 0, 3, ScreenBuffer);//V4
								}
							}
						}else{
							ErrorTimes++;
						}
					}else{
						ErrorTimes++;
					}
					if(SendTimes >= 10000) SendTimes = 0;
					if(ErrorTimes >= 10000) ErrorTimes = 0;
//					DisplayValue(SendTimes,6,168,0, 4, ScreenBuffer);
//					DisplayValue(ErrorTimes,7,168,1, 4, ScreenBuffer);
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
			case danxiangdianliu:
				while(1)
				{
					StartAddr=0;
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+9;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+15;
					StringDisplay( "S1:", 2, 150, 0, ScreenBuffer );
					StringDisplay( "S2:", 4, 150, 0, ScreenBuffer );
					StringDisplay( "V1:", 2, 0 , 0, ScreenBuffer );
					StringDisplay( "I1:", 2, 48, 0, ScreenBuffer );
					if(Detector_Cfg.channel_num==2){
						StringDisplay( "V2:", 4, 0 , 0, ScreenBuffer );
						StringDisplay( "I2:", 4, 48, 0, ScreenBuffer );
					}
					SendTimes++;
					if(SUCCESS == get_data_detector(Card_ID, Detector_Cfg_ID, &data))
					{
						if( data[0] == 0x01 ){
							switch(data[2]){
								case 0x00:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x01:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x02:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
								case 0x03:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
							}
							DisplayValueN( data[3] <<8|data[4] ,2, 24, 0, 3, ScreenBuffer);//V1
							I(&data[5], tmp);
							StringDisplay(tmp, 2, 72, 0, ScreenBuffer);//I1
							if(Detector_Cfg.channel_num==2){
								DisplayValueN( data[7] <<8|data[8] ,4, 24, 0, 3, ScreenBuffer);//V2
								I(&data[9], tmp);
								StringDisplay(tmp, 4, 72, 0, ScreenBuffer);//I2
							}
						}else{
							ErrorTimes++;
						}
					}else{
						ErrorTimes++;
					}
					if(SendTimes >= 10000) SendTimes = 0;
					if(ErrorTimes >= 10000) ErrorTimes = 0;
//					DisplayValue(SendTimes,6,168,0, 4, ScreenBuffer);
//					DisplayValue(ErrorTimes,7,168,1, 4, ScreenBuffer);
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
				break;
			case sixiandianya:
			case sanxiandianya:
				for(;;){
					StartAddr=0;
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+3;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+15;
					StringDisplay( "主A:", 2, 0 , 0, ScreenBuffer );
					StringDisplay( "主B:", 4, 0 , 0, ScreenBuffer );
					StringDisplay( "主C:", 6, 0 , 0, ScreenBuffer );
					StringDisplay( "备A:", 2, 48, 0, ScreenBuffer );
					StringDisplay( "备B:", 4, 48, 0, ScreenBuffer );
					StringDisplay( "备C:", 6, 48, 0, ScreenBuffer );
					StringDisplay( "S1:", 2, 150, 0, ScreenBuffer );
					StringDisplay( "S2:", 4, 150, 0, ScreenBuffer );
					
					SendTimes++;
					if(SUCCESS == get_data_detector(Card_ID, Detector_Cfg_ID, &data))
					{
						if( data[0] == 0x01 ){
							switch(data[2]){
								case 0x00:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x01:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "断开", 4, 168, 0, ScreenBuffer );}break;
								case 0x02:{StringDisplay( "断开", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
								case 0x03:{StringDisplay( "闭合", 2, 168, 0, ScreenBuffer );StringDisplay( "闭合", 4, 168, 0, ScreenBuffer );}break;
							}
							DisplayValueN( data[3] <<8|data[4] ,2, 24, 0, 3, ScreenBuffer);//主A
							DisplayValueN( data[5] <<8|data[6] ,4, 24, 0, 3, ScreenBuffer);//主B
							DisplayValueN( data[7] <<8|data[8] ,6, 24, 0, 3, ScreenBuffer);//主C
							if(Detector_Cfg.channel_num==2){
								DisplayValueN( data[9]<<8|data[10],2, 72,0, 3, ScreenBuffer);//备A
								DisplayValueN( data[11]<<8|data[12],4, 72,0, 3, ScreenBuffer);//备B
								DisplayValueN( data[13]<<8|data[14],6, 72,0, 3, ScreenBuffer);//备C
							}else{
								StringDisplay("-", 2, 72, 0, ScreenBuffer);//备A
								StringDisplay("-", 4, 72, 0, ScreenBuffer);//备B
								StringDisplay("-", 6, 72, 0, ScreenBuffer);//备C
							}
						}else{
							ErrorTimes++;
						}
					}else{
						ErrorTimes++;
					}
					if(SendTimes >= 10000) SendTimes = 0;
					if(ErrorTimes >= 10000) ErrorTimes = 0;
//					DisplayValueN(SendTimes,6,168,0, 4, ScreenBuffer);
//					DisplayValueN(ErrorTimes,7,168,1, 4, ScreenBuffer);
					
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
				break;
#endif
#if (PROGRAM_TYPE == 2)
			case loudian:
				for(;;){
					StartAddr=0;
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+9;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+15;
					StringDisplay( "电流", 3, 60, 0, ScreenBuffer );
					StringDisplay( "\x01", 4, 114, 0, ScreenBuffer );
				
					SendTimes++;
					if(SUCCESS == get_data_detector(Card_ID, Detector_Cfg_ID, &data))
					{
						if( data[0] == 0x01 ){
							DisplayValueN( data[1] <<8|data[2] , 4, 90, 0, 3, ScreenBuffer);
						}else{
							ErrorTimes++;
						}
					}else{
						ErrorTimes++;
					}
					if(SendTimes >= 10000) SendTimes = 0;
					if(ErrorTimes >= 10000) ErrorTimes = 0;
//					DisplayValueN(SendTimes,6,168, 0, 4, ScreenBuffer);
//					DisplayValueN(ErrorTimes,7,168, 1, 4, ScreenBuffer);
					
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
				break;
			case wendu:
				for(;;){
					StartAddr=0;
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=36;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+6;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=36;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+12;
					StringDisplay( "温度", 3, 60, 0, ScreenBuffer );
					StringDisplay( "℃", 3, 114, 0, ScreenBuffer );
				
					SendTimes++;
					if(SUCCESS == get_data_detector(Card_ID, Detector_Cfg_ID, &data))
					{
						if( data[0] == 0x01 ){
							uint16_t T= (data[1] <<8|data[2])/10;
							if(T>200)
								continue;
							DisplayValueN( T , 4, 90, 0, 3, ScreenBuffer);
						}else{
							ErrorTimes++;
						}
					}else{
						ErrorTimes++;
					}
					if(SendTimes >= 10000) SendTimes = 0;
					if(ErrorTimes >= 10000) ErrorTimes = 0;
//					DisplayValueN(SendTimes,6,168, 0, 4, ScreenBuffer);
//					DisplayValueN(ErrorTimes,7,168, 1, 4, ScreenBuffer);
					
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
				break;
			case zuhe:
				for(;;){
					StartAddr=0;
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+9;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+15;
					StringDisplay( "温度", 2, 0, 0, ScreenBuffer );
					//StringDisplay( "温度２", 4, 0, 0, ScreenBuffer );
					StringDisplay( "电流", 2, 96, 0, ScreenBuffer );
					//StringDisplay( "电流２", 4, 96, 0, ScreenBuffer );
					StringDisplay( "℃", 2, 60, 0, ScreenBuffer );
					//StringDisplay( "℃", 4, 60, 0, ScreenBuffer );
					StringDisplay( "\x01", 3, 156, 0, ScreenBuffer );
					//StringDisplay( "\x01", 5, 156, 0, ScreenBuffer );
				
					SendTimes++;
					if(SUCCESS == get_data_detector(Card_ID, Detector_Cfg_ID, &data))
					{
						if( data[0] == 0x01 ){
							DisplayValueN( (data[1] <<8|data[2])/10 , 2, 42, 0, 3, ScreenBuffer);//温度1
							//DisplayValueN( (data[3] <<8|data[4])/10 , 5, 42, 0, 3, ScreenBuffer);//温度2
							DisplayValueN( data[5] <<8|data[6] , 2, 138, 0, 3, ScreenBuffer);//电流1
							//DisplayValueN( data[7] <<8|data[8] , 5, 138, 0, 3, ScreenBuffer);//电流2
						}else{
							ErrorTimes++;
						}
					}else{
						ErrorTimes++;
					}
					if(SendTimes >= 10000) SendTimes = 0;
					if(ErrorTimes >= 10000) ErrorTimes = 0;
//					DisplayValueN(SendTimes,6,168, 0, 4, ScreenBuffer);
//					DisplayValueN(ErrorTimes,7,168, 1, 4, ScreenBuffer);
					
					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
				break;
#endif
			default:
				for(;;){
					StringDisplay( "回路：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=33;
					DisplayValueN( Card_ID, 0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+9;
					StringDisplay( "地址：", 0, StartAddr, 0, ScreenBuffer );StartAddr+=32;
					DisplayValueN( Detector_Cfg_ID,0, StartAddr, 1, 3, ScreenBuffer);StartAddr+=18+15;
					StringDisplay( "未知类型", 0, StartAddr, 1, ScreenBuffer );

					My_Key_Message = OSMboxPend(Key_Mbox_Handle,2000,&err);//查看按键
					if(OS_ERR_NONE == err){
						if(*My_Key_Message==Key_Return){
							return;
						}
					}
				}
		}
	}else{
		StringDisplay( "未登记　", 0, StartAddr, 1, ScreenBuffer );
	}
	
	My_Key_Message = OSMboxPend(Key_Mbox_Handle,0,&err);//等待按键
	if(*My_Key_Message==Key_Return){
		return;
	}
}
