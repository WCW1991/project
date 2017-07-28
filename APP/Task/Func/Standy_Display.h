#ifndef __STANDY_DISPLAY_H
#define __STANDY_DISPLAY_H

#include <includes.h>

typedef enum{
	StandyNomal,
	StandyFire,
	StandyFault
}DisplayType_enum;

extern DisplayType_enum DisplayType;

void Standy_Display( void );

#endif
