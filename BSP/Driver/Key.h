#ifndef __KEY_H
#define __KEY_H
#include "stm32f1xx_hal.h"
#include "ca7000.h"
#if MAIN_BOARD==1
typedef enum 
{
	Key_None = 0,
	Key_Unused = 0xffff,
	Key_1 = 0x0100,
	Key_2 = 0x0200,
	Key_3 = 0x0400,
	Key_Reset = 0x0800,
	Key_4 = 0x1000,
	Key_5 = 0x2000,
	Key_6 = 0x4000,
	Key_XiaoYin = 0x8000,
	Key_7 = 0x0001,
	Key_8 = 0x0002,
	Key_9 = 0x0004,
	Key_Delete = 0x0008,
	Key_x = 0x0010,
	Key_0 = 0x0020,
	Key_Return = 0x0040,
	Key_Set = 0x0080,
}Key_enum;
#endif
#if MAIN_BOARD==2
typedef enum 
{
	Key_None = 0,
	Key_Unused  = 0xffffffff,
	Key_1		= 0x01000000,
	Key_2		= 0x02000000,
	Key_3		= 0x04000000,
	Key_4		= 0x08000000,
	Key_5		= 0x10000000,
	Key_6		= 0x20000000,
	Key_7		= 0x40000000,
	Key_8		= 0x80000000,
	Key_9		= 0x00010000,
	Key_x		= 0x00020000,
	Key_0		= 0x00040000,
	Key_j		= 0x00080000,
	Key_Up		= 0x00100000,
	Key_Down	= 0x00200000,
	Key_Lift	= 0x00400000,
	Key_Right	= 0x00800000,
	Key_Set		= 0x00000100,
	Key_Upage	= 0x00000200,
	Key_Return	= 0x00000400,
	Key_Dpage	= 0x00000800,
	Key_Delete	= 0x00001000,
	Key_F1		= 0x00002000,
	Key_F2		= 0x00004000,
	Key_F3		= 0x00008000,
	Key_F4		= 0x00000001,
	Key_XiaoYin	= 0x00000002,
	Key_Reset	= 0x00000004,
	Key_Print	= 0x00000008,
}Key_enum;
#endif


Key_enum Geg_Key(void);
#endif