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
	uint8_t Mum;//RecordArray中保存数目
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
/*秒中断*/
static void OneSecondTimerHook(void *ptmr, void *parg)
{
//	for(int Card_ID=0;Card_ID<CARD_NUM_MAX;Card_ID++){
//		if(CardState.Card_lose[Card_ID]<0x7f)
//			CardState.Card_lose[Card_ID]++;//增加丟失秒数
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
	Store_Manage(&Record, OtherArea);	//系统上电记录

	OneSecondTimer = OSTmrCreate(0,10,OS_TMR_OPT_PERIODIC,OneSecondTimerHook,(void *)NULL,"OneSecondTimerHook",&err);
	OSTmrStart(OneSecondTimer,&err);//启动秒定时器
	for(;;){
		if(OSFlagPend(FLAG_GRP,StopPollingFlag,OS_FLAG_WAIT_CLR_ALL,0,&err)){
			OSFlagPost(FLAG_GRP,PollTaskEndFlag,OS_FLAG_CLR,&err);//巡检进行中
			BaseBoardPolling();
			CardPolling();
			PollingRespond();
			OSFlagPost(FLAG_GRP,PollTaskEndFlag,OS_FLAG_SET,&err);//巡检一次完成
			OSTimeDlyHMSM(0,0,0,500);
		}
	}
}
/*******************************************************************************
 * 有无故障响应处理
 * 提示：利用了全局变量Fault_Total_Number、Precedence_Fault_Total_Number
 ******************************************************************************/
static void PollingRespond(void)
{
	uint8_t  err;
	
	//Send_Receive_struct Send,Receive;
	if(Precedence_Fault_Total_Number==0)
		OSFlagPost(FLAG_GRP,BaoJingFlag,OS_FLAG_CLR,&err);//报警标志
	if(Fault_Total_Number==0)
		OSFlagPost(FLAG_GRP,GuZhangFlag,OS_FLAG_CLR,&err);//故障标志
	
	if(Fault_Total_Number==0&&Precedence_Fault_Total_Number==0)
	{
		OSFlagPost(FLAG_GRP,XiaoYinFlag,OS_FLAG_CLR,&err);
	}
	
	if(Precedence_Fault_Total_Number==0)
	{
		/*通知底板停止输出*/
		if(OSFlagAccept(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_WAIT_SET_ANY,&err)){
			if(SUCCESS == report_output(0x00)){
				OSFlagPost(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_CLR,&err);//底板输出标志
#if (PROGRAM_TYPE == 2)
				OSFlagPost(FLAG_GRP,TongXinFlag,OS_FLAG_CLR,&err);
#endif				
			}
		}
	}else{
		/*通知底板输出*/
		if(OSFlagAccept(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_WAIT_CLR_ANY,&err)){
			if(SUCCESS == report_output(0x01)){
				OSFlagPost(FLAG_GRP,BaseBoardOutPutFlag,OS_FLAG_SET,&err);//底板输出标志
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
		Record.event.event_type=0x0C;//电源故障
		Record.event.device_type=0x0E;//设备为消防电源
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
	if(state&ZhuDianGuZhang){//主备电灯切换
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
			if(	pTemp_Fault_Device->Update != 1 && pTemp_Fault_Device->Event_Type == 0x0C) //比较是否为未更新电源故障
			{//删除未更新故障
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
	
	if(Config.card_state==0) OSTimeDlyHMSM(0,0,1,0);//检查是否有回路版登记
	else{
		for(i=0;i<CARD_NUM_MAX;i++){
		  	if(Card_ID >= CARD_NUM_MAX) Card_ID = 0;
			if(!(Config.card_state&(0x01<<Card_ID++)))//检查当前回路板是否登记
			{
				CardState.Inform[Card_ID-1] = ERROR;
				CardState.Card_lose[Card_ID-1] = 0;
				Remove_CardFault(Card_ID);	//未登记的回路板移除丢失
				continue;
			}
			
			if(CardState.Inform[Card_ID-1]==SUCCESS){//查看回路板是否获得探测器登记信息
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
					Store_Manage(&Record, OtherArea);	//回路板请求
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
				if(OnceCardState==CardNormal){/*回路正常应答*/
					Remove_CardFault( Card_ID );//如果之前报了丢失则恢复
					if(data[2]) DealWith_Detector(data,Card_ID);//判断回路故障数，大于0则处理探测器故障
					else Remove_All_Detector_Fult( Card_ID );//移除当前回路所有探测器故障
				}else if(OnceCardState==CardNoCost || OnceCardState==CardDuanLu)
					CardFault( Card_ID,OnceCardState );
			}else{//通知登记信息
				if(ERROR==send_info_card( Card_ID )){
					CardState.Inform[Card_ID-1]=ERROR;
					OnceCardState=CardLost;
				}else{
					CardState.Inform[Card_ID-1]=SUCCESS;
					OnceCardState=CardNormal;
				}
			}
			
			if(OnceCardState!=CardLost)//通行正常清零计时
				CardState.Card_lose[Card_ID-1]=0;//掉线次数清零
			
			if(!(CardState.Card_lose[Card_ID-1]&0x80)){//未报丢失
				if(CardState.Card_lose[Card_ID-1]>=losetimes){//判断丢失
					CardState.Card_lose[Card_ID-1]|=0x80;
					CardFault( Card_ID,OnceCardState );//处理丢失
					CardState.Inform[Card_ID-1]=ERROR;
				}
			}
			break;
		}
	}
}

/*******************************************************************************
 * 告诉回路板丢失故障处理
 * 
 * Caller：CardPolling
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
	Record.event.event_type=0x0A;//回路故障
	Record.event.device_type=0x20;//消防电源
	Record.event.card_code=Card_ID;//回路号
	Record.event.address=0;
	Record.event.area_code=1;
	Record.event.channel=0;
	Record.event.area_machine=1;
	Record.event.fault_type=CardFault;
	Record.event.description=0;
	FlautManage_And_FlautStorageManage( Record );
	Remove_All_Detector_Fult( Card_ID );//移除当前回路所有探测器故障
	Remove_OtherCardFault( Card_ID,CardFault );//移除除当前回路故障外的同一回路其它故障
}

/*****************************************************************
 * 对探测器故障进行处理
 * Data：接收数据数组（除去前两个字节，命令和目的地址）
 * Card_ID：回路板地址
 * 
 * Caller：CardPolling
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
	
	data_point = 3;//故障数据在协议里是从第6个字节开始的，去掉前两个默认字节，是从第4个字节开始的，所以这赋值3
	for(N=0;N<Data[2];N++){//解析数据里每个探测器
		if(SUCCESS != Protocol_Analysis_Fault(&one_detector_record, Card_ID, &Data[data_point], &length_used)) return ;
		data_point += length_used;//偏移一个故障所占字节数
		for(i=0; i<one_detector_record.Mum; i++)
		{//循环处理一个探测器里的每个故障
			FlautManage_And_FlautStorageManage(one_detector_record.RecordArray[i]);
		}
	}
	
	if(Fault_Total_Number)//把本回路的旧故障删除
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
	if(Precedence_Fault_Total_Number)//把本回路的旧故障删除
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
	uint8_t detector_flag = 0;	//0表示探测器登记 1表示未登记
	Detector_Cfg_struct Detector_Cfg;
	
	/*判断探测器是否登记*/
	SpiFlashRead((uint8_t *)&Detector_Cfg,((Card_ID-1)*200+(FaultData[0]-1))*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,(uint16_t)sizeof(Detector_Cfg_struct));//读取该探测器的登记信息
	if(Detector_Cfg.State_type==0xff||Detector_Cfg.State_type&0x80==0) detector_flag = 1; //探测器未登记
	
	OneDetectorRecord->Mum=0;
	*length_used = 2;
	/*循环解析一个探测器中的多个故障*/
	for(int i=0;i<FaultData[1];i++){
		OneDetectorRecord->Mum+=1;
		OneDetectorRecord->RecordArray[i].time.year=systime.Year;
		OneDetectorRecord->RecordArray[i].time.month=systime.Month;
		OneDetectorRecord->RecordArray[i].time.day=systime.Day;
		OneDetectorRecord->RecordArray[i].time.hour=systime.Hour;
		OneDetectorRecord->RecordArray[i].time.minute=systime.Minute;
		OneDetectorRecord->RecordArray[i].time.second=systime.Second;
		
		OneDetectorRecord->RecordArray[i].event.device_type=0x0E;//电气火灾
		OneDetectorRecord->RecordArray[i].event.card_code=Card_ID;//回路板地址
		OneDetectorRecord->RecordArray[i].event.address=FaultData[0];//探测器地址
		OneDetectorRecord->RecordArray[i].event.area_code=1;
		OneDetectorRecord->RecordArray[i].event.area_machine=1;
		OneDetectorRecord->RecordArray[i].event.event_type = 0x08;
		
		switch(Detector_Cfg.State_type&0x7f){//判断故障类型
#if (PROGRAM_TYPE == 1)
			case 0x01://三相四线电压电流
			case 0x02://三相三线电压电流
			case 0x05://三相四线电压
			case 0x06://三相三线电压
			{
				if(FaultData[i+2]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[i+2]&0x03)+1;//电源号
					switch(FaultData[i+2]>>5){//判断故障类型
					case 1:{//过压
						OneDetectorRecord->RecordArray[i].event.fault_type=0x04;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//故障描述
					}break;
					case 2:{//欠压
						OneDetectorRecord->RecordArray[i].event.fault_type=0x05;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//故障描述
					}break;
					case 3:{//过流
						OneDetectorRecord->RecordArray[i].event.fault_type=0x06;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//故障描述
					}break;
					case 4:{//错相
						OneDetectorRecord->RecordArray[i].event.fault_type=0x08;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//故障描述
					}break;
					case 5:{//电源中断
						OneDetectorRecord->RecordArray[i].event.fault_type=0x09;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//故障描述
					}break;
					case 6:{//缺相
						OneDetectorRecord->RecordArray[i].event.fault_type=0x07;
						OneDetectorRecord->RecordArray[i].event.description=FaultData[i+2]>>2&0x07;//故障描述
					}break;
					default: return ERROR;
					}
				}else{//丢失
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;//电源号
					OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
				}
			}break;
			case 0x03:
			case 0x04:{//单相
				if(FaultData[i+2]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[i+2]&0x1F)+1;//电源号
					switch(FaultData[i+2]>>5){//判断故障类型
					case 1:{//过压
						OneDetectorRecord->RecordArray[i].event.fault_type=0x04;
						OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
					}break;
					case 2:{//欠压
						OneDetectorRecord->RecordArray[i].event.fault_type=0x05;
						OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
					}break;
					case 3:{//过流
						OneDetectorRecord->RecordArray[i].event.fault_type=0x06;
						OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
					}break;
					case 4:{//错相
						OneDetectorRecord->RecordArray[i].event.fault_type=0x08;
						OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
					}break;
					case 5:{//电源中断
						OneDetectorRecord->RecordArray[i].event.fault_type=0x09;
						OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
					}break;
					case 6:{//缺相
						OneDetectorRecord->RecordArray[i].event.fault_type=0x07;
						OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
					}break;
					default:return ERROR;
					}
				}else{//丢失
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;//电源号
					OneDetectorRecord->RecordArray[i].event.description=0;//故障描述
				}
			}break;
#else
			case 0x12:{//漏电
				if(FaultData[*length_used]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[*length_used]&0x1F);//通道
					switch(FaultData[*length_used]>>5)
					{
						case 0x01: //传感器丢失
							OneDetectorRecord->RecordArray[i].event.fault_type=0x11;
							OneDetectorRecord->RecordArray[i].event.description=0;
							break;
						case 0x02: //火警
							OneDetectorRecord->RecordArray[i].event.fault_type=0x12; //电气报警
							OneDetectorRecord->RecordArray[i].event.description=
								(FaultData[*length_used+1]<<8|FaultData[*length_used+2]);
							*length_used += 2;
							break;
						default:
							return ERROR;
					}
				}
				else //探测器丢失
				{
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;
					OneDetectorRecord->RecordArray[i].event.description=0;
				}
			}break;
			case 0x11:{//温度
				if(FaultData[*length_used]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[*length_used]&0x1F);//通道
					switch(FaultData[*length_used]>>5)
					{
						case 0x01: //传感器丢失
							OneDetectorRecord->RecordArray[i].event.fault_type=0x13;
							OneDetectorRecord->RecordArray[i].event.description=0;
							break;
						case 0x02: //火警
							OneDetectorRecord->RecordArray[i].event.fault_type=0x14; //温度报警
							OneDetectorRecord->RecordArray[i].event.description=
								(FaultData[*length_used+1]<<8|FaultData[*length_used+2]);
							OneDetectorRecord->RecordArray[i].event.description /= 10;
							*length_used += 2;
							break;
						default:
							return ERROR;
					}
				}
				else //探测器丢失
				{
					OneDetectorRecord->RecordArray[i].event.fault_type=0x01;
					OneDetectorRecord->RecordArray[i].event.channel=0;
					OneDetectorRecord->RecordArray[i].event.description=0;
				}
			}break;
			case 0x13:{//电流+温度
				if(FaultData[*length_used]!=0xFF){
					OneDetectorRecord->RecordArray[i].event.channel=(FaultData[*length_used]&0x1F);//通道
					switch(FaultData[*length_used]>>5)
					{
						case 0x01: //传感器丢失
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
						case 0x02: //火警
							if((FaultData[*length_used]&0x1f) >= 0x03)
							{
								OneDetectorRecord->RecordArray[i].event.fault_type=0x12; //电气报警
								OneDetectorRecord->RecordArray[i].event.description=
									(FaultData[*length_used+1]<<8|FaultData[*length_used+2]);
							}
							else
							{
								OneDetectorRecord->RecordArray[i].event.fault_type=0x14; //温度报警
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
				else //探测器丢失
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
	/*全局变量 Fault_Device_Tail */
	Fault_Device_List_struct *pTemp_Fault_Device=List;
	
	if(List == NULL) return ERROR;
	do{
		pTemp_Fault_Device=pTemp_Fault_Device->next;
		if( pTemp_Fault_Device->CardAddress==Record.event.card_code &&		/*回路号相等*/ \
			pTemp_Fault_Device->DetectorAddress==Record.event.address &&	/*探测器地址相等*/ \
			pTemp_Fault_Device->DetectorChannel==Record.event.channel &&	/*探测器通道相等*/ \
			pTemp_Fault_Device->Event_Type==Record.event.event_type &&		/*事件类型相等*/ \
			pTemp_Fault_Device->Fault_Type==Record.event.fault_type			/*故障类型相等*/
#if PROGRAM_TYPE == 1
			&&pTemp_Fault_Device->Fault_Type_Dsc==Record.event.description	/*故障描述相等*/
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
			OSFlagPost(FLAG_GRP,BaoJingFlag,OS_FLAG_SET,&err);//报警标志
	}else{
		List=&Fault_Device_Tail;
		Total_Number=&Fault_Total_Number;
		StoreArea=OtherArea;
		OSFlagPost(FLAG_GRP,GuZhangFlag,OS_FLAG_SET,&err);//故障标志
	}
	
	if(ERROR==Fault_Device_List_Matching( Record,*List ))//判断是否有相同故障
	{
		AddOneFault( List, &Record, Total_Number );//添加新故障
		if(Config.print_state)
		{
			OS_ENTER_CRITICAL()
			PrintOneFault( Record );
			OS_EXIT_CRITICAL()
		}
		report_record( &Record );
		Store_Manage( &Record, StoreArea );//存储故障
		OSFlagPost(FLAG_GRP,XiaoYinFlag,OS_FLAG_CLR,&err);
		OSFlagPost(FLAG_GRP,ScreenRefreshFlag,OS_FLAG_SET,&err);
	}
}
			