#include <yfuns.h>
#include <Print.h>
#include "bsp_periph.h"
#include "MyLib.h"
#define BUSY	PEin(1)
#define STB		PDout(2)
#define PE		PDin(3)

#define D0		PDout(15)
#define D1		PDout(14)
#define D2		PDout(13)
#define D3		PDout(12)
#define D4		PDout(11)
#define D5		PDout(10)
#define D6		PDout(9)
#define D7		PDout(8)
static ErrorStatus WriteAnByte(unsigned char byte_data)
{
	uint16_t timeout=0;
	while(BUSY){
		if(timeout++>1000)
			return ERROR;
	}
	if( byte_data&0x01u )D0 = 1;
	else D0 = 0;
	
	if( byte_data&0x02u )D1 = 1;
	else D1 = 0;
	
	if( byte_data&0x04u )D2 = 1;
	else D2 = 0;
	
	if( byte_data&0x08u )D3 = 1;
	else D3 = 0;
	
	if( byte_data&0x10u )D4 = 1;
	else D4 = 0;
	
	if( byte_data&0x20u )D5 = 1;
	else D5 = 0;
	
	if( byte_data&0x40u )D6 = 1;
	else D6 = 0;
	
	if( byte_data&0x80u )D7 = 1;
	else D7 = 0;
	
	STB=0;
	Delay_NOP( 10 );
	STB=1;
	return SUCCESS;
}
_STD_BEGIN
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  size_t nChars = 0;

  if (buffer == 0)
    return 0;

  /* This template only writes to "standard out" and "standard err",
   * for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size != 0; --size)
  {
	  if(ERROR==WriteAnByte(*(buffer++)))
		  return _LLIO_ERROR;
    ++nChars;
  }

  return nChars;

}
_STD_END

static void PrintByteN( unsigned char* data_src,unsigned char N)
{
	while( N--){
		WriteAnByte(*(data_src++));
	}
}
	
void InitPrinter( void )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x40);
}

void Choose_Set( Character_Set_enum Character_Set )
{
	switch(Character_Set){
	case Character_Set_1:{//·¢ËÍ0x1b36
		PrintByteN( (uint8_t*)Character_Set_1,2);
	}break;
	case Character_Set_2:{//·¢ËÍ0x1b37
		PrintByteN( (uint8_t*)Character_Set_2,2);
	}break;
	case HanZi_Set:{//·¢ËÍ0x1c26
		PrintByteN( (uint8_t*)HanZi_Set,2);
	}break;
	}
}

void Move_Paper( uint8_t point )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x4A);
	WriteAnByte(point);
}

void SetLineSpacing( uint8_t point )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x31);
	WriteAnByte(point);
}

void SetCharSpacing( uint8_t point )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x70);
	WriteAnByte(point);
}

void SetGrayLevel( uint8_t Level )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x6D);
	if(Level>12)
		Level=12;
	WriteAnByte(Level);
}

void SetZoomLevel_H( uint8_t Level )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x55);
	if(Level>8)
		Level=8;
	WriteAnByte(Level);
}

void SetZoomLevel_V( uint8_t Level )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x56);
	if(Level>8)
		Level=8;
	WriteAnByte(Level);
}

void SetZoomLevel( uint8_t Level )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x57);
	if(Level>8)
		Level=8;
	WriteAnByte(Level);
}

void AddUnderline( FlagStatus Underline )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x2D);
	WriteAnByte(Underline);
}

void PrintInverse( FlagStatus Inverse )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x69);
	WriteAnByte(Inverse);
}

void PrintInversion( FlagStatus Inversion )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x63);
	WriteAnByte(!Inversion);
}

void PrintSpacing( uint8_t N )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x66);
	WriteAnByte(0x00);
	WriteAnByte(N);
}

void SwitchNewline( void )
{
	WriteAnByte(0x0A);
}

void PrintNullLine( uint8_t N )
{
	WriteAnByte(0x1B);
	WriteAnByte(0x66);
	WriteAnByte(0x01);
	WriteAnByte(N);
}

FlagStatus GetPrinterState( PrinterState_enum PrinterState )
{
	uint16_t timeout=0;
	WriteAnByte(0x1C);
	WriteAnByte(0x76);
	WriteAnByte(PrinterState);
	while(BUSY)
		if(timeout++>60000)
			return RESET;
	if(PE){
		return SET;
	}else{
		return RESET;
	}
}
//void PrinterENorDIS(uint8_t E)
//{
//	if(E){
//		WriteAnByte(0x1B);
//		WriteAnByte(0x64);
//		WriteAnByte(0x01);
//	}else{
//		WriteAnByte(0x1B);
//		WriteAnByte(0x64);
//		WriteAnByte(0x00);
//	}
//}














