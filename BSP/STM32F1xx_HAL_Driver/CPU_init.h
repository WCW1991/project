#ifndef __CPU_INIT_H
#define __CPU_INIT_H
#define LCD_D0_Pin GPIO_PIN_7
#define LCD_D0_GPIO_Port GPIOE
#define LCD_D1_Pin GPIO_PIN_8
#define LCD_D1_GPIO_Port GPIOE
#define LCD_D2_Pin GPIO_PIN_9
#define LCD_D2_GPIO_Port GPIOE
#define LCD_D3_Pin GPIO_PIN_10
#define LCD_D3_GPIO_Port GPIOE
#define LCD_D4_Pin GPIO_PIN_11
#define LCD_D4_GPIO_Port GPIOE
#define LCD_D5_Pin GPIO_PIN_6
#define LCD_D5_GPIO_Port GPIOE
#define LCD_D6_Pin GPIO_PIN_5
#define LCD_D6_GPIO_Port GPIOE
#define LCD_D7_Pin GPIO_PIN_4
#define LCD_D7_GPIO_Port GPIOE

#define LCD_CSA_Pin GPIO_PIN_7
#define LCD_CSA_GPIO_Port GPIOC
#define LCD_CSB_Pin GPIO_PIN_6
#define LCD_CSB_GPIO_Port GPIOC

#define LCD_RS_Pin GPIO_PIN_11
#define LCD_RS_GPIO_Port GPIOA

#define LCD_E_Pin GPIO_PIN_8
#define LCD_E_GPIO_Port GPIOC

#define LCD_RST_Pin GPIO_PIN_9
#define LCD_RST_GPIO_Port GPIOC

#define LCD_BG_Pin GPIO_PIN_12
#define LCD_BG_GPIO_Port GPIOA

#define SPI1_w25x40_CS_Pin GPIO_PIN_4
#define SPI1_w25x40_CS_GPIO_Port GPIOC
#define SPI1_w25x40_WH_Pin GPIO_PIN_5
#define SPI1_w25x40_WH_GPIO_Port GPIOC

#define IIC_1302_CLK_Pin GPIO_PIN_0
#define IIC_1302_CLK_GPIO_Port GPIOC
#define IIC_1302_IO_Pin GPIO_PIN_1
#define IIC_1302_IO_GPIO_Port GPIOC
#define IIC_1302_RST_Pin GPIO_PIN_2
#define IIC_1302_RST_GPIO_Port GPIOC

#define IIC_24LC64_SCL_Pin GPIO_PIN_2
#define IIC_24LC64_SCL_GPIO_Port GPIOE
#define IIC_24LC64_SDA_Pin GPIO_PIN_3
#define IIC_24LC64_SDA_GPIO_Port GPIOE

#define PRN_STB_Pin GPIO_PIN_2
#define PRN_STB_GPIO_Port GPIOD
#define PRNBUSY_Pin GPIO_PIN_1
#define PRNBUSY_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_8
#define D7_GPIO_Port GPIOD
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOD
#define D5_Pin GPIO_PIN_10
#define D5_GPIO_Port GPIOD
#define D4_Pin GPIO_PIN_11
#define D4_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_12
#define D3_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_13
#define D2_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_14
#define D1_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_15
#define D0_GPIO_Port GPIOD


#define KEY_PL_Pin GPIO_PIN_12
#define KEY_PL_GPIO_Port GPIOE
#define KEY_CP_Pin GPIO_PIN_13
#define KEY_CP_GPIO_Port GPIOE
#define KEY_DO_Pin GPIO_PIN_14
#define KEY_DO_GPIO_Port GPIOE

#define LED10_Pin GPIO_PIN_0
#define LED10_GPIO_Port GPIOB
#define LED595DI_Pin GPIO_PIN_15
#define LED595DI_GPIO_Port GPIOE
#define LED595RCK_Pin GPIO_PIN_10
#define LED595RCK_GPIO_Port GPIOB
#define LED595SRCK_Pin GPIO_PIN_11
#define LED595SRCK_GPIO_Port GPIOB
#define SysBrLed_Pin GPIO_PIN_12
#define SysBrLed_GPIO_Port GPIOB
#define ADD_IN_Pin GPIO_PIN_13
#define ADD_IN_GPIO_Port GPIOB
#define ADD_CODE_Pin GPIO_PIN_14
#define ADD_CODE_GPIO_Port GPIOB

#define MTXD1_Pin GPIO_PIN_9
#define MTXD1_GPIO_Port GPIOA
#define MRXD1_Pin GPIO_PIN_10
#define MRXD1_GPIO_Port GPIOA
#define MTXD2_Pin GPIO_PIN_10
#define MTXD2_GPIO_Port GPIOC
#define MRXD2_Pin GPIO_PIN_11
#define MRXD2_GPIO_Port GPIOC
void CPU_Init(void);

#endif