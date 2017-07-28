#include "Record.h"
/*
*********************************************************************************************************
*
* 文件名    : Record.c
* 版本号    : V1.00
* 作  者    : j
*             
*
* 版权说明  : 成都安吉斯智能设备有限公司
* 文件描述  : 实现事件循环记录和读取
* 开发环境  : 
* 修改日期  : 2017/02/10
*********************************************************************************************************
*/



/*****************************************************************
 * 存储
 * Record：待存储的记录
 * Type：存储区域指定，它可以是
 *		OtherArea：存储到其它故障记录区
 *		FireArea：存储到火警故障记录区
 *****************************************************************/
void Store_Manage( Record_struct *Record, StoreArea_enum StoreArea )
{
	uint16_t StartUpPage=0;
	uint32_t FlashAddr=0;
	Record_State_struct *record_state;
	
	if(StoreArea==FireArea){
		FlashAddr=FireRecord_StartAddr;
		StartUpPage=FireRecord_StartAddr/4096;
		record_state = &(Record_State.fire);
	}else{
		FlashAddr=FaultRecord_Start;
		StartUpPage=FaultRecord_Start/4096;
		record_state = &(Record_State.fault);
	}	
	
	if((record_state->Index%PageFault)==0){//如果是每个扇区的最后一个数据
		if(record_state->Index>=TotalFault){
			/*---------------存储区域边界----------------*/
			SpiFlashEraseSector(StartUpPage);
			record_state->Index=0;
			SpiFlashWrite((uint8_t*)Record, (record_state->Index)*((uint16_t)sizeof(Record_struct))+FlashAddr, (uint16_t)sizeof(Record_struct));//存储当前故障
			
			record_state->Index=1;
			record_state->FullNumber=1;
			record_state_store_ds1302();
		}else{
			SpiFlashEraseSector(StartUpPage+record_state->Index/PageFault);//擦除下一个扇区
			SpiFlashWrite((uint8_t*)Record, (record_state->Index)*((uint16_t)sizeof(Record_struct))+FlashAddr, (uint16_t)sizeof(Record_struct));//存储当前故障
			
			record_state->Index++;/*0～4095*/
			if(record_state->FullNumber){//当未满之前只作为标记未满
				record_state->FullNumber++;//当满之后作为当前扇区指示
				if(record_state->FullNumber>16)record_state->FullNumber=1;/*1～16*/
			}
			record_state_store_ds1302();
		}
	}else{//如果是某个扇区的中间位置或起始位置
		SpiFlashWrite((uint8_t*)Record, (record_state->Index)*((uint16_t)sizeof(Record_struct))+FlashAddr, (uint16_t)sizeof(Record_struct));//存储当前故障
		record_state->Index++;/*0～4095*/																				/*改*/
		record_state_store_ds1302();
	}
}
