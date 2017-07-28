#ifndef __MYLIB_H
#define __MYLIB_H
static inline void Delay_NOP( uint16_t nS )
{
	for(uint16_t i=0;i<nS;i++)
		__no_operation();
}

static inline void Delay_mS( uint16_t mS )
{
	uint32_t N=0,i=0;
	N=mS*7200*2;
	for(i=0;i<N;i++)
		__no_operation();
}

/*
inline void INIT_LIST_HEAD(Record_List_struct *list)
{
    list->next = list;
    list->prev = list;
}

inline void __list_add(Record_List_struct *new,Record_List_struct *prev,Record_List_struct *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

inline void __list_del(Record_List_struct * prev,Record_List_struct * next)
{
    next->prev = prev;
    prev->next = next;
}

inline void list_add_tail(Record_List_struct *new,Record_List_struct *head)
{
    __list_add(new, head, head->next);
}

inline void list_del(Record_List_struct *entry)
{
    __list_del(entry->prev, entry->next);
}
*/
#endif