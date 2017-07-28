#include "bsp_periph.h"
#include "CPU_init.h"
#include "w25x40.h"
static void MX_GPIO_Init(void);
static void GPIOforExternal(void);
extern void OS_CPU_SysTickInit(void);
void CPU_Init (void)
{
	if(HAL_OK!=HAL_Init())while(1);
	SystemClock_Config();
	MX_TIM1_Init();
	tim6_init();
	MX_SPI1_Init();
	spiflah_init();
	MX_UART4_Init();
	MX_USART1_UART_Init();
	MX_GPIO_Init();
	GPIOforExternal();
	OS_CPU_SysTickInit();
}
/*
********************************************************************************
* 外部硬件用到的GPIO
*
********************************************************************************
*/
static void GPIOforExternal(void)
{
	/* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
	/*初始化DS1302 IO*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = IIC_1302_CLK_Pin|IIC_1302_IO_Pin|IIC_1302_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*打印机*/
	GPIO_InitStruct.Pin = D7_Pin|D6_Pin|D5_Pin|D4_Pin|D3_Pin|D2_Pin|D1_Pin|D0_Pin|PRN_STB_Pin|GPIO_PIN_3;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = PRNBUSY_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	/*按键*/
	GPIO_InitStruct.Pin = KEY_PL_Pin|KEY_CP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOE, KEY_PL_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, KEY_CP_Pin, GPIO_PIN_SET);
	
	GPIO_InitStruct.Pin = KEY_DO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	/*LED*/
	GPIO_InitStruct.Pin = LED595DI_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED595RCK_Pin|LED595SRCK_Pin|SysBrLed_Pin;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin =LED595DI_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED10_Pin|LED595RCK_Pin|LED595SRCK_Pin|SysBrLed_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*LCD*/
	GPIO_InitStruct.Pin = LCD_CSB_Pin|LCD_CSA_Pin|LCD_E_Pin|LCD_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LCD_D7_Pin|LCD_D6_Pin|LCD_D5_Pin|LCD_D0_Pin|LCD_D1_Pin
						 |LCD_D2_Pin|LCD_D3_Pin|LCD_D4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_BG_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/*2总线*/
	GPIO_InitStruct.Pin = ADD_CODE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	PBout(14)=1;
	
	GPIO_InitStruct.Pin = ADD_IN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
        * Free pins are configured automatically as Analog (this feature is enabled through 
        * the Code Generation settings)
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PC13 PC14 PC15 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


  /*Configure GPIO pins : PA0 PA1 PA2 PA3 
                           PA4 PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB5 
                           PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD3 PD4 
                           PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PE0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}