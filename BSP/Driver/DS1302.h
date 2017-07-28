#ifndef _REAL_TIMER_DS1302_2003_7_21_
#define _REAL_TIMER_DS1302_2003_7_21_
#include "stm32f1xx_hal.h"

typedef struct
{
	unsigned char Year;
	unsigned char Month;
	unsigned char Day;
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second;
}SYSTEMTIME; //定义的时间类型

void Initial_DS1302(void);
void DS1302_GetTime(SYSTEMTIME *Time);
void DS1302_SetTime(SYSTEMTIME time);
void Write1302RAM(unsigned char *Buffer,unsigned char Addr,unsigned char N);
void Read1302RAM(unsigned char *Buffer,unsigned char Addr,unsigned char N);

#endif