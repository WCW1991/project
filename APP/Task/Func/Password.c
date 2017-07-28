#include "Password.h"

uint8_t const PWD_BMP[] =
{//请输入密码复位
	0x88, 0x90, 0x00, 0x10, 0x50, 0x50, 0xF8, 0x50, 0x50, 0x50, 0x10, 0x00,
	0xA0, 0x60, 0xB8, 0x20, 0x40, 0x20, 0x50, 0x48, 0x50, 0x20, 0x40, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x90, 0x60, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x30, 0x90, 0x10, 0x90, 0x30, 0x58, 0x90, 0x50, 0x10, 0x90, 0x30, 0x00,
	0x10, 0x90, 0x70, 0x10, 0x08, 0xE8, 0x08, 0x08, 0x08, 0xF8, 0x00, 0x00,
	0x20, 0x10, 0xE8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xE8, 0x08, 0x00,
	0x80, 0xE0, 0x18, 0x40, 0x40, 0x40, 0x48, 0x50, 0x40, 0x40, 0x40, 0x00, 
	
	0x00, 0x7F, 0x20, 0x01, 0x7F, 0x15, 0x15, 0x15, 0x55, 0x7F, 0x01, 0x00,
	0x09, 0x09, 0x7F, 0x05, 0x7F, 0x15, 0x7F, 0x00, 0x1E, 0x40, 0x7F, 0x00,
	0x40, 0x20, 0x10, 0x0C, 0x03, 0x00, 0x03, 0x0C, 0x10, 0x20, 0x40, 0x00,
	0x02, 0x71, 0x44, 0x45, 0x42, 0x7B, 0x42, 0x42, 0x43, 0x70, 0x03, 0x00,
	0x02, 0x3F, 0x11, 0x3F, 0x08, 0x09, 0x09, 0x09, 0x49, 0x41, 0x3F, 0x00,
	0x00, 0x50, 0x4B, 0x5E, 0x2A, 0x2A, 0x2A, 0x2A, 0x5A, 0x4B, 0x40, 0x00,
	0x00, 0x7F, 0x00, 0x40, 0x43, 0x5C, 0x40, 0x60, 0x58, 0x47, 0x40, 0x00,
};

uint8_t const Xin_BMP[] =
{//＊
	0x00, 0x80, 0x80, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
	
	0x00, 0x00, 0x10, 0x09, 0x05, 0x03, 0x05, 0x09, 0x10, 0x00, 0x00, 0x00
};

uint8_t const Space_BMP[] =
{//空格
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define PWD_FRESH_CYC	500
ErrorStatus Password_Check_Reset( void )
{
	uint8_t  err,Input[4];
	Key_enum *My_Key_Message;
	
	Clean_Lcd(ScreenBuffer);
	Bmp_Display((uint8_t *)PWD_BMP, 2, 22, 2, 84, ScreenBuffer);
	for(uint8_t i=0;i<4;){
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,MenuDelay,&err);//等待按键
		if( err==OS_ERR_NONE ){
			switch(*My_Key_Message){
				case Key_1:{Input[i]=1;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_2:{Input[i]=2;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_3:{Input[i]=3;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_4:{Input[i]=4;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_5:{Input[i]=5;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_6:{Input[i]=6;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_7:{Input[i]=7;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_8:{Input[i]=8;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_9:{Input[i]=9;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_0:{Input[i]=0;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_Delete:{if(i != 0){ i--;}Bmp_Display((uint8_t *)Space_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);}break;
				case Key_Return:{*My_Key_Message = Key_Unused;Clean_Lcd(ScreenBuffer);return ERROR;}break;
				default:break;
			}
		}
		else
		{
			Clean_Lcd(ScreenBuffer);
			return ERROR;
		}
	}
	if((memcmp(Config.password, "\xff\xff\xff\xff", 4) == 0) && (memcmp(Input, "\x01\x01\x01\x01", 4) == 0))
	{
		*My_Key_Message = Key_Unused;
		Clean_Lcd(ScreenBuffer);
		return SUCCESS;
	}
	if(memcmp(Input, Config.password, 4) == 0)
	{
		*My_Key_Message = Key_Unused;
		Clean_Lcd(ScreenBuffer);
		return SUCCESS;
	}
	else
	{
		*My_Key_Message = Key_Unused;
		Clean_Lcd(ScreenBuffer);
		return ERROR;
	}
}

ErrorStatus Password_Check( void )
{
	uint16_t counter = 0;
	uint8_t  err,Input[4];
	Key_enum *My_Key_Message;
	
//	return SUCCESS;
	
	Clean_Lcd(ScreenBuffer);
	for(uint8_t i=0;i<4;){
		StringDisplay( "请输入密码", 2, 34, 0, ScreenBuffer );
		for(uint8_t j=0; j<i; j++)
		{
			Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*j), 2, 12, ScreenBuffer);
		}
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,PWD_FRESH_CYC,&err);//等待按键
		if( err==OS_ERR_NONE ){
			counter = 0;
			switch(*My_Key_Message){
				case Key_1:{Input[i]=1;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_2:{Input[i]=2;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_3:{Input[i]=3;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_4:{Input[i]=4;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_5:{Input[i]=5;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_6:{Input[i]=6;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_7:{Input[i]=7;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_8:{Input[i]=8;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_9:{Input[i]=9;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_0:{Input[i]=0;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_Delete:{if(i != 0){ i--;}Bmp_Display((uint8_t *)Space_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);}break;
				case Key_Return:{return ERROR;}break;
				default:break;
			}
		}
		else
		{
			counter++;
			if(counter >= MenuDelay/PWD_FRESH_CYC)
			{
				return ERROR;
			}
		}
	}
	if((memcmp(Config.password, "\xff\xff\xff\xff", 4) == 0) && (memcmp(Input, "\x01\x01\x01\x01", 4) == 0))
	{
		return SUCCESS;
	}
	if(memcmp(Input, Config.password, 4) == 0)
	{
		return SUCCESS;
	}
	else return ERROR;
}

uint8_t ModifyPassword(void)
{
	uint8_t  err;
	uint16_t counter = 0;
	Key_enum *My_Key_Message;
	uint8_t Input[4];
	for(;;){
		Clean_Lcd(ScreenBuffer);
		for(uint8_t i=0;i<4;){
			StringDisplay( "修改密码", 0, 0, 1, ScreenBuffer );
			StringDisplay( "输入原始密码：", 2, 0, 0, ScreenBuffer);
			for(uint8_t j=0; j<i; j++)
			{
				Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*j), 2, 12, ScreenBuffer);
			}
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,PWD_FRESH_CYC,&err);//等待按键
			if( err==OS_ERR_NONE ){
				counter = 0;
				switch(*My_Key_Message){
				case Key_1:{Input[i]=1;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_2:{Input[i]=2;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_3:{Input[i]=3;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_4:{Input[i]=4;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_5:{Input[i]=5;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_6:{Input[i]=6;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_7:{Input[i]=7;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_8:{Input[i]=8;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_9:{Input[i]=9;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_0:{Input[i]=0;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_Delete:{if(i != 0){ i--;}Bmp_Display((uint8_t *)Space_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);}break;
				case Key_Return:{return 0;}break;
				default:break;
				}
			}
			else
			{
				counter++;
				if(counter >= MenuDelay/PWD_FRESH_CYC)
				{
					return 1;
				}
			}
		}
		if(memcmp(Input, Config.password, 4))
			return 0;
		Clean_Lcd(ScreenBuffer);
		for(uint8_t i=0;i<4;){
			StringDisplay( "修改密码", 0, 0, 1, ScreenBuffer );
			StringDisplay( "　输入新密码：", 2, 0, 0, ScreenBuffer);
			for(uint8_t j=0; j<i; j++)
			{
				Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*j), 2, 12, ScreenBuffer);
			}
			My_Key_Message = OSMboxPend(Key_Mbox_Handle,PWD_FRESH_CYC,&err);//等待按键
			if( err==OS_ERR_NONE ){
				counter = 0;
				switch(*My_Key_Message){
				case Key_1:{Input[i]=1;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_2:{Input[i]=2;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_3:{Input[i]=3;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_4:{Input[i]=4;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_5:{Input[i]=5;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_6:{Input[i]=6;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_7:{Input[i]=7;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_8:{Input[i]=8;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_9:{Input[i]=9;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_0:{Input[i]=0;Bmp_Display((uint8_t *)Xin_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);i++;}break;
				case Key_Delete:{if(i != 0){ i--;}Bmp_Display((uint8_t *)Space_BMP, 4, 70+(12*i), 2, 12, ScreenBuffer);}break;
				case Key_Return:{return 0;}break;
				default:break;
				}
			}
			else
			{
				counter++;
				if(counter >= MenuDelay/PWD_FRESH_CYC)
				{
					return 1;
				}
			}
		}
		
		memcpy(Config.password, Input, 4);
		config_store();
		
		return 0;
	}
	
}
