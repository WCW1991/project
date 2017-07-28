#include "describe.h"

typedef struct{
	uint16_t Num;
	uint8_t Data[512];
}TypeDef_Recv_Data_t;

extern uint8_t SPI_FLASH_BUF[4096];

static uint8_t Parse_Describe(TypeDef_Recv_Data_t *recv, uint8_t *loop, uint8_t *addr, uint8_t *describe);

#define DESCRIBE_DATA_MIN	12
#define DESCRIBE_CHECK_CYC	50
uint8_t Describe(void)
{
	uint8_t  err;
	uint8_t read_loop = 0xff;
	TypeDef_Recv_Data_t recv;
	uint8_t loop, addr;
	uint8_t describe[16];
	Key_enum *My_Key_Message;
	
	OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_SET,&err);
	OSFlagPend(FLAG_GRP,PollTaskEndFlag,OS_FLAG_WAIT_SET_ALL,0,&err);//等待巡检完成
	
	recv.Num = 0;
	ReceiveBuffer.RX = ReceiveBuffer.TX = 0;
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	HAL_NVIC_EnableIRQ(UART4_IRQn);
	
	Clean_Lcd(ScreenBuffer);
	while(1)
	{
		StringDisplay( "导入描述信息按返回键退出", 2, 24, 0, ScreenBuffer );
		
		while(ReceiveBuffer.RX != ReceiveBuffer.TX)
		{
			if(recv.Num+1 > 512) break;
			recv.Data[recv.Num] = ReceiveBuffer.Data[ReceiveBuffer.TX];
			recv.Num++;
			ReceiveBuffer.TX = (ReceiveBuffer.TX+1)&0x3ff;
		}
		
		while(1)
		{
			if(recv.Num < DESCRIBE_DATA_MIN) break;
			if(SUCCESS != Parse_Describe(&recv, &loop, &addr, describe)) break;
			if(loop != read_loop)
			{
				//回路改变保存一次描述信息
				if(read_loop != 0xff)
				{
					SpiFlashEraseSector(DescriptionRecord_StartAddr/4096 + read_loop);
					SpiFlashWriteNoCheck(SPI_FLASH_BUF, read_loop*SPI_FLASH_SECTOR_SIZE+DescriptionRecord_StartAddr, 4096);
				}
				read_loop = loop;
				SpiFlashRead(SPI_FLASH_BUF, read_loop*SPI_FLASH_SECTOR_SIZE+DescriptionRecord_StartAddr, 4096);
			}
			memcpy(&SPI_FLASH_BUF[addr*16], describe, 16);
		}
		
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,DESCRIBE_CHECK_CYC,&err);//等待按键
		if( err == OS_ERR_NONE && *My_Key_Message == Key_Return)
		{
			//退出之前保存一次描述信息
			if(read_loop != 0xff)
			{
				SpiFlashEraseSector(DescriptionRecord_StartAddr/4096 + read_loop);
				SpiFlashWriteNoCheck(SPI_FLASH_BUF, read_loop*SPI_FLASH_SECTOR_SIZE+DescriptionRecord_StartAddr, 4096);
			}
			break;
		}
	}
	
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	HAL_NVIC_DisableIRQ(UART4_IRQn);
	
	OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_CLR,&err);
	
	return 0;
}

static uint8_t Parse_Describe(TypeDef_Recv_Data_t *recv, uint8_t *loop, uint8_t *addr, uint8_t *describe)
{
	uint8_t found = 0;
	uint16_t i = 0, j = 0;
	
	if(recv->Num < DESCRIBE_DATA_MIN) return ERROR;
	found = 0;
	for(i=0; i<recv->Num-1; i++)
	{
		if(recv->Data[i] == '*' && recv->Data[i+1] == '*')
		{
			found = 1;
			break;
		}
	}
	if(found != 1) return ERROR;
	if(i != 0) memcpy(recv->Data, &recv->Data[i], recv->Num-i);
	recv->Num -= i;
	
	if(recv->Num < DESCRIBE_DATA_MIN) return ERROR;
	found = 0;
	for(i=0; i<recv->Num-1; i++)
	{
		if(recv->Data[i] == '#' && recv->Data[i+1] == '#')
		{
			recv->Data[i] = '\0';
			found = 1;
			break;
		}
	}
	if(found != 1) return ERROR;
	
	for(j=0; j<i; j++)
	{
		UART4->DR = recv->Data[j];
		while(!(UART4->SR&USART_SR_TXE));
	}
	UART4->DR = '\r';
	while(!(UART4->SR&USART_SR_TXE));
	UART4->DR = '\n';
	while(!(UART4->SR&USART_SR_TXE));
	
	*loop = recv->Data[4] - '0' - 1;
	*addr = atoi((const char*)&recv->Data[6]) - 1;
	memcpy(describe, &recv->Data[10], 16);
	
	memcpy(recv->Data, &recv->Data[i+2], recv->Num-(i+2));
	recv->Num -= (i+2);
	if(*loop>4)
		return ERROR;
		
	return SUCCESS;
}
