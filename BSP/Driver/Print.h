#ifndef _Print_H_
#define _Print_H_
#include "stm32f1xx_hal.h"
#include <stdio.h>
typedef enum{
	NoPaper		=0,
	FewPaper	=1,
	QieDao		=2
}PrinterState_enum;

typedef enum{
	Character_Set_1	=0x1B36,
	Character_Set_2	=0x1B37,
	HanZi_Set		=0x1C26
}Character_Set_enum;

void InitPrinter( void );
void Choose_Set( Character_Set_enum Character_Set );
void Move_Paper( uint8_t point );
void SetLineSpacing( uint8_t point );
void SetCharSpacing( uint8_t point );
void SetGrayLevel( uint8_t Level );
void SetZoomLevel_H( uint8_t Level );
void SetZoomLevel_V( uint8_t Level );
void SetZoomLevel( uint8_t Level );
void AddUnderline( FlagStatus Underline );
void PrintInverse( FlagStatus Inverse );
void PrintInversion( FlagStatus Inversion );
void PrintSpacing( uint8_t N );
void SwitchNewline( void );
void PrintNullLine( uint8_t N );
FlagStatus GetPrinterState( PrinterState_enum PrinterState );

#endif