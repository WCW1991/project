#include "Input_Value.h"
/*
*********************************************************************************************************
*
* 文件名    : Input_Value.c
* 版本号    : V1.00
* 作  者    : j
*             
*
* 版权说明  : 成都安吉斯智能设备有限公司
* 文件描述  : 实现总线通讯
* 开发环境  : 
* 修改日期  : 2017/02/16
*********************************************************************************************************
*/

/*******************************************************************************
* 函数名         : input_value8
* 功能描述       : 实现输入框输入8位数据（等待超时返回Key_None）
*
* 输入参数       : -value 输入数值存储指针
*				  -length 可输入长度
*				  -StartPage 输入框y位置
*				  -StartAddr 输入框x位置
*
* 返回值         : 按键值
*******************************************************************************/
Key_enum input_value8(uint8_t *value, uint8_t length, uint8_t StartPage, uint8_t StartAddr)
{
	uint8_t  err, tmp;
	Key_enum *My_Key_Message;
	static uint8_t bit = 0;
	
	DisplayValueN(*value, StartPage, StartAddr, 1, length, ScreenBuffer);
	
	My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
	if( err==OS_ERR_NONE ){
		switch(*My_Key_Message){
			case Key_1: tmp = 1; break;
			case Key_2: tmp = 2; break;
			case Key_3: tmp = 3; break;
			case Key_4: tmp = 4; break;
			case Key_5: tmp = 5; break;
			case Key_6: tmp = 6; break;
			case Key_7: tmp = 7; break;
			case Key_8: tmp = 8; break;
			case Key_9: tmp = 9; break;
			case Key_0: tmp = 0; break;
			case Key_Delete:
				if(bit > 0)
				{
					bit--;
					*value /= 10;
				}
				else *value = 0;
				return *My_Key_Message;
			default: bit = 0;return *My_Key_Message;
		}
		if(bit == 0 || bit >= length)
		{
			*value=tmp;
			if(*value == 0) bit = 0;
			else bit = 1;
		}
		else if(bit < length)
		{
			if((*value * 10) + tmp > 255) *value = 255;
			else *value = (*value * 10) + tmp;
			bit++;
		}
	}
	else return Key_None;
	
	return *My_Key_Message;
}

/*******************************************************************************
* 函数名         : input_value16
* 功能描述       : 实现输入框输入16位数据（等待超时返回Key_None）
*
* 输入参数       : -value 输入数值存储指针
*				  -length 可输入长度
*				  -StartPage 输入框y位置
*				  -StartAddr 输入框x位置
*
* 返回值         : 按键值
*******************************************************************************/
Key_enum input_value16(uint16_t *value, uint8_t length, uint8_t StartPage, uint8_t StartAddr)
{
	uint8_t  err, tmp;
	Key_enum *My_Key_Message;
	static uint8_t bit = 0;
	
	DisplayValueN(*value, StartPage, StartAddr, 1, length, ScreenBuffer);
	
	My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
	if( err==OS_ERR_NONE ){
		switch(*My_Key_Message){
			case Key_1: tmp = 1; break;
			case Key_2: tmp = 2; break;
			case Key_3: tmp = 3; break;
			case Key_4: tmp = 4; break;
			case Key_5: tmp = 5; break;
			case Key_6: tmp = 6; break;
			case Key_7: tmp = 7; break;
			case Key_8: tmp = 8; break;
			case Key_9: tmp = 9; break;
			case Key_0: tmp = 0; break;
			case Key_Delete:
				if(bit > 0)
				{
					bit--;
					*value /= 10;
				}
				else *value = 0;
				return *My_Key_Message;
			default: bit = 0; return *My_Key_Message;
		}
		if(bit == 0 || bit >= length)
		{
			*value=tmp;
			if(*value == 0) bit = 0;
			else bit = 1;
		}
		else if(bit < length)
		{
			*value = (*value * 10) + tmp;
			bit++;
		}
	}
	else return Key_None;
	
	return *My_Key_Message;
}
