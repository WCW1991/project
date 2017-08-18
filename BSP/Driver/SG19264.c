#include <SG19264.h>
#include "bsp_periph.h"
#include "w25x40.h"
#include "MyLib.h"
const unsigned char Num[]=
{
/*------------------------------------------------------------------------------
;  源文件 / 文字 : 0123456789/:-mA℃
;  宽×高（像素）: 6×8*13
------------------------------------------------------------------------------*/
0x00,0x7E,0x81,0x81,0x7E,0x00, 0x00,0x00,0x82,0xFF,0x80,0x00,//0、1
0x00,0xC6,0xA1,0x91,0x8E,0x00, 0x00,0x42,0x89,0x89,0x76,0x00,//2、3
0x30,0x28,0xA6,0xFF,0xA0,0x00, 0x00,0x4F,0x89,0x89,0x71,0x00,//4、5
0x00,0x7E,0x89,0x89,0x72,0x00, 0x00,0x01,0xF1,0x0D,0x03,0x00,//6、7
0x00,0x76,0x89,0x89,0x76,0x00, 0x00,0x4E,0x91,0x91,0x7E,0x00,//8、9
};

const unsigned char abc[]=
{
/*------------------------------------------------------------------------------
;  源文件 / 文字 : abc
;  宽×高（像素）: 6×8*26
------------------------------------------------------------------------------*/
0x28,0x54,0x54,0x78,0x40,0x00, 0x01,0x7F,0x48,0x48,0x30,0x00,//a、b
0x00,0x38,0x44,0x44,0x2C,0x00, 0x00,0x30,0x48,0x49,0x7F,0x40,//c、d
0x00,0x38,0x54,0x54,0x58,0x00, 0x00,0x48,0x7E,0x49,0x0B,0x00,
0x00,0xFC,0xD4,0xDC,0xC4,0x00, 0x41,0x7F,0x48,0x08,0x70,0x40,
0x00,0x44,0x7D,0x40,0x00,0x00, 0x00,0x80,0x84,0xFD,0x00,0x00,
0x41,0x7F,0x10,0x2C,0x44,0x00, 0x00,0x41,0x7F,0x40,0x00,0x00,
0x7C,0x04,0x7C,0x04,0x78,0x00, 0x44,0x7C,0x44,0x04,0x78,0x40,
0x38,0x44,0x44,0x44,0x38,0x00, 0x84,0xFC,0x24,0x24,0x18,0x00,
0x00,0x18,0x24,0x24,0xFC,0x80, 0x00,0x44,0x7C,0x48,0x04,0x00,
0x00,0x4C,0x54,0x54,0x64,0x00, 0x00,0x04,0x7F,0x44,0x00,0x00,
0x04,0x3C,0x40,0x44,0x7C,0x40, 0x04,0x1C,0x60,0x1C,0x04,0x00,
0x04,0x1C,0x60,0x1C,0x60,0x1C, 0x44,0x6C,0x10,0x6C,0x44,0x00,
0x84,0x9C,0x60,0x1C,0x04,0x00, 0x00,0x64,0x54,0x4C,0x44,0x00,//y、z
};
const unsigned char ABC[]=
{
/*------------------------------------------------------------------------------
;  源文件 / 文字 : ABCDEFGHIJKLMNOPQRSTUVWXYZ
;  宽×高（像素）: 6×8
------------------------------------------------------------------------------*/
0x40,0x7C,0x13,0x7C,0x40,0x00, 0x41,0x7F,0x49,0x49,0x36,0x00,
0x3E,0x41,0x41,0x41,0x23,0x00, 0x41,0x7F,0x41,0x41,0x3E,0x00,
0x41,0x7F,0x49,0x49,0x63,0x00, 0x41,0x7F,0x49,0x1D,0x03,0x00,
0x3E,0x41,0x51,0x33,0x10,0x00, 0x41,0x7F,0x49,0x08,0x7F,0x41,
0x00,0x41,0x7F,0x41,0x00,0x00, 0x80,0x81,0x7F,0x01,0x00,0x00,
0x41,0x7F,0x05,0x7A,0x41,0x00, 0x41,0x7F,0x41,0x40,0x60,0x00,
0x41,0x7F,0x1E,0x0C,0x7F,0x41, 0x41,0x7F,0x4C,0x11,0x7F,0x01,
0x3E,0x41,0x41,0x41,0x3E,0x00, 0x41,0x7F,0x49,0x09,0x06,0x00,
0x3E,0x61,0x51,0xE1,0xBE,0x00, 0x41,0x7F,0x45,0x0D,0x32,0x40,
0x00,0x66,0x49,0x49,0x33,0x00, 0x03,0x41,0x7F,0x41,0x03,0x00,
0x01,0x3F,0x40,0x40,0x3F,0x01, 0x01,0x1F,0x60,0x1F,0x01,0x00,
0x07,0x78,0x0F,0x78,0x07,0x00, 0x41,0x77,0x08,0x77,0x41,0x00,
0x01,0x47,0x78,0x47,0x01,0x00, 0x43,0x71,0x49,0x47,0x61,0x00,
};

const unsigned char Bmp1[11]=
{
0xF8,0x08,0xF8,0x08,0xF0,0x00,0x80,0xF8,0x26,0xF8,0x80,//mA 11×8
};
const unsigned char Bmp2[9]=
{ 
0x03,0x03,0x3C,0x42,0x81,0x81,0x81,0x81,0x42,//℃ 9×8
};
const unsigned char Bmp3[6]=
{ 
0x00,0xC0,0x30,0x0C,0x03,0x00, // / 6×8
};
const unsigned char Bmp4[6]=
{
0x00,0x00,0x6C,0x00,0x00,0x00, // ：6×8
};
const unsigned char Bmp5[6]=
{ 
0x00,0x10,0x10,0x10,0x10,0x00, // - 6×8
};
const unsigned char Bmp6[6]=
{ 
0x40,0x7C,0x13,0x7C,0x40,0x00, // A 6×8
};



#define lcd_NOP		12

#define LCD_RST		PCout(9)
#define LCD_RS		PAout(11)
#define LCD_E		PCout(8)

#define LCD_D0		PEout(7)
#define LCD_D1		PEout(8)
#define LCD_D2		PEout(9)
#define LCD_D3		PEout(10)
#define LCD_D4		PEout(11)
#define LCD_D5		PEout(6)
#define LCD_D6		PEout(5)
#define LCD_D7		PEout(4)

#define LCD_CSA		PCout(7)
#define LCD_CSB		PCout(6)


/* LCD命令 */
#define Com_Lcd_On			0x3f
#define Com_Lcd_Off			0x3e
#define Com_Lcd_Page		0xb8 //低三位使用时设置
#define Com_Lcd_Addr		0x40 //低6位为地址
#define Com_Lcd_Line_Start	0xc0 //低6位为地址


#define Lift_Lcd	LCD_CSA=0;LCD_CSB=0;
#define Midd_Lcd	LCD_CSA=0;LCD_CSB=1;
#define Right_Lcd	LCD_CSA=1;LCD_CSB=0;
#define No_Lcd		LCD_CSA=1;LCD_CSB=1;

static void Write_Com( uint8_t  Com )
{
	LCD_E = 0;
	LCD_RS = 0;//命令
	
	if( Com&0x01u )	LCD_D0 = 1;
	else			LCD_D0 = 0;
	
	if( Com&0x02u )	LCD_D1 = 1;
	else			LCD_D1 = 0;
	
	if( Com&0x04u )	LCD_D2 = 1;
	else			LCD_D2 = 0;
	
	if( Com&0x08u )	LCD_D3 = 1;
	else			LCD_D3 = 0;
	
	if( Com&0x10u )	LCD_D4 = 1;
	else			LCD_D4 = 0;
	
	if( Com&0x20u )	LCD_D5 = 1;
	else			LCD_D5 = 0;
	
	if( Com&0x40u )	LCD_D6 = 1;
	else			LCD_D6 = 0;
	
	if( Com&0x80u )	LCD_D7 = 1;
	else			LCD_D7 = 0;
	Delay_NOP( lcd_NOP );
	LCD_E = 1;
	Delay_NOP( lcd_NOP );
	LCD_E = 0;//下降沿锁存
	Delay_NOP( lcd_NOP );
}

static void Write_Data( uint8_t  Data )
{
	LCD_E = 0;
	LCD_RS = 1;//数据
	if( Data&0x01u )LCD_D0 = 1;
	else			LCD_D0 = 0;
	
	if( Data&0x02u )LCD_D1 = 1;
	else			LCD_D1 = 0;
	
	if( Data&0x04u )LCD_D2 = 1;
	else			LCD_D2 = 0;
	
	if( Data&0x08u )LCD_D3 = 1;
	else			LCD_D3 = 0;
	
	if( Data&0x10u )LCD_D4 = 1;
	else			LCD_D4 = 0;
	
	if( Data&0x20u )LCD_D5 = 1;
	else			LCD_D5 = 0;
	
	if( Data&0x40u )LCD_D6 = 1;
	else			LCD_D6 = 0;
	
	if( Data&0x80u )LCD_D7 = 1;
	else			LCD_D7 = 0;
	Delay_NOP( lcd_NOP );
	LCD_E = 1;
	Delay_NOP( lcd_NOP );
	LCD_E = 0;//下降沿锁存
	Delay_NOP( lcd_NOP );
}

static void ZiKuChack( void )
{
	uint8_t test[]={"安"};
	uint8_t Buffer[2];
	SpiFlashRead(Buffer,((test[0]-0xa1)*94)*24+(test[1]-0xa1)*24,2);//字模检查
	if(Buffer[0]!=0x60&&Buffer[0]!=0x20){
		uint16_t Line=0;
		uint16_t j=0;
		/****************************/
		Lift_Lcd;
		for(Line=0;Line<8;Line++){
			Write_Com( Com_Lcd_Page | Line );
			Write_Com( Com_Lcd_Addr | 0 );
			for(j=0;j<64;j++){
				Write_Data(0xFF);
			}
		}
		/****************************/
		Midd_Lcd;
		for(Line=0;Line<8;Line++){
			Write_Com( Com_Lcd_Page | Line );
			Write_Com( Com_Lcd_Addr | 0 );
			for(j=0;j<64;j++){
				Write_Data(0xFF);
			}
		}
		/****************************/
		Right_Lcd;
		for(Line=0;Line<8;Line++){
			Write_Com( Com_Lcd_Page | Line );
			Write_Com( Com_Lcd_Addr | 0 );
			for(j=0;j<64;j++){
				Write_Data(0xFF);
			}
		}
		No_Lcd;
		while(1);
	}
}
/*
********************************************************************************
* 功能描述	：初始化LCD，能返回则说明驱动能用，前提相应的IO口可用，能读取字库
********************************************************************************
*/
void Init_Lcd( void )
{
	ScreenBuffS ScreenBuffer;
	LCD_E = 0;
	LCD_RST = 0;
	Delay_NOP( 250 );
	LCD_RST = 1;
	Delay_NOP( 250 );
	
	Lift_Lcd;
	Write_Com( Com_Lcd_Line_Start | 0u );
	Write_Com( Com_Lcd_On );
	Midd_Lcd;
	Write_Com( Com_Lcd_Line_Start | 0u );
	Write_Com( Com_Lcd_On );
	Right_Lcd;
	Write_Com( Com_Lcd_Line_Start | 0u );
	Write_Com( Com_Lcd_On );
	
	if(SUCCESS != SpiFlashCheck())
		while(1){
			Set_Lcd( ScreenBuffer );
			BufferToLCD( ScreenBuffer );
		};
	ZiKuChack();
	
	TurnOn_Backlight();
}
/*
********************************************************************************
* 功能描述	：清空屏幕缓冲
* 参数		：
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
void Clean_Lcd( ScreenBuffS ScreenBuffer )
{
	uint8_t i,j;
	for(j=0;j<8;j++)
		for(i=0;i<192;i++)
			ScreenBuffer[j][i]=0;
}
/*
********************************************************************************
* 功能描述	：填满屏幕缓冲
* 参数		：
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
void Set_Lcd( ScreenBuffS ScreenBuffer )
{
	uint8_t i,j;
	for(j=0;j<8;j++)
		for(i=0;i<192;i++)
			ScreenBuffer[j][i]=0xFF;
}
/*
********************************************************************************
* 功能描述	：将屏幕缓冲输出
* 参数		：
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
void BufferToLCD( ScreenBuffS ScreenBuffer )
{
	uint16_t Line=0;
	uint16_t j=0;
	LCD_E = 0;
	LCD_RST = 0;
	Delay_NOP( 250 );
	LCD_RST = 1;
	Delay_NOP( 250 );
	
	Lift_Lcd;
	Write_Com( Com_Lcd_Line_Start | 0u );
	Write_Com( Com_Lcd_On );
	Midd_Lcd;
	Write_Com( Com_Lcd_Line_Start | 0u );
	Write_Com( Com_Lcd_On );
	Right_Lcd;
	Write_Com( Com_Lcd_Line_Start | 0u );
	Write_Com( Com_Lcd_On );
	
	/****************************/
	Lift_Lcd;
	for(Line=0;Line<8;Line++){
		Write_Com( Com_Lcd_Page | Line );
		Write_Com( Com_Lcd_Addr | 0 );
		for(j=0;j<64;j++){
			Write_Data(ScreenBuffer[Line][j]);
		}
	}
	/****************************/
	Midd_Lcd;
	for(Line=0;Line<8;Line++){
		Write_Com( Com_Lcd_Page | Line );
		Write_Com( Com_Lcd_Addr | 0 );
		for(j=0;j<64;j++){
			Write_Data(ScreenBuffer[Line][j+64]);
		}
	}
	/****************************/
	Right_Lcd;
	for(Line=0;Line<8;Line++){
		Write_Com( Com_Lcd_Page | Line );
		Write_Com( Com_Lcd_Addr | 0 );
		for(j=0;j<64;j++){
			Write_Data(ScreenBuffer[Line][j+128]);
		}
	}
	No_Lcd;
}
/*
********************************************************************************
* 功能描述	：显示字符
* 参数		：
* 	SetCode	：GB23012编码、ASCII（数字、小写字母、/、：、-、mA、℃、A、.）编码、1代表mA、2代表℃
* 	Reverse	：非0则反色
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
extern const unsigned char en_6x16_zm[];
void StringDisplay( uint8_t *SetCode, uint8_t StartPage, uint8_t StartAddr, uint8_t Reverse, ScreenBuffS ScreenBuffer )
{
	#define ByteOfASCII 6
	#define ByteOfGB2312 24
	uint8_t i,j;
	uint8_t Buffer[24];
	if(StartPage>7||StartAddr>185)return;
	while(*SetCode){
		if(*SetCode<0xA1){/*ASCII*/
			if(Reverse>0){
				for( i=0,j=0;i<12;i++ ){
					if(i<6)
						Buffer[i]=en_6x16_zm[(*SetCode)*12+i];//下划线
					else
						Buffer[i]=0x80|en_6x16_zm[(*SetCode)*12+i];//下划线
				}
			}else{
				for( i=0;i<12;i++ )
					Buffer[i]=en_6x16_zm[(*SetCode)*12+i];//
			}
			for(j=0,i=0;j<2;j++,StartPage++){
				for(;i<(j+1)*6;i++){
					if(StartAddr>=192)return;
					ScreenBuffer[StartPage][StartAddr++]=Buffer[i];
				}
				StartAddr-=6;
			}
			StartAddr+=6;
			StartPage-=2;
			SetCode++;
		}else{/*GB2312*/
			if((*SetCode>=0xAA)&&(*SetCode<=0xAF)||(*SetCode>=0xF8))return;
			if((*SetCode+1)<0xA1||(*SetCode+1)>0xFE)return;
			
			SpiFlashRead(Buffer,(((*SetCode-0xa1)*94)+(*(SetCode+1)-0xa1))*ByteOfGB2312,ByteOfGB2312);//读字模
			if(Reverse>0)
				for( i=0;i<ByteOfGB2312;i++ )
					Buffer[i]=~Buffer[i];//反色
			for(j=0,i=0;j<2;j++,StartPage++){
				for(;i<(j+1)*12;){
					if(StartAddr>=192)return;
					ScreenBuffer[StartPage][StartAddr++]=Buffer[i++];
				}
				StartAddr-=12;
			}
			StartAddr+=12;
			StartPage-=2;
			SetCode++;SetCode++;
		}
	}
}

/*
********************************************************************************
* 功能描述	：显示一个数值（根据大小自动调节位数）
* 参数		：
* 	Value	：要显示的数值
* 	Reverse	：非0则反色
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
void DisplayValue( uint16_t Value, uint8_t StartPage, uint8_t StartAddr, uint8_t Reverse, ScreenBuffS ScreenBuffer )
{
	#define Num2ASCII(n) (n+0x30)
	char Buffer[5];
	if(Value<60000){
		sprintf(Buffer,"%d",Value);
		StringDisplay( (uint8_t*)Buffer, StartPage, StartAddr, Reverse, ScreenBuffer );
	}
}
/*
********************************************************************************
* 功能描述	：显示一个数值
* 参数		：
* 	Value	：要显示的数值
*	WeiNum	：数值所占位数（1、2、3、4）
* 	Reverse	：非0则反色
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
void DisplayValueN( uint16_t Value, uint8_t StartPage, uint8_t StartAddr, uint8_t Reverse, uint8_t WeiNum, ScreenBuffS ScreenBuffer )
{
	#define Num2ASCII(n) (n+0x30)
	char Buffer[5];
	uint8_t i,ONum,ValueNum=0;
	if(WeiNum==0)return;
	if(Value<10)
		ValueNum=1;
	else{
		if(Value<100)
			ValueNum=2;
		else
			if(Value<1000)
				ValueNum=3;
			else
				if(Value<10000)
					ValueNum=4;
				else
					ValueNum=5;
	}
	if(WeiNum>ValueNum)
		ONum=WeiNum-ValueNum;
	else
		ONum=0;
	if(ONum){
		for(i=0;i<ONum;i++){
			StringDisplay( "0", StartPage, StartAddr, Reverse, ScreenBuffer );
			StartAddr+=6;
		}
		sprintf(Buffer,"%d",Value);
		StringDisplay( (uint8_t*)Buffer, StartPage, StartAddr, Reverse, ScreenBuffer );
	}else{
		sprintf(Buffer,"%d",Value);
		StringDisplay( (uint8_t*)Buffer, StartPage, StartAddr, Reverse, ScreenBuffer );
	}
}

/*
********************************************************************************
* 功能描述	：填充屏幕缓冲中指定矩形区域
* 参数		：
*	Buffer：待填充对象
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
void Bmp_Display( uint8_t* Buffer, uint8_t StartPage, uint8_t StartAddr, uint8_t High, uint8_t Wide, ScreenBuffS ScreenBuffer )
{
	uint8_t i,j;
	for(j=0;j<High;j++)
		for(i=0;i<Wide;i++)
			ScreenBuffer[j+StartPage][i+StartAddr]=Buffer[j*Wide+i];
}


static void Display2Num( uint8_t Value, uint8_t StartPage, uint8_t StartAddr, ScreenBuffS ScreenBuffer )
{
	int i=0;
	for(i=0;i<6;i++){
		if(StartAddr>=192)return;
		ScreenBuffer[StartPage][StartAddr++]=Num[(Value/10)*6+i];
	}
	for(i=0;i<6;i++){
		if(StartAddr>=192)return;
		ScreenBuffer[StartPage][StartAddr++]=Num[(Value%10)*6+i];
	}
}
/*
********************************************************************************
* 功能描述	：显示日期和时间
* 参数		：
* 	Systime	：结构体在DS1302里定义
* 	StartPage	：只能是2或6
*	ScreenBuffer：屏幕缓冲
********************************************************************************
*/
void DisplaySystime( SYSTEMTIME Systime, uint8_t StartPage, ScreenBuffS ScreenBuffer )
{
	uint8_t i,StartAddr=144;
	if(StartPage!=2&&StartPage!=6)return;
	StartAddr=144;
	Display2Num( Systime.Year,StartPage,StartAddr,ScreenBuffer);StartAddr+=12;
	for(i=0;i<6;i++){ScreenBuffer[StartPage][StartAddr++]=Bmp3[i];}
	Display2Num( Systime.Month,StartPage,StartAddr,ScreenBuffer);StartAddr+=12;
	for(i=0;i<6;i++){ScreenBuffer[StartPage][StartAddr++]=Bmp3[i];}
	Display2Num( Systime.Day,StartPage,StartAddr,ScreenBuffer);
	StartAddr=144;
	Display2Num( Systime.Hour,StartPage+1,StartAddr,ScreenBuffer);StartAddr+=12;
	for(i=0;i<6;i++){ScreenBuffer[StartPage+1][StartAddr++]=Bmp4[i];}
	Display2Num( Systime.Minute,StartPage+1,StartAddr,ScreenBuffer);StartAddr+=12;
	for(i=0;i<6;i++){ScreenBuffer[StartPage+1][StartAddr++]=Bmp4[i];}
	Display2Num( Systime.Second,StartPage+1,StartAddr,ScreenBuffer);
}

void TurnOn_Backlight(void)
{
	PAout(12)=1;
}

void TurnOff_Backlight(void)
{
	PAout(12)=0;
}

















