#include "Record.h"
/*
*********************************************************************************************************
*
* �ļ���    : Record.c
* �汾��    : V1.00
* ��  ��    : j
*             
*
* ��Ȩ˵��  : �ɶ�����˹�����豸���޹�˾
* �ļ�����  : ʵ���¼�ѭ����¼�Ͷ�ȡ
* ��������  : 
* �޸�����  : 2017/02/10
*********************************************************************************************************
*/



/*****************************************************************
 * �洢
 * Record�����洢�ļ�¼
 * Type���洢����ָ������������
 *		OtherArea���洢���������ϼ�¼��
 *		FireArea���洢���𾯹��ϼ�¼��
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
	
	if((record_state->Index%PageFault)==0){//�����ÿ�����������һ������
		if(record_state->Index>=TotalFault){
			/*---------------�洢����߽�----------------*/
			SpiFlashEraseSector(StartUpPage);
			record_state->Index=0;
			SpiFlashWrite((uint8_t*)Record, (record_state->Index)*((uint16_t)sizeof(Record_struct))+FlashAddr, (uint16_t)sizeof(Record_struct));//�洢��ǰ����
			
			record_state->Index=1;
			record_state->FullNumber=1;
			record_state_store_ds1302();
		}else{
			SpiFlashEraseSector(StartUpPage+record_state->Index/PageFault);//������һ������
			SpiFlashWrite((uint8_t*)Record, (record_state->Index)*((uint16_t)sizeof(Record_struct))+FlashAddr, (uint16_t)sizeof(Record_struct));//�洢��ǰ����
			
			record_state->Index++;/*0��4095*/
			if(record_state->FullNumber){//��δ��֮ǰֻ��Ϊ���δ��
				record_state->FullNumber++;//����֮����Ϊ��ǰ����ָʾ
				if(record_state->FullNumber>16)record_state->FullNumber=1;/*1��16*/
			}
			record_state_store_ds1302();
		}
	}else{//�����ĳ���������м�λ�û���ʼλ��
		SpiFlashWrite((uint8_t*)Record, (record_state->Index)*((uint16_t)sizeof(Record_struct))+FlashAddr, (uint16_t)sizeof(Record_struct));//�洢��ǰ����
		record_state->Index++;/*0��4095*/																				/*��*/
		record_state_store_ds1302();
	}
}
