#include "Key.h"
#include "bsp_periph.h"
#include "MyLib.h"

#define KEY_PL	PEout(12)
#define KEY_CP	PEout(13)
#define KEY_DO	PEin(14)

#if MAIN_BOARD==1
static inline uint16_t Geg_Key_Value(void)
{
	uint16_t Key_Value=0;
	KEY_CP = 1;
	KEY_PL = 1;//锁存按键数据
	Delay_NOP( 100 );
	for(uint8_t i=0;i<16;i++){
		KEY_CP = 0;//时钟下降沿
		Delay_NOP( 100 );
		if( KEY_DO == SET )//一位读取
			Key_Value = Key_Value<<1u;
		else
			Key_Value = Key_Value<<1u|0x0001u;
		KEY_CP = 1;
		Delay_NOP( 100 );
	}
	KEY_PL = 0;
 	return Key_Value;
}
/*
********************************************************************************
* 功能描述	：查看当前按键
* 参数		：
* 返回值	：Key_enum枚举类型
********************************************************************************
*/
Key_enum Geg_Key(void)
{
	uint16_t Key_Value=0;
	Key_Value = Geg_Key_Value();
	if(Key_Value==0x0000)
		return Key_None;
	if( Key_Value ){
		if( Key_Value&0xff00 ){
			if( Key_Value&Key_1 )
				return Key_1;
			else
			if( Key_Value&Key_2 )
				return Key_2;
			else
			if( Key_Value&Key_3 )
				return Key_3;
			else
			if( Key_Value&Key_Reset )
				return Key_Reset;
			else
			if( Key_Value&Key_4 )
				return Key_4;
			else
			if( Key_Value&Key_5 )
				return Key_5;
			else
			if( Key_Value&Key_6 )
				return Key_6;
			else
			if( Key_Value&Key_XiaoYin )
				return Key_XiaoYin;
		}else{
			if( Key_Value&Key_7 )
				return Key_7;
			else
			if( Key_Value&Key_8 )
				return Key_8;
			else
			if( Key_Value&Key_9 )
				return Key_9;
			else
			if( Key_Value&Key_Delete )
				return Key_Delete;
			else
			if( Key_Value&Key_x )
				return Key_x;
			else
			if( Key_Value&Key_0 )
				return Key_0;
			else
			if( Key_Value&Key_Return )
				return Key_Return;
			else
			if( Key_Value&Key_Set )
				return Key_Set;
		}
		return Key_None;
	}
	else
		return Key_None;
}
#endif

#if MAIN_BOARD==2
static inline uint32_t Geg_Key_Value(void)
{
	uint32_t Key_Value=0;
	KEY_CP = 1;
	KEY_PL = 1;//锁存按键数据
	Delay_NOP( 100 );
	//KEY_CP = 0;//时钟下降沿
	for(uint8_t i=0;i<32;i++){
		KEY_CP = 0;//时钟上升沿保持一位数据
		Delay_NOP( 100 );
		if( KEY_DO == SET )//一位读取
			Key_Value = Key_Value<<1u;
		else
			Key_Value = Key_Value<<1u|0x00000001u;
		KEY_CP = 1;
		Delay_NOP( 100 );
	}
	KEY_PL = 0;
 	return Key_Value;
}
/*
********************************************************************************
* 功能描述	：查看当前按键
* 参数		：
* 返回值	：Key_enum枚举类型
********************************************************************************
*/
Key_enum Geg_Key(void)
{
	uint32_t Key_Value=0;
	Key_Value = Geg_Key_Value();
	if( !Key_Value )return Key_None;
	if(Key_Value&0xffff0000){
		if( Key_Value&0xff000000 ){
			if( Key_Value&Key_1 )
				return Key_1;
			else
			if( Key_Value&Key_2 )
				return Key_2;
			else
			if( Key_Value&Key_3 )
				return Key_3;
			else
			if( Key_Value&Key_4 )
				return Key_4;
			else
			if( Key_Value&Key_5 )
				return Key_5;
			else
			if( Key_Value&Key_6 )
				return Key_6;
			else
			if( Key_Value&Key_7 )
				return Key_7;
			else
			if( Key_Value&Key_8 )
				return Key_8;
		}else{
			if( Key_Value&Key_9 )
				return Key_9;
			else
			if( Key_Value&Key_x )
				return Key_x;
			else
			if( Key_Value&Key_0 )
				return Key_0;
			else
			if( Key_Value&Key_j )
				return Key_j;
			else
			if( Key_Value&Key_Up )
				return Key_Up;
			else
			if( Key_Value&Key_Down )
				return Key_Down;
			else
			if( Key_Value&Key_Lift )
				return Key_Lift;
			else
			if( Key_Value&Key_Right )
				return Key_Right;
		}
	}else{
		if( Key_Value&0x0000ff00 ){
			if( Key_Value&Key_Set )
				return Key_Set;
			else
			if( Key_Value&Key_Return )
				return Key_Return;
			else
			if( Key_Value&Key_Upage )
				return Key_Upage;
			else
			if( Key_Value&Key_Delete )
				return Key_Delete;
			else
			if( Key_Value&Key_Dpage )
				return Key_Dpage;
			else
			if( Key_Value&Key_F1 )
				return Key_F1;
			else
			if( Key_Value&Key_F2 )
				return Key_F2;
			else
			if( Key_Value&Key_F3 )
				return Key_F3;
		}else{
			if( Key_Value&Key_F4 )
				return Key_F4;
			else
			if( Key_Value&Key_XiaoYin )
				return Key_XiaoYin;
			else
			if( Key_Value&Key_Reset )
				return Key_Reset;
			else
			if( Key_Value&Key_Print )
				return Key_Print;
			else
				return Key_None;
		}
	}
		return Key_None;
}
#endif
