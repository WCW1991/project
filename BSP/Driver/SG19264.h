#ifndef  __SG19264_H__
#define  __SG19264_H__
#include "stm32f1xx_hal.h"
#include <DS1302.h>

//Ô­½Ó¿ÚÌæ»»
#define String_Display(SetCode, StartPage, StartAddr, length, Reverse) StringDisplay(SetCode, StartPage, StartAddr, Reverse, ScreenBuffer)
typedef uint8_t ScreenBuffS[8][192];

void Init_Lcd( void );
void Clean_Lcd( ScreenBuffS ScreenBuffer );
void Set_Lcd( ScreenBuffS ScreenBuffer );
void BufferToLCD( ScreenBuffS ScreenBuffer );
void StringDisplay( uint8_t *SetCode, uint8_t StartPage, uint8_t StartAddr, uint8_t Reverse, ScreenBuffS ScreenBuffer );
void DisplayValue( uint16_t Value, uint8_t StartPage, uint8_t StartAddr, uint8_t Reverse, ScreenBuffS ScreenBuffer );
void DisplayValueN( uint16_t Value, uint8_t StartPage, uint8_t StartAddr, uint8_t Reverse, uint8_t WeiNum, ScreenBuffS ScreenBuffer );
void Bmp_Display( uint8_t* Buffer, uint8_t StartPage, uint8_t StartAddr, uint8_t High, uint8_t Wide, ScreenBuffS ScreenBuffer );
void DisplaySystime( SYSTEMTIME Systime, uint8_t StartPage, ScreenBuffS ScreenBuffer );
void TurnOn_Backlight(void);
void TurnOff_Backlight(void);
#endif