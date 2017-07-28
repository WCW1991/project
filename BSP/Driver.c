#include "Driver.h"
#include "CPU_init.h"
void DriverInit(void)
{
	CPU_Init();
	/*需要初始化的外部硬件*/
	Initial_DS1302();
	InitPrinter();
	Init_Lcd();
}
