#include "MainBianChen.h"

#define Send_Data_H PBout(14)=0	//发送数据位为高，总线电压为0V
#define Send_Data_L PBout(14)=1	//发送数据位为低，总线电压为24V

#define REV_DATA_PIN PBin(13)

#define SendBuffSize 30
#define ReceiveBuffSize 30

uint8_t DetectorIORevBuff[ReceiveBuffSize];
uint8_t RevByteCnt=0;
uint8_t DetectorRevByteCnt=0;

static ErrorStatus  sendrevdataFun(uint8_t order,uint8_t DetAddr,uint8_t dataBuffTemp[],uint8_t datalen);
static void Send_Data_N(uint8_t Databuff[],uint8_t sendlen);
static ErrorStatus RevData__EXTI_Callback(void);

static ErrorStatus ReadDetector( uint8_t *OldAddr,uint8_t *Type,uint8_t *Channel,uint16_t *Rate );
static ErrorStatus SetDetector( uint8_t OldAddr,uint8_t NewAddr,uint8_t NewType,uint8_t Channel,uint16_t NewRate );

#if (PROGRAM_TYPE == 1)
	struct{
		uint8_t *String;
		uint8_t Value;
	}TypeList[]={
		{.String="三相四线",.Value=0x01},
		{.String="三相三线",.Value=0x02},
		{.String="单相电压",.Value=0x03},
		{.String="单相电流",.Value=0x04},
		{.String="四线电压",.Value=0x05},
		{.String="三线电压",.Value=0x06},
	};
	#define TYPE_NUMBER 6
#endif

uint8_t MainBianChen(void)
{
	uint8_t  err;
	Key_enum key;
	uint8_t OldAddr=0, Addr=0, Type=0, Channel=0;
	uint16_t Rate=0;
#if (PROGRAM_TYPE == 1)
	enum{
		Addr_Cursor=0,
		Type_Cursor,
		Channel_Cursor,
		Rate_Cursor,
		Delay_Cursor,
		Threshold_Cursor,
	}Cursor=Addr_Cursor;
#endif

	if(SUCCESS!=ReadDetector(&OldAddr,&Type,&Channel,&Rate))
	{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "请将探测器连接至本机！", 2, 24, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		StringDisplay( "请将探测器连接至本机！", 2, 24, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		StringDisplay( "请将探测器连接至本机！", 2, 24, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		OSMboxPend(Key_Mbox_Handle,10,&err);	//消除延时期间按键
		return 0;
	}
	
	Addr=OldAddr;
	for(;;){
		Clean_Lcd(ScreenBuffer);
#if (PROGRAM_TYPE == 1)
		StringDisplay( "地址", 0, 0, 0, ScreenBuffer );
		StringDisplay( "类型", 0, 48, 0, ScreenBuffer );
		StringDisplay( "通道", 0, 96, 0, ScreenBuffer );
		StringDisplay( "电流变比", 0, 144, 0, ScreenBuffer );
		DisplayValueN( Addr, 2, 3, 0, 3, ScreenBuffer);
		StringDisplay( TypeList[Type].String, 2, 36, 0, ScreenBuffer );
		DisplayValueN( Channel, 2, 96+3, 0, 3, ScreenBuffer);
		DisplayValueN( Rate, 2, 144+12, 0, 4, ScreenBuffer);
		
	#if MAIN_BOARD==1
		StringDisplay( "按＊切换输入", 6, 0, 0, ScreenBuffer );
	#endif
		switch(Cursor){
			case Addr_Cursor:
				key = input_value8(&Addr, 3, 2, 3);
				break;
			case Type_Cursor:
				StringDisplay( TypeList[Type].String, 2, 36, 1, ScreenBuffer );
				key = *(Key_enum *)(OSMboxPend(Key_Mbox_Handle,MenuDelay,&err));
				break;
			case Channel_Cursor:
				key = input_value8(&Channel, 3, 2, 96+3);
				break;
			case Rate_Cursor:
				key = input_value16(&Rate, 4, 2, 144+12);
				break;
			default: break;
		}

#endif
#if (PROGRAM_TYPE == 2)
		StringDisplay( "地址：", 3, 57, 0, ScreenBuffer );
		DisplayValueN( Addr, 3, 93, 0, 3, ScreenBuffer);
		key = input_value8(&Addr, 3, 3, 93);
#endif
		
		switch(key)
		{
#if MAIN_BOARD==1
	#if (PROGRAM_TYPE == 1)
			case Key_x:
				switch(Cursor){
					case Addr_Cursor: Cursor=Type_Cursor; break;
					case Type_Cursor: Cursor=Channel_Cursor; break;
					case Channel_Cursor: Cursor=Rate_Cursor; break;
					case Rate_Cursor: Cursor=Addr_Cursor; break;
				}
				break;
			case Key_2:
				if(Cursor == Type_Cursor)
				{
					if(Type == 0) Type = TYPE_NUMBER-1;
					else Type--;
				}
				break;
			case Key_8:
				if(Cursor == Type_Cursor)
				{
					Type++;
					if(Type == TYPE_NUMBER) Type = 0;
				}
				break;
	#endif
#endif
#if MAIN_BOARD==2
	#if (PROGRAM_TYPE == 1)
			case Key_Lift:
				switch(Cursor){
					case Addr_Cursor: Cursor=Rate_Cursor; break;
					case Type_Cursor: Cursor=Addr_Cursor; break;
					case Channel_Cursor: Cursor=Type_Cursor; break;
					case Rate_Cursor: Cursor=Channel_Cursor; break;
				}
				break;
			case Key_Right:
				switch(Cursor){
					case Addr_Cursor: Cursor=Type_Cursor; break;
					case Type_Cursor: Cursor=Channel_Cursor; break;
					case Channel_Cursor: Cursor=Rate_Cursor; break;
					case Rate_Cursor: Cursor=Addr_Cursor; break;
				}
				break;
			case Key_Up:
				if(Cursor == Type_Cursor)
				{
					if(Type == 0) Type = TYPE_NUMBER-1;
					else Type--;
				}
				break;
			case Key_Down:
				if(Cursor == Type_Cursor)
				{
					Type++;
					if(Type == TYPE_NUMBER) Type = 0;
				}
				break;
	#endif
#endif
			case Key_Set:
#if (PROGRAM_TYPE == 1)
				SetDetector( OldAddr, Addr, TypeList[Type].Value, Channel, Rate);
#endif
#if (PROGRAM_TYPE == 2)
				SetDetector( OldAddr, Addr, 0, 0, 0);
#endif
				if(SUCCESS!=ReadDetector(&Addr, &Type, &Channel, &Rate))
				{
					Clean_Lcd(ScreenBuffer);
					StringDisplay( "无法检测到探测器", 2, 0, 0, ScreenBuffer );
					OSTimeDlyHMSM(0,0,1,0);
					StringDisplay( "无法检测到探测器", 2, 0, 0, ScreenBuffer );
					OSTimeDlyHMSM(0,0,1,0);
					StringDisplay( "无法检测到探测器", 2, 0, 0, ScreenBuffer );
					OSTimeDlyHMSM(0,0,1,0);
					return 0;
				}
				OldAddr = Addr;
				break;
			case Key_Return: return 0;
			case Key_None: return 1;
		}
	}
}

static ErrorStatus ReadDetector( uint8_t *OldAddr,uint8_t *Type,uint8_t *Channel,uint16_t *Rate )
{
	DetectorIORevBuff[0]=0;
	if(SUCCESS != sendrevdataFun(0x76,0,DetectorIORevBuff,0)) return ERROR;
	
	*OldAddr = DetectorIORevBuff[1];
	*Type = (DetectorIORevBuff[2]>>4) - 1;
	*Channel = DetectorIORevBuff[2]&0x0f;
#if (PROGRAM_TYPE == 1)
	*Rate = (DetectorIORevBuff[3]<<8) | DetectorIORevBuff[4];
#endif
	return SUCCESS;
}

static ErrorStatus SetDetector( uint8_t OldAddr,uint8_t NewAddr,uint8_t NewType,uint8_t Channel,uint16_t NewRate )
{
#if (PROGRAM_TYPE == 1)
	DetectorIORevBuff[0]=0;
	DetectorIORevBuff[1]=NewRate>>8;
	DetectorIORevBuff[2]=NewRate;
	if(SUCCESS != sendrevdataFun(0x22,OldAddr,DetectorIORevBuff,3))
	{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "电流变比设置失败", 2, 0,0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		StringDisplay( "电流变比设置失败", 2, 0,0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		return ERROR;
	}
	OSTimeDlyHMSM(0,0,0,200);
	
	DetectorIORevBuff[0]=NewType<<4|Channel;
	if(SUCCESS != sendrevdataFun(0x24,OldAddr,DetectorIORevBuff,1))
	{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "类型通道设置失败", 2, 0,0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		StringDisplay( "类型通道设置失败", 2, 0,0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		return ERROR;
	}
	OSTimeDlyHMSM(0,0,0,200);
#endif
	
	DetectorIORevBuff[0]=NewAddr;
	if(SUCCESS != sendrevdataFun(0x21,OldAddr,DetectorIORevBuff,1))
	{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "地址设置失败", 2, 0, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		StringDisplay( "地址设置失败", 2, 0, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		return ERROR;
	}
	
#if (PROGRAM_TYPE == 1)
	OSTimeDlyHMSM(0,0,0,200);
	DetectorIORevBuff[0]=NewAddr;
	if(SUCCESS != sendrevdataFun(0x21,OldAddr,DetectorIORevBuff,1))	//消防电源地址需要设置两次
	{
		Clean_Lcd(ScreenBuffer);
		StringDisplay( "地址设置失败", 2, 0, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		StringDisplay( "地址设置失败", 2, 0, 0, ScreenBuffer );
		OSTimeDlyHMSM(0,0,1,0);
		return ERROR;
	}
#endif
	
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "设置成功", 2, 0, 0, ScreenBuffer );
	OSTimeDlyHMSM(0,0,1,0);
	StringDisplay( "设置成功", 2, 0, 0, ScreenBuffer );
	OSTimeDlyHMSM(0,0,1,0);
	return SUCCESS;
}

static ErrorStatus  sendrevdataFun(uint8_t order,uint8_t DetAddr,uint8_t dataBuffTemp[],uint8_t datalen)
{
	uint32_t 	j=0;
	uint8_t 	Checknum=0,SendBuff[SendBuffSize]={0};

	SendBuff[j++]=order;
	SendBuff[j++]=DetAddr;
	memcpy(&SendBuff[j], dataBuffTemp, datalen);
	j += datalen;
#if (PROGRAM_TYPE == 1)
	SendBuff[j] = CRC8_Calculate(SendBuff, j);
	j++;
#endif
#if (PROGRAM_TYPE == 2)
	for(uint32_t i=0;i<j;i++) SendBuff[j] += SendBuff[i];
	j++;
#endif
	memset(DetectorIORevBuff, 0, ReceiveBuffSize);
	
	OS_ENTER_CRITICAL();
	Send_Data_N(SendBuff,j);

	if(SUCCESS != RevData__EXTI_Callback()) 
	{
		OS_EXIT_CRITICAL();
		return ERROR;
	}
	OS_EXIT_CRITICAL();
	
#if (PROGRAM_TYPE == 1)
	Checknum = CRC8_Calculate(DetectorIORevBuff, DetectorRevByteCnt-1);
#endif
#if (PROGRAM_TYPE == 2)
	for(uint32_t i=0;i<DetectorRevByteCnt-1;i++) Checknum +=DetectorIORevBuff[i];
#endif
	
	if(Checknum==DetectorIORevBuff[DetectorRevByteCnt-1])
	{
		if(order == 0x21)
		{
			if((DetectorIORevBuff[0] == SendBuff[0])) return SUCCESS;
			else return ERROR;
		}
		else if(order == 0x76)
		{
			if(DetectorIORevBuff[0] == SendBuff[0]) return SUCCESS;
			else return ERROR;
		}
		else
		{
			if((DetectorIORevBuff[0] == SendBuff[0]) && (DetectorIORevBuff[1] == SendBuff[1])) return SUCCESS;
			else return ERROR;
		}
	}
	else return ERROR;
}

static void Send_Data_N(uint8_t Databuff[],uint8_t sendlen)
{
	uint8_t i=0,j=0,temp=0;
	
#if (PROGRAM_TYPE == 1)
	Send_Data_H;
	delay_us(1000);
	delay_us(1000);
	Send_Data_L;
	delay_us(1000);
	delay_us(1000);
#endif
	
    for(j=0;j<sendlen;j++)
    {
#if (PROGRAM_TYPE == 1)
	    Send_Data_H; 						
	    delay_us(1000);								
	    Send_Data_L; 
		delay_us(1000);
#endif
#if (PROGRAM_TYPE == 2)
		Send_Data_H;
		delay_us(1000);
		Send_Data_L;
		delay_us(500);
		Send_Data_H;
		delay_us(1500);
#endif
		
		temp = Databuff[j];				
		for(i=0;i<8;i++)						
		{					
			if((temp>>i)&0x01)
				Send_Data_H;
			else
				Send_Data_L; 
			delay_us(1000);
#if (PROGRAM_TYPE == 1)
			Send_Data_L;													
			delay_us(1000);
#endif
		}
#if (PROGRAM_TYPE == 1)
		Send_Data_L;
		delay_us(1000);
		delay_us(1000);
#endif
#if (PROGRAM_TYPE == 2)
		Send_Data_H;
		delay_us(1000);
		Send_Data_L;
		delay_us(3000);
#endif
    }
}
static ErrorStatus RevData__EXTI_Callback(void)
{
	FlagStatus Start=RESET;
	uint8_t i=0,Byte=0;
	uint32_t StartTimecnt=0;
	uint32_t TimeOutcnt=0,RevByteCnt=0;
	
	Start = SET;
	
#if (PROGRAM_TYPE == 1)
	TimeOutcnt = 0;
	while(REV_DATA_PIN!=0)	//由高变低												//等待同步位高结束
	{
		delay_us(10);
		TimeOutcnt++;
		if(TimeOutcnt>20000)												//>=2.5ms表示故障
			return ERROR;
	}
	delay_us(1500);
	TimeOutcnt = 0;
	while(REV_DATA_PIN==0) //由低变高
	{
		delay_us(10);
		TimeOutcnt++;	
		if(TimeOutcnt>250){
			return ERROR;
		}
	}
	delay_us(1500);
#endif
	
	while(Start == SET)
	{
		TimeOutcnt = 0;								
		
#if (PROGRAM_TYPE == 1)
		StartTimecnt = 0;
		while(REV_DATA_PIN!=0) //由高变低
		{
			delay_us(10);
			StartTimecnt++;	
			if(StartTimecnt>250){
				return ERROR;
			}
		}
		delay_us(500);
		StartTimecnt = 0;
		while(REV_DATA_PIN==0) //由低变高
		{
			delay_us(10);
			StartTimecnt++;	
			if(StartTimecnt>250){
				return ERROR;
			}
		}
		delay_us(1000);
#endif
#if (PROGRAM_TYPE == 2)
		StartTimecnt = 0;
		while(REV_DATA_PIN!=0) //由高变低
		{
			delay_us(10);
			StartTimecnt++;	
			if(StartTimecnt>20000){
				return ERROR;
			}
		}
		delay_us(500);
		StartTimecnt = 0;
		while(REV_DATA_PIN==0) //由低变高
		{
			delay_us(10);
			StartTimecnt++;	
			if(StartTimecnt>250){
				return ERROR;
			}
		}
		delay_us(400);
		StartTimecnt = 0;
		while(REV_DATA_PIN!=0) //由高变低
		{
			delay_us(10);
			StartTimecnt++;	
			if(StartTimecnt>250){
				return ERROR;
			}
		}
		delay_us(400);
		StartTimecnt = 0;
		while(REV_DATA_PIN==0) //由低变高
		{
			delay_us(10);
			StartTimecnt++;	
			if(StartTimecnt>250){
				return ERROR;
			}
		}
		delay_us(1000);
#endif
		
		delay_us(200);
		for(i=0;i<8;i++)
		{
			if(REV_DATA_PIN==0)						
				Byte = (Byte>>1)|0x80;
			else
				Byte = (Byte>>1)&(~0x80);
			delay_us(1000);							
		}
		DetectorIORevBuff[RevByteCnt++]=Byte;
		
#if (PROGRAM_TYPE == 2)
		delay_us(2000);
#endif
		delay_us(1000);
		delay_us(800);
		if(RevByteCnt>=ReceiveBuffSize)
		{
			break;
		}
		while(REV_DATA_PIN == 1)					
		{
			TimeOutcnt++;
			delay_us(10);	
			if(TimeOutcnt>600)
			{
				Start=RESET;
				break;
			}
		}
	}
	DetectorRevByteCnt=RevByteCnt;
	
	return SUCCESS;
}
