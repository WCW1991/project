#include <Usart.h>

TypeDef_Receive_t ReceiveBuffer;

ErrorStatus OneByte=ERROR;

void ISR_Uart1_Receive(void)
{
	if(((ReceiveBuffer.RX+1)&0x3ff) == ReceiveBuffer.TX){
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		ReceiveBuffer.RX = ReceiveBuffer.TX = 0;
		OneByte=ERROR;
	}else{
		ReceiveBuffer.Data[ReceiveBuffer.RX] = USART1->DR;
		ReceiveBuffer.RX = (ReceiveBuffer.RX+1)&0x3ff;
		OneByte=SUCCESS;
		USART1->SR &= ~USART_SR_RXNE;
	}
}

void ISR_Uart4_Receive(void)
{
	if(((ReceiveBuffer.RX+1)&0x3ff) == ReceiveBuffer.TX){
		HAL_NVIC_DisableIRQ(UART4_IRQn);
		ReceiveBuffer.RX = ReceiveBuffer.TX = 0;
		OneByte=ERROR;
	}else{
		ReceiveBuffer.Data[ReceiveBuffer.RX] = UART4->DR;
		ReceiveBuffer.RX = (ReceiveBuffer.RX+1)&0x3ff;
		OneByte=SUCCESS;
		UART4->SR &= ~USART_SR_RXNE;
	}
}