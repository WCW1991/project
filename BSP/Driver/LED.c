#include "LED.h"
#include "bsp_periph.h"
#include "MyLib.h"
#include "os_cpu.h"
static uint16_t Now_Stat=0;
#define	SysBreakLED			PBout(12)
#define	SysBreakLEDRead		PBin(12)
#define	SysBreakLED_ON	SysBreakLED=1
#define	SysBreakLED_OFF	SysBreakLED=0

#define LED595SRCK	PBout(11)
#define LED595DI	PEout(15)
#define LED595RCK	PBout(10)
/* 12λ����д��595 */
static void Output_Led( uint16_t Light_12 )//�ߵ�ƽ��
{
	OS_ENTER_CRITICAL()
	for( uint8_t i=0; i<11; i++){
		if( Light_12&(((uint16_t)0x01u)<<i) )
			LED595DI = 1;
		else
			LED595DI = 0;
		Delay_NOP( 1 );
		LED595SRCK = 1;
		Delay_NOP( 1 );
		LED595SRCK = 0;
	}
	Delay_NOP( 1 );
	LED595RCK = 1;
	Delay_NOP( 1 );
	LED595RCK = 0;
	OS_EXIT_CRITICAL()
	Now_Stat = Now_Stat;
}

/*
********************************************************************************
* ��������	����������LED
* ����		��
********************************************************************************
*/
void LED_All_On( void )
{
	Now_Stat = 0xffffu;
	Output_Led( Now_Stat );
	SysBreakLED_ON;
}
/*
********************************************************************************
* ��������	��Ϩ������LED
* ����		��
********************************************************************************
*/
void LED_All_Off( void )
{
	Now_Stat = 0x0000u;
	Output_Led( Now_Stat );
	SysBreakLED_OFF;
}
/*
********************************************************************************
* ��������	������ָ��LED
* ����		��
*	LEDx	��LED_enumö������
********************************************************************************
*/
void LED_On( LED_enum LEDx )
{
	if(LEDx==SysBreak_LED8){
		SysBreakLED_ON;
	}else{
		if( Now_Stat&(uint16_t)LEDx )
			return;
		else
			Now_Stat |= (uint16_t)LEDx;
		Output_Led( Now_Stat );
	}
}
/*
********************************************************************************
* ��������	��Ϩ��ָ��LED
* ����		��
*	LEDx	��LED_enumö������
********************************************************************************
*/
void LED_Off( LED_enum LEDx )
{
	if(LEDx==SysBreak_LED8){
		SysBreakLED_OFF;
	}else{
		if( !(Now_Stat&(uint16_t)LEDx) )
			return;
		else
			Now_Stat &= ~(uint16_t)LEDx;
		Output_Led( Now_Stat );
	}
}
/*
********************************************************************************
* ��������	����תָ��LED
* ����		��
*	LEDx	��LED_enumö������
********************************************************************************
*/
void LED_Toggle( LED_enum LEDx )
{
	if(LEDx==SysBreak_LED8){
		SysBreakLED=!SysBreakLEDRead;
	}else{
		if( Now_Stat&(uint16_t)LEDx )
			Now_Stat &= ~(uint16_t)LEDx;
		else
			Now_Stat |= (uint16_t)LEDx;
		Output_Led( Now_Stat );
	}
}