#include "Fault_List.h"
/*
*********************************************************************************************************
*
* 文件名    : Fault_List.c
* 版本号    : V1.00
* 作  者    : j
*             
*
* 版权说明  : 成都安吉斯智能设备有限公司
* 文件描述  : 实现事件链表操作
* 开发环境  : 
* 修改日期  : 2017/02/10
*********************************************************************************************************
*/

Fault_Device_List_struct *Fault_Device_Tail, *Precedence_Fault_Device_Tail;
uint16_t Fault_Display_Index, Precedence_Fault_Display_Index;
uint16_t Fault_Total_Number,Precedence_Fault_Total_Number;	//故障总数


/*******************************************************
 * 从当前链表中删除一个节点
 *
 * List_Tail：链表指针的指针（用于判断删除的是不是它本身）
 * 
 * DeleteNode：待删除的节点
 * 
 * Total_Number：故障数量是传的全局变量的指针，也就是取其内容会改变全局变量
 *******************************************************/
FlagStatus DeleteOneFault( Fault_Device_List_struct **List_Tail,Fault_Device_List_struct *DeleteNode,uint16_t *Total_Number )
{
	if((*List_Tail)==DeleteNode)(*List_Tail)=(*List_Tail)->prev;
	list_del(DeleteNode);					//删除
	free(DeleteNode);
	
	*Total_Number-=1;
	if(!(*Total_Number)){//如果删除的是最后一个故障
		(*List_Tail)=(Fault_Device_List_struct *)NULL;
	}
	
	if(*Total_Number)
		return SET;//可以继续删除
	else{
		return RESET;//不能再删除
	}
}

/*******************************************************
 * 从当前链表中增加一个节点
 *
 * List_Tail：链表指针的指针
 * 
 * Record：需要增加的节点信息的指针
 * 
 * Total_Number：故障数量是传的全局变量的指针，也就是取其内容会改变全局变量
 *******************************************************/
FlagStatus AddOneFault( Fault_Device_List_struct **List_Tail,Record_struct *Record, uint16_t *Total_Number )
{
	Fault_Device_List_struct *Fault_Device_node;

	Fault_Device_node = (Fault_Device_List_struct*)malloc(sizeof(Fault_Device_List_struct));
	if(Fault_Device_node == NULL) while(1);	

	Fault_Device_node->Update = 1;
	Fault_Device_node->CardAddress=Record->event.card_code;		//0～Card_Number-1
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
	/*普通链表中删除*/
	if(Fault_Device_Tail!=(Fault_Device_List_struct *)NULL){
		number = Fault_Total_Number;
		tmp=Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device=tmp->next;
			if(pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->DetectorAddress!=0){
				if(RESET==DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number)){
					break;/*不能继续删除*/
				}
			}
			else tmp = tmp->next;
		}
	}
#if (PROGRAM_TYPE == 1)
	/*优先级链表中删除*/
	if(Precedence_Fault_Device_Tail!=(Fault_Device_List_struct *)NULL){//200个探测器检查
		number = Precedence_Fault_Total_Number;
		tmp = Precedence_Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device=tmp->next;
			if(pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->DetectorAddress!=0){
				if(RESET==DeleteOneFault(&Precedence_Fault_Device_Tail,pTemp_Fault_Device,&Precedence_Fault_Total_Number)){
					break;/*不能继续删除*/
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
	/*普通链表中删除*/
	if(Fault_Device_Tail!=(Fault_Device_List_struct *)NULL){
		number = Fault_Total_Number;
		tmp=Fault_Device_Tail;
		for(i=0; i<number; i++)
		{
			pTemp_Fault_Device=tmp->next;
			if(pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->Fault_Type!=FaultType){
				if(RESET==DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number)){
					break;/*不能继续删除*/
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
			if( pTemp_Fault_Device->CardAddress==Card_ID&&pTemp_Fault_Device->Event_Type==0x0A ){		/*回路号相等*/
				if(RESET==DeleteOneFault(&Fault_Device_Tail,pTemp_Fault_Device,&Fault_Total_Number)){
					return;/*不能继续删除*/
				}
			}
			else tmp = tmp->next;
		}
	}
}
