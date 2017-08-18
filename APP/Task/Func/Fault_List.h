#ifndef __FAULT_LIST_H
#define __FAULT_LIST_H

#include "includes.h"

typedef struct Fault_Device_List_struct
{
	uint8_t Update;//更新故障时标记当前存在的故障
	uint8_t Event_Type;
	uint8_t CardAddress;
	uint8_t DetectorAddress;
	uint8_t DetectorChannel;
	uint8_t Fault_Type;
	uint16_t Fault_Type_Dsc;
	RecordTime_struct time;
	struct Fault_Device_List_struct *prev;
	struct Fault_Device_List_struct *next;
}Fault_Device_List_struct;

extern Fault_Device_List_struct *Fault_Device_Tail, *Precedence_Fault_Device_Tail;
extern uint16_t Fault_Display_Index, Precedence_Fault_Display_Index;
extern uint16_t Fault_Total_Number,Precedence_Fault_Total_Number;	//故障总数

FlagStatus DeleteOneFault( Fault_Device_List_struct **List_Tail,Fault_Device_List_struct *DeleteNode,uint16_t *Total_Number );
FlagStatus AddOneFault( Fault_Device_List_struct **List_Tail,Record_struct *Record, uint16_t *Total_Number );
void Remove_All_Detector_Fult( uint8_t Card_ID );
void Remove_OtherCardFault( uint8_t Card_ID,uint8_t FaultType );
void Remove_CardFault( uint8_t Card_ID );

static inline void INIT_LIST_HEAD(struct Fault_Device_List_struct *list)
{
    list->next = list;
    list->prev = list;
}
static inline void __list_add(struct Fault_Device_List_struct *new,
                  struct Fault_Device_List_struct *prev,
                  struct Fault_Device_List_struct *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void __list_del(struct Fault_Device_List_struct * prev, struct Fault_Device_List_struct * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_add_tail(struct Fault_Device_List_struct *new, struct Fault_Device_List_struct *head)
{
    __list_add(new, head, head->next);
}

static inline void list_del(struct Fault_Device_List_struct *entry)
{
    __list_del(entry->prev, entry->next);
}

#endif
