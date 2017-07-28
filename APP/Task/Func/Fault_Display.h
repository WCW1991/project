#ifndef __FAULT_DISPLAY_H
#define __FAULT_DISPLAY_H

#include "includes.h"

typedef struct
{
	uint8_t Event_Type;
	uint8_t CardAddress;
	uint8_t DetectorAddress;
	uint8_t DetectorChannel;
	uint8_t Fault_Type;
	uint16_t Fault_Type_Dsc;
	RecordTime_struct time;
}TypeDef_Fault_Display_t;

void display_fault(uint16_t index, TypeDef_Fault_Display_t *fault, uint8_t page);

#endif
