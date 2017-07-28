#include "Fault_List.h"
/*
*********************************************************************************************************
*
* �ļ���    : Fault_List.c
* �汾��    : V1.00
* ��  ��    : j
*             
*
* ��Ȩ˵��  : �ɶ�����˹�����豸���޹�˾
* �ļ�����  : ʵ���¼��������
* ��������  : 
* �޸�����  : 2017/02/10
*********************************************************************************************************
*/

Fault_Device_List_struct *Fault_Device_Tail, *Precedence_Fault_Device_Tail;
uint16_t Fault_Display_Index, Precedence_Fault_Display_Index;
uint16_t Fault_Total_Number,Precedence_Fault_Total_Number;	//��������


/*******************************************************
 * �ӵ�ǰ������ɾ��һ���ڵ�
 *
 * List_Tail������ָ���ָ�루�����ж�ɾ�����ǲ���������
 * 
 * DeleteNode����ɾ���Ľڵ�
 * 
 * Total_Number�����������Ǵ���ȫ�ֱ�����ָ�룬Ҳ����ȡ�����ݻ�ı�ȫ�ֱ���
 *******************************************************/
FlagStatus DeleteOneFault( Fault_Device_List_struct **List_Tail,Fault_Device_List_struct *DeleteNode,uint16_t *Total_Number )
{
	if((*List_Tail)==DeleteNode)(*List_Tail)=(*List_Tail)->prev;
	list_del(DeleteNode);					//ɾ��
	free(DeleteNode);
	
	*Total_Number-=1;
	if(!(*Total_Number)){//���ɾ���������һ������
		(*List_Tail)=(Fault_Device_List_struct *)NULL;
	}
	
	if(*Total_Number)
		return SET;//���Լ���ɾ��
	else{
		return RESET;//������ɾ��
	}
}

/*******************************************************
 * �ӵ�ǰ����������һ���ڵ�
 *
 * List_Tail������ָ���ָ��
 * 
 * Record����Ҫ���ӵĽڵ���Ϣ��ָ��
 * 
 * Total_Number�����������Ǵ���ȫ�ֱ�����ָ�룬Ҳ����ȡ�����ݻ�ı�ȫ�ֱ���
 *******************************************************/
FlagStatus AddOneFault( Fault_Device_List_struct **List_Tail,Record_struct *Record, uint16_t *Total_Number )
{
	Fault_Device_List_struct *Fault_Device_node;

	Fault_Device_node = (Fault_Device_List_struct*)malloc(sizeof(Fault_Device_List_struct));
	if(Fault_Device_node == NULL) while(1);	

	Fault_Device_node->Update = 1;
	Fault_Device_node->CardAddress=Record->event.card_code;		//0��Card_Number-1
	Fault_Device_node->DetectorAddress=Record->event.address;	//0
	Fault_Device_node->time.year=systime.Year;
	Fault_Device_node->time.month=systime.Month;
	Fault_Device_node->time.day=systime.Day;
	Fault_Device_node->time.hour=systime.Hour;
	Fault_Device_node->time.minute=systime.Minute;
	Fault_Device_node->time.second=systime.Second;
	Fault_Device_node->DetectorChannel=Record->event.channel;
	Fault_Device_node->Event_Type=Record->event.event_type;
	Fault_Device_node->Fault_Type=Record->event.fault_type;
	Fault_Device_node->Fault_Type_Dsc=Record->event.description;
	(*Total_Number)++;
	
	if(*List_Tail == NULL)
	{
		*List_Tail = Fault_Device_node;
		INIT_LIST_HEAD(*List_Tail);
	}
	else
	{
	  list_add_tail(Fault_Device_node, *List_Tail);
	  *List_Tail = (*List_Tail)->next;
	}
	
	return SET;
}

void Remove_All_Detector_Fult( uint8_t Card_ID )
{
	//uint8_t DetectorAddr;
	uint16_t i = 0, number = 0;
	Fault_Device_List_struct *pTemp_Fault_Device, *tmp;
	/*��ͨ������ɾ��*/
	if(Fault_Device_Tail!=(Fault_Device_List_struct *)NULL){
		number = Fault_Total_Number;
		tmp=Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device=tmp->next;
			if(pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->DetectorAddress!=0){
				if(RESET==DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number)){
					break;/*���ܼ���ɾ��*/
				}
			}
			else tmp = tmp->next;
		}
	}
#if (PROGRAM_TYPE == 1)
	/*���ȼ�������ɾ��*/
	if(Precedence_Fault_Device_Tail!=(Fault_Device_List_struct *)NULL){//200��̽�������
		number = Precedence_Fault_Total_Number;
		tmp = Precedence_Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device=tmp->next;
			if(pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->DetectorAddress!=0){
				if(RESET==DeleteOneFault(&Precedence_Fault_Device_Tail,pTemp_Fault_Device,&Precedence_Fault_Total_Number)){
					break;/*���ܼ���ɾ��*/
				}
			}
			else tmp = tmp->next;
		}
	}
#endif
}

void Remove_OtherCardFault( uint8_t Card_ID,uint8_t FaultType )
{
	uint16_t i = 0, number = 0;
	Fault_Device_List_struct *pTemp_Fault_Device, *tmp;
	/*��ͨ������ɾ��*/
	if(Fault_Device_Tail!=(Fault_Device_List_struct *)NULL){
		number = Fault_Total_Number;
		tmp=Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device=tmp->next;
			if(pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->Fault_Type!=FaultType){
				if(RESET==DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number)){
					break;/*���ܼ���ɾ��*/
				}
			}
			else tmp = tmp->next;
		}
	}
}

void Remove_CardFault( uint8_t Card_ID )
{
	uint16_t i = 0, number = 0;
	Fault_Device_List_struct *pTemp_Fault_Device, *tmp;
	
	if(Fault_Device_Tail!=(Fault_Device_List_struct *)NULL){
		number = Fault_Total_Number;
		tmp=Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device=tmp->next;
			if( pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->Event_Type==0x0A ){		/*��·�����*/
				if(RESET==DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number)){
					return;/*���ܼ���ɾ��*/
				}
			}
			else tmp = tmp->next;
		}
	}
}
