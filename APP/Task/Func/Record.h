#ifndef __RECORD_H
#define __RECORD_H

#include <includes.h>

typedef enum{
	OtherArea,
	FireArea
}StoreArea_enum;

void Store_Manage( Record_struct *Record, StoreArea_enum StoreArea );

#endif
