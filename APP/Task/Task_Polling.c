#include "Task_Polling.h"
#define losetimes 70

typedef enum{
	CardNormal	=0x00,
	CardDuanLu	=0x01,
	CardNoCost	=0x02,
	CardOverCost=0x03,
	CardGround	=0x04,
	CardLost	=0x05,
}OnceCardState_enum;

typedef struct{
	uint8_t Mum;//RecordArray�б�����Ŀ
	Record_struct RecordArray[16];
}OneDetectorRecordS;


static OS_TMR *OneSecondTimer;


static void CardPolling(void);
static void BaseBoardPolling(void);
static void CardFault( uint8_t Card_ID,OnceCardState_enum CardFault );
static void PollingRespond(void);
static void DealWith_Detector( uint8_t *Data, uint8_t Card_ID );
static ErrorStatus Protocol_Analysis_Fault( OneDetectorRecordS *OneDetectorRecord, uint8_t Card_ID, uint8_t *FaultData, uint8_t *length_used );



static ErrorStatus Fault_Device_List_Matching( Record_struct Record,Fault_Device_List_struct *List );
static void FlautManage_And_FlautStorageManage( Record_struct Record );
/*���ж�*/
static void OneSecondTimerHook(void *ptmr, void *parg)
{
//	for(int Card_ID=0;Card_ID<CARD_NUM_MAX;Card_ID++){
//		if(CardState.Card_lose[Card_ID]<0x7f)
//			CardState.Card_lose[Card_ID]++;//���ӁGʧ����
//	}
}


void Task_Polling( void )
{
	uint8_t  err;
	Record_struct Record;
	
	OSTimeDlyHMSM(0,0,1,0);
	
	Record.event.card_code = 0x00;
	Record.event.address = 0x00;
	Record.event.channel = 0x00;
	Record.event.event_type = 0x0f;
	Record.event.fault_type = 0x01;
	Record.event.description = 0x0000;
	Record.time.year=systime.Year;
	Record.time.month=systime.Month;
	Record.time.day=systime.Day;
	Record.time.hour=systime.Hour;
	Record.time.minute=systime.Minute;
	Record.time.second=systime.Second;
	Store_Manage(&Record, OtherArea);	//ϵͳ�ϵ��¼

	OneSecondTimer = OSTmrCreate(0,10,OS_TMR_OPT_PERIODIC,OneSecondTimerHook,(void *)NULL,"OneSecondTimerHook",&err);
	OSTmrStart(OneSecondTimer,&err);//�����붨ʱ��
	for(;;){
		if(OSFlagPend(FLAG_GRP,StopPollingFlag,OS_FLAG_WAIT_CLR_ALL,0,&err)){
			OSFlagPost(FLAG_GRP,PollTaskEndFlag,OS_FLAG_CLR,&err);//Ѳ�������
			BaseBoardPolling();
			CardPolling();
			PollingRespond();
			OSFlagPost(FLAG_GRP,PollTaskEndFlag,OS_FLAG_SET,&err);//Ѳ��һ�����
			OSTimeDlyHMSM(0,0,0,500);
		}
	}
}
/*******************************************************************************
 * ���޹�����Ӧ����
 * ��ʾ��������ȫ�ֱ���Fault_Total_Number��Precedence_Fault_Total_Number
 ******************************************************************************/
static void PollingRespond(void)
{
	uint8_t  err;
	
	//Send_Receive_struct Send,Receive;
	if(Precedence_Fault_Total_Number==0)
		OSFlagPost(FLAG_GRP,BaoJingFlag,OS_FLAG_CLR,&err);//������־
	if(Fault_Total_Number==0)
		OSFlagPost(FLAG_GRP,GuZhangFlag,OS_FLAG_CLR,&err);//���ϱ�־
	
	if(Fault_Total_Number==0&&Precedence_Fault_Total_Number==0)
	{
		OSFlagPost(FLAG_GRP,XiaoYinFlag,OS_FLAG_CLR,&err);
	}
	
	if(Precedence_Fault_Total_Number==0)
	{
		/*֪ͨ�װ�ֹͣ���*/
		if(OSFlagAccept(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_WAIT_SET_ANY,&err)){
			if(SUCCESS == report_output(0x00)){
				OSFlagPost(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_CLR,&err);//�װ������־
#if (PROGRAM_TYPE == 2)
				OSFlagPost(FLAG_GRP,TongXinFlag,OS_FLAG_CLR,&err);
#endif				
			}
		}
	}else{
		/*֪ͨ�װ����*/
		if(OSFlagAccept(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_WAIT_CLR_ANY,&err)){
			if(SUCCESS == report_output(0x01)){
				OSFlagPost(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_SET,&err);//�װ������־
#if (PROGRAM_TYPE == 2)				
				OSFlagPost(FLAG_GRP,TongXinFlag,OS_FLAG_SET,&err);
#endif
			}
		}
	}
#if (PROGRAM_TYPE == 1)
	uint8_t F=0;
	for(uint8_t x=0;x<CARD_NUM_MAX;x++){
		if(CardState.Card_lose[x]>losetimes){
			OSFlagPost(FLAG_GRP,TongXinFlag,OS_FLAG_SET,&err);
			F=1;
			break;
		}
	}
	if(F==0)
		OSFlagPost(FLAG_GRP,TongXinFlag,OS_FLAG_CLR,&err);
#endif
}
static void BaseBoardPolling(void)
{
#define None				0x00
#define Output24DuanLu		0x01
#define Output24DuanXian	0x02
#define ChongDianShort		0x04
#define ChongDianDuanXian	0x08
#define BeiDianGuZhang		0x10
#define ZhuDianGuZhang		0x20
#define LianDong			0x40
  
	uint8_t  err;
	uint16_t  X;
	Fault_Device_List_struct *pTemp_Fault_Device,*StoreTail;
	Record_struct Record;
	uint8_t state = 0;
	
	if(SUCCESS != poll_state_board(&state)) return;
	
	if(Fault_Total_Number){
		X=Fault_Total_Number;
		pTemp_Fault_Device=Fault_Device_Tail;
		do{
			pTemp_Fault_Device=pTemp_Fault_Device->next;
			pTemp_Fault_Device->Update = 0;
		}while(--X);
	}
	
	if(state){
		Record.time.year=systime.Year;
		Record.time.month=systime.Month;
		Record.time.day=systime.Day;
		Record.time.hour=systime.Hour;
		Record.time.minute=systime.Minute;
		Record.time.second=systime.Second;
		Record.event.event_type=0x0C;//��Դ����
		Record.event.device_type=0x0E;//�豸Ϊ������Դ
		Record.event.area_code=0x01;
		Record.event.area_machine=0x00;
		Record.event.card_code=0x00;
		Record.event.address=0x00;
		Record.event.channel=0x00;
		Record.event.description=0x00;
		if(state&Output24DuanLu){
			Record.event.fault_type=0x04;
			FlautManage_And_FlautStorageManage( Record );
		}
		if(state&Output24DuanXian){
			Record.event.fault_type=0x05;
			FlautManage_And_FlautStorageManage( Record );
		}
		if(state&(ChongDianShort|ChongDianDuanXian)){
			Record.event.fault_type=0x03;
			FlautManage_And_FlautStorageManage( Record );
		}
		if(state&BeiDianGuZhang){
			Record.event.fault_type=0x02;
			FlautManage_And_FlautStorageManage( Record );
		}
		if(state&ZhuDianGuZhang){
			Record.event.fault_type=0x01;
			FlautManage_And_FlautStorageManage( Record );
		}
	}
	if(state&ZhuDianGuZhang){//��������л�
		OSFlagPost(FLAG_GRP,ZhuDianFlag,OS_FLAG_CLR,&err);
		OSFlagPost(FLAG_GRP,BeiDianFlag,OS_FLAG_SET,&err);
	}else{
		OSFlagPost(FLAG_GRP,ZhuDianFlag,OS_FLAG_SET,&err);
		OSFlagPost(FLAG_GRP,BeiDianFlag,OS_FLAG_CLR,&err);
	}
	
	if(Fault_Total_Number){
		X=Fault_Total_Number;
		StoreTail=Fault_Device_Tail;
		do{
			pTemp_Fault_Device=StoreTail->next;
			if(	pTemp_Fault_Device->Update != 1 && pTemp_Fault_Device->Event_Type == 0x0C) //�Ƚ��Ƿ�Ϊδ���µ�Դ����
			{//ɾ��δ���¹���
				DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number);
			}else{
				StoreTail=StoreTail->next;
			}
		}while(--X);
	}
}

static void CardPolling(void)
{
	uint8_t  i=0;
	Record_struct Record;
	OnceCardState_enum OnceCardState=CardNormal;
	static uint8_t Card_ID=0;
	uint8_t *data = NULL;
	
	if(Config.card_state==0) OSTimeDlyHMSM(0,0,1,0);//����Ƿ��л�·��Ǽ�
	else{
		for(i=0;i<CARD_NUM_MAX;i++){
		  	if(Card_ID >= CARD_NUM_MAX) Card_ID = 0;
			if(!(Config.card_state&(0x01<<Card_ID++)))//��鵱ǰ��·���Ƿ�Ǽ�
			{
				CardState.Inform[Card_ID-1] = ERROR;
				CardState.Card_lose[Card_ID-1] = 0;
				Remove_CardFault(Card_ID);	//δ�ǼǵĻ�·���Ƴ���ʧ
				continue;
			}
			
			if(CardState.Inform[Card_ID-1]==SUCCESS){//�鿴��·���Ƿ���̽�����Ǽ���Ϣ
				if(SUCCESS != poll_data_card(Card_ID, &data)){ OnceCardState=CardLost; goto cardlost;}
				if(data[1]&0x04){
					/******************************/
					Record.event.card_code = Card_ID;
					Record.event.address = 0x00;
					Record.event.channel = 0x00;
					Record.event.event_type = 0x0f;
					Record.event.fault_type = 0x02;
					Record.event.description = 0x0000;
					Record.time.year=systime.Year;
					Record.time.month=systime.Month;
					Record.time.day=systime.Day;
					Record.time.hour=systime.Hour;
					Record.time.minute=systime.Minute;
					Record.time.second=systime.Second;
					Store_Manage(&Record, OtherArea);	//��·������
					/******************************/
					if(ERROR==send_info_card( Card_ID )){
						CardState.Inform[Card_ID-1]=ERROR;
						OnceCardState=CardLost;
					}else CardState.Inform[Card_ID-1]=SUCCESS;
				}
				if(data[1]&0x02) OnceCardState=CardNoCost;
				if(data[1]&0x01) OnceCardState=CardDuanLu;
				if(data[1]==0) OnceCardState=CardNormal;
			cardlost:
				if(OnceCardState==CardNormal){/*��·����Ӧ��*/
					Remove_CardFault( Card_ID );//���֮ǰ���˶�ʧ��ָ�
					if(data[2]) DealWith_Detector(data,Card_ID);//�жϻ�·������������0����̽��������
					else Remove_All_Detector_Fult( Card_ID );//�Ƴ���ǰ��·����̽��������
				}else if(OnceCardState==CardNoCost || OnceCardState==CardDuanLu)
					CardFault( Card_ID,OnceCardState );
			}else{//֪ͨ�Ǽ���Ϣ
				if(ERROR==send_info_card( Card_ID )){
					CardState.Inform[Card_ID-1]=ERROR;
					OnceCardState=CardLost;
				}else{
					CardState.Inform[Card_ID-1]=SUCCESS;
					OnceCardState=CardNormal;
				}
			}
			
			if(OnceCardState!=CardLost)//ͨ�����������ʱ
				CardState.Card_lose[Card_ID-1]=0;//���ߴ�������
			
			if(!(CardState.Card_lose[Card_ID-1]&0x80)){//δ����ʧ
				if(CardState.Card_lose[Card_ID-1]>=losetimes){//�ж϶�ʧ
					CardState.Card_lose[Card_ID-1]|=0x80;
					CardFault( Card_ID,OnceCardState );//����ʧ
					CardState.Inform[Card_ID-1]=ERROR;
				}
			}
			break;
		}
	}
}

/*******************************************************************************
 * ���߻�·�嶪ʧ���ϴ���
 * 
 * Caller��CardPolling
 ******************************************************************************/
static void CardFault( uint8_t Card_ID,OnceCardState_enum CardFault )
{
	uint8_t  err;
	OSFlagPost(FLAG_GRP,GuZhangFlag,OS_FLAG_SET,&err);
	Record_struct Record;
	
	Record.time.year=systime.Year;
	Record.time.month=systime.Month;
	Record.time.day=systime.Day;
	Record.time.hour=systime.Hour;
	Record.time.minute=systime.Minute;
	Record.time.second=systime.Second;
	Record.event.event_type=0x0A;//��·����
	Record.event.device_type=0x20;//������Դ
	Record.event.card_code=Card_ID;//��·��
	Record.event.address=0;
	Record.event.area_code=1;
	Record.event.channel=0;
	Record.event.area_machine=1;
	Record.event.fault_type=CardFault;
	Record.event.description=0;
	FlautManage_And_FlautStorageManage( Record );
	Remove_All_Detector_Fult( Card_ID );//�Ƴ���ǰ��·����̽��������
	Remove_OtherCardFault( Card_ID,CardFault );//�Ƴ�����ǰ��·�������ͬһ��·��������
}

/*****************************************************************
 * ��̽�������Ͻ��д���
 * Data�������������飨��ȥǰ�����ֽڣ������Ŀ�ĵ�ַ��
 * Card_ID����·���ַ
 * 
 * Caller��CardPolling
 *****************************************************************/
static void DealWith_Detector( uint8_t *Data, uint8_t Card_ID )
{
	uint16_t number = 0;
	Fault_Device_List_struct *pTemp_Fault_Device, *tmp;
	OneDetectorRecordS one_detector_record;
	uint16_t data_point = 0;
	uint8_t length_used = 0;
	uint16_t i = 0;
	uint16_t N=0;
	//
	if(Fault_Total_Number)
	{
		pTemp_Fault_Device = Fault_Device_Tail->next;
		for(i=0; i<Fault_Total_Number; i++)
		{
			pTemp_Fault_Device->Update = 0;
			pTemp_Fault_Device = pTemp_Fault_Device->next;
		}
	}
	if(Precedence_Fault_Total_Number)
	{
		pTemp_Fault_Device = Precedence_Fault_Device_Tail->next;
		for(i=0; i<Precedence_Fault_Total_Number; i++)
		{
			pTemp_Fault_Device->Update = 0;
			pTemp_Fault_Device = pTemp_Fault_Device->next;
		}
	}
	
	data_point = 3;//����������Э�����Ǵӵ�6���ֽڿ�ʼ�ģ�ȥ��ǰ����Ĭ���ֽڣ��Ǵӵ�4���ֽڿ�ʼ�ģ������⸳ֵ3
	for(N=0;N<Data[2];N++){//����������ÿ��̽����
		if(SUCCESS != Protocol_Analysis_Fault(&one_detector_record, Card_ID, &Data[data_point], &length_used)) return ;
		data_point += length_used;//ƫ��һ��������ռ�ֽ���
		for(i=0; i<one_detector_record.Mum; i++)
		{//ѭ������һ��̽�������ÿ������
			FlautManage_And_FlautStorageManage(one_detector_record.RecordArray[i]);
		}
	}
	
	if(Fault_Total_Number)//�ѱ���·�ľɹ���ɾ��
	{
		number = Fault_Total_Number;
		tmp = Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device = tmp->next;
			if(pTemp_Fault_Device->CardAddress == Card_ID && pTemp_Fault_Device->Update == 0)
			{
				DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number);
			}
			else tmp = tmp->next;
		}
	}
	
#if (PROGRAM_TYPE == 1)
	if(Precedence_Fault_Total_Number)//�ѱ���·�ľɹ���ɾ��
	{
		number = Precedence_Fault_Total_Number;
		
		tmp = Precedence_Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device = tmp->next;
			if(pTemp_Fault_Device->CardAddress == Card_ID && pTemp_Fault_Device->Update == 0)
			{
				DeleteOneFault(&Precedence_Fault_Device_Tail,pTemp_Fault_Device,&Precedence_Fault_Total_Number);
			}
			else tmp = tmp->next;
		}
	}
#endif
}

static ErrorStatus Protocol_Analysis_Fault( OneDetectorRecordS *OneDetectorRecord, uint8_t Card_ID, uint8_t *FaultData, uint8_t *length_used )
{
	uint8_t detector_flag = 0;	//0��ʾ̽�����Ǽ� 1��ʾδ�Ǽ�
	Detector_Cfg_struct Detector_Cfg;
	
	/*�ж�̽�����Ƿ�Ǽ�*/
	SpiFlashRead((uint8_t *)&Detector_Cfg,((Card_ID-1)*200+(FaultData[0]-1))*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,(uint16_t)sizeof(Detector_Cfg_struct));//��ȡ��̽�����ĵǼ���Ϣ
	if(Detector_Cfg.State_type==0xff||Detector_Cfg.State_type&0x80==0) detector_flag = 1; //̽����δ�Ǽ�
	
	OneDetectorRecord->Mum=0;
	*length_used = 2;
	/*ѭ������һ��̽�����еĶ������*/
	for(int i=0;i<FaultData[1];i++){
		OneDetectorRecord->Mum+=1;
		OneDetectorRecord->RecordArray[i].time.year=systime.Year;
		OneDetectorRecord->RecordArray[i].time.month=systime.Month;
		OneDetectorRecord->RecordArray[i].time.day=systime.Day;
		OneDetectorRecord->RecordArray[i].time.hour=systime.Hour;
		OneDetectorRecord->RecordArray[i].time.minute=systime.Minute;
		OneDetectorRecord->RecordArray[i].time.second=systime.Second;
		
		OneDetectorRecord->RecordArray[i].event.device_type=0x0E;//��������
		OneDetectorRecord->RecordArray[i].event.card_code=Card_ID;//��·���ַ
		OneDetectorRecord->RecordArray[i].event.address=FaultData[0];//̽������ַ
		OneDetectorRecord->RecordArray[i].event.area_code=1;
		OneDetectorRecord->RecordArray[i].event.area_machine=1;
		OneDetectorRecord->RecordArray[i].event.event_type = 0x08;
		
		switch(Detector_Cfg.State_type&0x7f){//�жϹ�������
#if (PROGRAM_TYPE == 1)
			case 0x01://�������ߵ�ѹ����
			case 0x02://�������ߵ�ѹ����
			case 0x05://�������ߵ�ѹ
			case 0x06://�������ߵ�ѹ
			{
				if(FaultData[i+2]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[i+2]&0x03)+1;//��Դ��
					switch(FaultData[i+2]>>5){//�жϹ�������
					case 1:{//��ѹ
						OneDetectorRecord->RecordArray[i].event.fault_type=0x04;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//��������
					}break;
					case 2:{//Ƿѹ
						OneDetectorRecord->RecordArray[i].event.fault_type=0x05;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//��������
					}break;
					case 3:{//����
						OneDetectorRecord->RecordArray[i].event.fault_type=0x06;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//��������
					}break;
					case 4:{//����
						OneDetectorRecord->RecordArray[i].event.fault_type=0x08;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//��������
					}break;
					case 5:{//��Դ�ж�
						OneDetectorRecord->RecordArray[i].event.fault_type=0x09;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//��������
					}break;
					case 6:{//ȱ��
						OneDetectorRecord->RecordArray[i].event.fault_type=0x07;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//��������
					}break;
					default: return ERROR;
					}
				}else{//��ʧ
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;//��Դ��
					OneDetectorRecord->RecordArray[i].event.description=0;//��������
				}
			}break;
			case 0x03:
			case 0x04:{//����
				if(FaultData[i+2]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[i+2]&0x1F)+1;//��Դ��
					switch(FaultData[i+2]>>5){//�жϹ�������
					case 1:{//��ѹ
						OneDetectorRecord->RecordArray[i].event.fault_type=0x04;
						OneDetectorRecord->RecordArray[i].event.description=0;//��������
					}break;
					case 2:{//Ƿѹ
						OneDetectorRecord->RecordArray[i].event.fault_type=0x05;
						OneDetectorRecord->RecordArray[i].event.description=0;//��������
					}break;
					case 3:{//����
						OneDetectorRecord->RecordArray[i].event.fault_type=0x06;
						OneDetectorRecord->RecordArray[i].event.description=0;//��������
					}break;
					case 4:{//����
						OneDetectorRecord->RecordArray[i].event.fault_type=0x08;
						OneDetectorRecord->RecordArray[i].event.description=0;//��������
					}break;
					case 5:{//��Դ�ж�
						OneDetectorRecord->RecordArray[i].event.fault_type=0x09;
						OneDetectorRecord->RecordArray[i].event.description=0;//��������
					}break;
					case 6:{//ȱ��
						OneDetectorRecord->RecordArray[i].event.fault_type=0x07;
						OneDetectorRecord->RecordArray[i].event.description=0;//��������
					}break;
					default:return ERROR;
					}
				}else{//��ʧ
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;//��Դ��
					OneDetectorRecord->RecordArray[i].event.description=0;//��������
				}
			}break;
#else
			case 0x12:{//©��
				if(FaultData[*length_used]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[*length_used]&0x1F);//ͨ��
					switch(FaultData[*length_used]>>5)
					{
						case 0x01: //��������ʧ
							OneDetectorRecord->RecordArray[i].event.fault_type=0x11;
							OneDetectorRecord->RecordArray[i].event.description=0;
							break;
						case 0x02: //��
							OneDetectorRecord->RecordArray[i].event.fault_type=0x12; //��������
							OneDetectorRecord->RecordArray[i].event.description=
								(FaultData[*length_used+1]<<8|FaultData[*length_used+2]);
							*length_used += 2;
							break;
						default:
							return ERROR;
					}
				}
				else //̽������ʧ
				{
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;
					OneDetectorRecord->RecordArray[i].event.description=0;
				}
			}break;
			case 0x11:{//�¶�
				if(FaultData[*length_used]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[*length_used]&0x1F);//ͨ��
					switch(FaultData[*length_used]>>5)
					{
						case 0x01: //��������ʧ
							OneDetectorRecord->RecordArray[i].event.fault_type=0x13;
							OneDetectorRecord->RecordArray[i].event.description=0;
							break;
						case 0x02: //��
							OneDetectorRecord->RecordArray[i].event.fault_type=0x14; //�¶ȱ���
							OneDetectorRecord->RecordArray[i].event.description=
								(FaultData[*length_used+1]<<8|FaultData[*length_used+2]);
							OneDetectorRecord->RecordArray[i].event.description /= 10;
							*length_used += 2;
							break;
						default:
							return ERROR;
					}
				}
				else //̽������ʧ
				{
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;
					OneDetectorRecord->RecordArray[i].event.description=0;
				}
			}break;
			case 0x13:{//����+�¶�
				if(FaultData[*length_used]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[*length_used]&0x1F);//ͨ��
					switch(FaultData[*length_used]>>5)
					{
						case 0x01: //��������ʧ
							if((FaultData[*length_used]&0x1f) >= 0x03)
							{
								OneDetectorRecord->RecordArray[i].event.fault_type=0x11;
							}
							else
							{
								OneDetectorRecord->RecordArray[i].event.fault_type=0x13;
							}
							OneDetectorRecord->RecordArray[i].event.description=0;
							break;
						case 0x02: //��
							if((FaultData[*length_used]&0x1f) >= 0x03)
							{
								OneDetectorRecord->RecordArray[i].event.fault_type=0x12; //��������
								OneDetectorRecord->RecordArray[i].event.description=
									(FaultData[*length_used+1]<<8|FaultData[*length_used+2]);
							}
							else
							{
								OneDetectorRecord->RecordArray[i].event.fault_type=0x14; //�¶ȱ���
								OneDetectorRecord->RecordArray[i].event.description=
									(FaultData[*length_used+1]<<8|FaultData[*length_used+2]);
								OneDetectorRecord->RecordArray[i].event.description /= 10;
							}
							*length_used += 2;
							break;
						default:
							return ERROR;
					}
				}
				else //̽������ʧ
				{
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;
					OneDetectorRecord->RecordArray[i].event.description=0;
				}
			}break;
#endif
			default:return ERROR;
		}
		*length_used += 1;
	}
	
	if(detector_flag == 1) OneDetectorRecord->Mum = 0;
	return SUCCESS;
}












ErrorStatus Fault_Device_List_Matching( Record_struct Record,Fault_Device_List_struct *List )
{
	/*ȫ�ֱ��� Fault_Device_Tail */
	Fault_Device_List_struct *pTemp_Fault_Device=List;
	
	if(List == NULL) return ERROR;
	do{
		pTemp_Fault_Device=pTemp_Fault_Device->next;
		if( pTemp_Fault_Device->CardAddress==Record.event.card_code &&		/*��·�����*/ \
			pTemp_Fault_Device->DetectorAddress==Record.event.address &&	/*̽������ַ���*/ \
			pTemp_Fault_Device->DetectorChannel==Record.event.channel &&	/*̽����ͨ�����*/ \
			pTemp_Fault_Device->Event_Type==Record.event.event_type &&		/*�¼��������*/ \
			pTemp_Fault_Device->Fault_Type==Record.event.fault_type			/*�����������*/
#if PROGRAM_TYPE == 1
			&&pTemp_Fault_Device->Fault_Type_Dsc==Record.event.description	/*�����������*/
#endif
			)
		{
			pTemp_Fault_Device->Update = 1;
			return SUCCESS;
		}
	}while(!(pTemp_Fault_Device==List));
	return ERROR;
}

static void FlautManage_And_FlautStorageManage( Record_struct Record )
{
	uint8_t  err;
	StoreArea_enum StoreArea;
	Fault_Device_List_struct **List;
	uint16_t *Total_Number;

	if(Record.event.event_type==0x08 &&
		(Record.event.fault_type==0x09 || Record.event.fault_type==0x12 || Record.event.fault_type==0x14)){
		List=&Precedence_Fault_Device_Tail;
		Total_Number=&Precedence_Fault_Total_Number;
		StoreArea=FireArea;
			OSFlagPost(FLAG_GRP,BaoJingFlag,OS_FLAG_SET,&err);//������־
	}else{
		List=&Fault_Device_Tail;
		Total_Number=&Fault_Total_Number;
		StoreArea=OtherArea;
		OSFlagPost(FLAG_GRP,GuZhangFlag,OS_FLAG_SET,&err);//���ϱ�־
	}
	
	if(ERROR==Fault_Device_List_Matching( Record,*List ))//�ж��Ƿ�����ͬ����
	{
		AddOneFault( List, &Record, Total_Number );//����¹���
		if(Config.print_state)
		{
			OS_ENTER_CRITICAL()
			PrintOneFault( Record );
			OS_EXIT_CRITICAL()
		}
		report_record( &Record );
		Store_Manage( &Record, StoreArea );//�洢����
		OSFlagPost(FLAG_GRP,XiaoYinFlag,OS_FLAG_CLR,&err);
		OSFlagPost(FLAG_GRP,ScreenRefreshFlag,OS_FLAG_SET,&err);
	}
}
			