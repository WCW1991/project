#include "Driver.h"
#include "CPU_init.h"
void DriverInit(void)
{
	CPU_Init();
	/*��Ҫ��ʼ�����ⲿӲ��*/
	Initial_DS1302();
	InitPrinter();
	Init_Lcd();
}
