#ifndef __CHECK_RECORD_H
#define __CHECK_RECORD_H

#include <includes.h>

typedef enum{
	FireFault=0,
	OtherFault=!FireFault
}CheckRecordType_enum;

uint8_t CheckRecord( CheckRecordType_enum CheckRecordType );

#endif
