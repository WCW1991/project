#include "ca7000.h"

CardState_struct CardState;

ScreenBuffS ScreenBuffer;

SYSTEMTIME systime;

Send_Receive_struct Send_Lv1, Send_Lv2;
Send_Receive_struct Send_Menu;

uint8_t Standy_State = 0x01;	//��¼һСʱ���Ƿ��в��� 0x01�в��� 0x02�޲���

Config_TypeDef Config;
Record_State_TypeDef Record_State;
