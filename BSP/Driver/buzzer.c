#include "bsp_periph.h"
#include "buzzer.h"

void Buzzer_ON( void )
{
	uint32_t Temp;
	HAL_TIM_PWM_Start(&htim1, 10);
	Temp=TIM1->CCMR2;
	Temp=(Temp&~TIM_CCMR2_OC3M_Msk)|TIM_CCMR2_OC3M_2|TIM_CCMR2_OC3M_1;
	TIM1->CCMR2=Temp;
}

void Buzzer_OFF( void )
{
	uint32_t Temp;
	HAL_TIM_PWM_Stop(&htim1, 10);
	Temp=TIM1->CCMR2;
	Temp=(Temp&~TIM_CCMR2_OC3M_Msk)|TIM_CCMR2_OC3M_2;
	TIM1->CCMR2=Temp;
}