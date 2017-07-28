#ifndef _Usart_H_
#define _Usart_H_
#include "stm32f1xx_hal.h"
#include "bsp_periph.h"
#include "MyLib.h"

typedef enum
{
	Terminal1,
	Terminal2
}Terminal_enum;

typedef struct
{
	Terminal_enum	Terminal;
	uint16_t		DataNumber;
	uint8_t			Data[1024];
}Send_Receive_struct;

typedef struct{
	uint16_t RX;
	uint16_t TX;
	uint8_t Data[1024];
}TypeDef_Receive_t;

extern ErrorStatus OneByte;
extern TypeDef_Receive_t ReceiveBuffer;


void ISR_Uart1_Receive(void);
void ISR_Uart4_Receive(void);

#endif