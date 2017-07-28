#include "SetTime.h"

uint8_t SetTime( void )
{
	Key_enum key;
	SYSTEMTIME systime2=systime;
	enum{
		Year_Cursor=0,
		Month_Cursor,
		Day_Cursor,
		Hour_Cursor,
		Minute_Cursor,
	}Cursor=Year_Cursor;
	
	Clean_Lcd(ScreenBuffer);
	for(;;){
		StringDisplay( "ʱ������", 0, 0, 0, ScreenBuffer );
		/* �� */
		DisplayValueN(20 , 2, 36, 0, 2, ScreenBuffer);
		DisplayValueN(systime.Year , 2, 48, 1, 2, ScreenBuffer);
		//Display_Number( 12, 3, 60,0 );// -
		StringDisplay("-", 2, 60, 0, ScreenBuffer);
		
		/* �� */
		DisplayValueN( systime.Month , 2, 66, 0, 2, ScreenBuffer);
		//Display_Number( 12, 3, 78,0 );// -
		StringDisplay("-", 2, 78, 0, ScreenBuffer);
		
		/* �� */
		DisplayValueN( systime.Day , 2, 84, 0, 2, ScreenBuffer);
		
		/* ʱ */
		DisplayValueN( systime.Hour , 2, 102, 0, 2, ScreenBuffer);
		//Display_Number( 11, 3, 114,0 );// :
		StringDisplay(":", 2, 114, 0, ScreenBuffer);
		
		/* �� */
		DisplayValueN( systime.Minute , 2, 120, 0, 2, ScreenBuffer);
#if MAIN_BOARD==1
		StringDisplay( "�����л�����", 0, 120, 0, ScreenBuffer );
		StringDisplay( "�����ü�����", 6, 0, 0, ScreenBuffer );
#endif
#if MAIN_BOARD==2
		StringDisplay( "��ȷ�ϼ�����", 6, 0, 0, ScreenBuffer );
#endif
		
		DisplayValueN( systime2.Year , 2, 48, 0, 2, ScreenBuffer);
		DisplayValueN( systime2.Month , 2, 66, 0, 2, ScreenBuffer);
		DisplayValueN( systime2.Day , 2, 84, 0, 2, ScreenBuffer);
		DisplayValueN( systime2.Hour , 2, 102, 0, 2, ScreenBuffer);
		DisplayValueN( systime2.Minute , 2, 120, 0, 2, ScreenBuffer);
		
		switch(Cursor){
			case Year_Cursor: key = input_value8(&systime2.Year, 2, 2, 48); break;
			case Month_Cursor: key = input_value8(&systime2.Month, 2, 2, 66); break;
			case Day_Cursor: key = input_value8(&systime2.Day, 2, 2, 84); break;
			case Hour_Cursor: key = input_value8(&systime2.Hour, 2, 2,102); break;
			case Minute_Cursor: key = input_value8(&systime2.Minute, 2, 2, 120); break;
		}
		switch(key)
		{
#if MAIN_BOARD==1
			case Key_x:{
				switch(Cursor){
					case Year_Cursor: Cursor=Month_Cursor; break;
					case Month_Cursor: Cursor=Day_Cursor; break;
					case Day_Cursor: Cursor=Hour_Cursor; break;
					case Hour_Cursor: Cursor=Minute_Cursor; break;
					case Minute_Cursor: Cursor=Year_Cursor; break;
				}
			}break;
#endif
#if MAIN_BOARD==2
			case Key_Right:{
				switch(Cursor){
					case Year_Cursor: Cursor=Month_Cursor; break;
					case Month_Cursor: Cursor=Day_Cursor; break;
					case Day_Cursor: Cursor=Hour_Cursor; break;
					case Hour_Cursor: Cursor=Minute_Cursor; break;
					case Minute_Cursor: Cursor=Year_Cursor; break;
				}
			}break;
			case Key_Lift:{
				switch(Cursor){
					case Year_Cursor	: Cursor=Minute_Cursor; break;
					case Month_Cursor	: Cursor=Year_Cursor; break;
					case Day_Cursor		: Cursor=Month_Cursor; break;
					case Hour_Cursor	: Cursor=Day_Cursor; break;
					case Minute_Cursor	: Cursor=Hour_Cursor; break;
				}
			}break;
#endif
			case Key_Set:{DS1302_SetTime(systime2);return 0;}break;
			case Key_Return:return 0;
			case Key_None: return 1;
		}
	}
}
