#include "Config.h"
/*
*********************************************************************************************************
*
* 文件名    : Config.c
* 版本号    : V1.00
* 作  者    : j
*             
*
* 版权说明  : 成都安吉斯智能设备有限公司
* 文件描述  : 实现配置读取存储
* 开发环境  : 
* 修改日期  : 2017/03/22
*********************************************************************************************************
*/

void config_init(void)
{
	uint16_t crc16;
	Config_Store p;
	
	SpiFlashRead((uint8_t *)&p, CfgRecord_StartAddr, sizeof(Config_Store));
	crc16 = crc_16((uint8_t *)&p, sizeof(Config_TypeDef));
	if(crc16 != p.crc16||p.crc16==0xff)
	{
		for(uint8_t x=52;x<=58;x++){//擦除第53-59扇区
			SpiFlashEraseSector(x);
		}
		//配置信息不存在，初始化配置
		memset(Config.password, 1, 4);
		Config.card_state = 0;
		Config.print_state = 0;
		Config.machine_cfg.area_code=0;
		Config.machine_cfg.area_machine=0;
		config_store();
	}
	else
	{
		Config = p.config;
	}
}

void config_store(void)
{
	Config_Store p, temp;

	p.config = Config;
	p.crc16 = crc_16((uint8_t *)&p, sizeof(Config_TypeDef));

	do{
		SpiFlashWrite((uint8_t *)&p, CfgRecord_StartAddr, sizeof(Config_Store));
		SpiFlashRead((uint8_t *)&temp, CfgRecord_StartAddr, sizeof(Config_Store));
	}while(memcmp(&p, &temp, sizeof(Config_Store)) != 0);
}

void record_state_init(void)
{
	uint8_t i = 0, state = 0;
	uint16_t crc16;
	Record_State_Store p;
	
	for(i=0; i<3; i++)
	{
		SpiFlashRead((uint8_t *)&p, RecordState_StartAddr+i*0x1000, sizeof(Record_State_Store));
		crc16 = crc_16((uint8_t *)&p, sizeof(Record_State_TypeDef));
		if(crc16 == p.crc16)
		{
			Record_State = p.record_state;
			state = 1;
			break;
		}
	}
	
	if(state == 1)
	{
		Read1302RAM((uint8_t *)&p, RecordState_In_1302RAM, sizeof(Record_State_Store));
		crc16 = crc_16((uint8_t *)&p, sizeof(Record_State_TypeDef));
		if(crc16 != p.crc16)
		{
			record_state_store();
			record_state_store_ds1302();
		}
		else
		{
			Record_State = p.record_state;
			record_state_store();
		}
	}
	else
	{
		Record_State.fault.Index = 0;
		Record_State.fault.FullNumber = 0;
		Record_State.fire.Index = 0;
		Record_State.fire.FullNumber = 0;
		record_state_store();
		record_state_store_ds1302();
	}
}

void record_state_store(void)
{
	uint8_t i = 0;
	Record_State_Store p, temp;
	
	p.record_state = Record_State;
	p.crc16 = crc_16((uint8_t *)&p, sizeof(Record_State_TypeDef));
	
	for(i=0; i<3; i++)
	{
		do{
			SpiFlashWrite((uint8_t *)&p, RecordState_StartAddr+i*0x1000, sizeof(Record_State_Store));
			SpiFlashRead((uint8_t *)&temp, RecordState_StartAddr+i*0x1000, sizeof(Record_State_Store));
		}while(memcmp(&p, &temp, sizeof(Record_State_Store)) != 0);
	}
}

void record_state_store_ds1302(void)
{
	Record_State_Store p, temp;
	
	p.record_state = Record_State;
	p.crc16 = crc_16((uint8_t *)&p, sizeof(Record_State_TypeDef));
	
	do{
		Write1302RAM((uint8_t *)&p, RecordState_In_1302RAM, sizeof(Record_State_Store));
		Read1302RAM((uint8_t *)&temp, RecordState_In_1302RAM, sizeof(Record_State_Store));
	}while(memcmp(&p, &temp, sizeof(Record_State_Store)) != 0);
}
