#ifndef __LED_H  
#define __LED_H
#include "stm32f1xx_hal.h"
//Ô­½Ó¿ÚÌæ»»
#define BSP_LED_All_Off() LED_All_Off()
#define BSP_LED_All_On() LED_All_On()
#define BSP_LED_On(LEDx) LED_On(LEDx)
#define BSP_LED_Off(LEDx) LED_Off(LEDx)
#define BSP_LED_Toggle(LEDx) LED_Toggle(LEDx)
typedef enum 
{
	zhudian_LED1 = 0x0400,
	yunxing_LED2 = 0x0200,
	baojing_LED3 = 0x0100,
	guzhang_LED4 = 0x0080,
	beidian_LED5 = 0x0040,
#if (MAIN_BOARD == 1) && (PROGRAM_TYPE == 1)
	xiaoyin_LED6 = 0x0010,
	OutPut_LED7 = 0x0020,
#else
	xiaoyin_LED6 = 0x0020,
	OutPut_LED7 = 0x0010,
#endif
	SysBreak_LED8 = 0xffff,
	LED9 = 0x0008,
	LED10 = 0x0004,
	LED11 = 0x0002,
	LED12 = 0x0001
}LED_enum;

void LED_All_On( void );
void LED_All_Off( void );
void LED_On( LED_enum LEDx );
void LED_Off( LED_enum LEDx );
void LED_Toggle( LED_enum LEDx );

#endif