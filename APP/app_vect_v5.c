#include "app_vect_v5.h"
#include "stm32f1xx_hal.h"
#include <ucos_ii.h>
#include "bsp_periph.h"
typedef  union {
    CPU_FNCT_VOID   Fnct;
    void           *Ptr;
} APP_INTVECT_ELEM;
#define  BSP_INT_SRC_NBR                                 60

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];

#pragma language=extended
#pragma segment="CSTACK"

static  void		BSP_IntHandler     (uint32_t  int_id);

static  void		BSP_IntHandlerDummy(void);

static  void       App_NMI_ISR        (void);

static  void       App_Fault_ISR      (void);

static  void       App_BusFault_ISR   (void);

static  void       App_UsageFault_ISR (void);

static  void       App_MemFault_ISR   (void);

static  void       App_Spurious_ISR   (void);

extern  void       __iar_program_start(void);


/*
*********************************************************************************************************
*                                  EXCEPTION / INTERRUPT VECTOR TABLE
*
* Note(s) : (1) The Cortex-M3 may have up to 256 external interrupts, which are the final entries in the
*               vector table.  The STM32 has 60 external interrupt vectors.
*********************************************************************************************************
*/

__root  const  APP_INTVECT_ELEM  __vector_table[] @ ".intvec" = {
    { .Ptr = (void *)__sfe( "CSTACK" )},                        /*  0, SP start value.                                  */
    __iar_program_start,                                        /*  1, PC start value.                                  */
    App_NMI_ISR,                                                /*  2, NMI.                                             */
    App_Fault_ISR,                                              /*  3, Hard Fault.                                      */
    App_MemFault_ISR,                                           /*  4, Memory Management.                               */
    App_BusFault_ISR,                                           /*  5, Bus Fault.                                       */
    App_UsageFault_ISR,                                         /*  6, Usage Fault.                                     */
    App_Spurious_ISR,                                           /*  7, Reserved.                                        */
    App_Spurious_ISR,                                           /*  8, Reserved.                                        */
    App_Spurious_ISR,                                           /*  9, Reserved.                                        */
    App_Spurious_ISR,                                           /* 10, Reserved.                                        */
    App_Spurious_ISR,                                           /* 11, SVCall.                                          */
    App_Spurious_ISR,                                           /* 12, Debug Monitor.                                   */
    App_Spurious_ISR,                                           /* 13, Reserved.                                        */
    OS_CPU_PendSVHandler,                                       /* 14, PendSV Handler.                                  */
    OS_CPU_SysTickHandler,                                      /* 15, uC/OS-II Tick ISR Handler.                       */

    BSP_IntHandlerWWDG,                                         /* 16, INTISR[  0]  Window Watchdog.                    */
    BSP_IntHandlerPVD,                                          /* 17, INTISR[  1]  PVD through EXTI Line Detection.    */
    BSP_IntHandlerTAMPER,                                       /* 18, INTISR[  2]  Tamper Interrupt.                   */
    BSP_IntHandlerRTC,                                          /* 19, INTISR[  3]  RTC Global Interrupt.               */
    BSP_IntHandlerFLASH,                                        /* 20, INTISR[  4]  FLASH Global Interrupt.             */
    BSP_IntHandlerRCC,                                          /* 21, INTISR[  5]  RCC Global Interrupt.               */
    BSP_IntHandlerEXTI0,                                        /* 22, INTISR[  6]  EXTI Line0 Interrupt.               */
    BSP_IntHandlerEXTI1,                                        /* 23, INTISR[  7]  EXTI Line1 Interrupt.               */
    BSP_IntHandlerEXTI2,                                        /* 24, INTISR[  8]  EXTI Line2 Interrupt.               */
    BSP_IntHandlerEXTI3,                                        /* 25, INTISR[  9]  EXTI Line3 Interrupt.               */
    BSP_IntHandlerEXTI4,                                        /* 26, INTISR[ 10]  EXTI Line4 Interrupt.               */
    BSP_IntHandlerDMA1_CH1,                                     /* 27, INTISR[ 11]  DMA Channel1 Global Interrupt.      */
    BSP_IntHandlerDMA1_CH2,                                     /* 28, INTISR[ 12]  DMA Channel2 Global Interrupt.      */
    BSP_IntHandlerDMA1_CH3,                                     /* 29, INTISR[ 13]  DMA Channel3 Global Interrupt.      */
    BSP_IntHandlerDMA1_CH4,                                     /* 30, INTISR[ 14]  DMA Channel4 Global Interrupt.      */
    BSP_IntHandlerDMA1_CH5,                                     /* 31, INTISR[ 15]  DMA Channel5 Global Interrupt.      */

    BSP_IntHandlerDMA1_CH6,                                     /* 32, INTISR[ 16]  DMA Channel6 Global Interrupt.      */
    BSP_IntHandlerDMA1_CH7,                                     /* 33, INTISR[ 17]  DMA Channel7 Global Interrupt.      */
    BSP_IntHandlerADC1_2,                                       /* 34, INTISR[ 18]  ADC1 & ADC2 Global Interrupt.       */
    BSP_IntHandlerUSB_HP_CAN_TX,                                /* 35, INTISR[ 19]  USB High Prio / CAN TX  Interrupts. */
    BSP_IntHandlerUSB_LP_CAN_RX0,                               /* 36, INTISR[ 20]  USB Low  Prio / CAN RX0 Interrupts. */
    BSP_IntHandlerCAN_RX1,                                      /* 37, INTISR[ 21]  CAN RX1 Interrupt.                  */
    BSP_IntHandlerCAN_SCE,                                      /* 38, INTISR[ 22]  CAN SCE Interrupt.                  */
    BSP_IntHandlerEXTI9_5,                                      /* 39, INTISR[ 23]  EXTI Line[9:5] Interrupt.           */
    BSP_IntHandlerTIM1_BRK,                                     /* 40, INTISR[ 24]  TIM1 Break  Interrupt.              */
    BSP_IntHandlerTIM1_UP,                                      /* 41, INTISR[ 25]  TIM1 Update Interrupt.              */
    BSP_IntHandlerTIM1_TRG_COM,                                 /* 42, INTISR[ 26]  TIM1 Trig & Commutation Interrupts. */
    BSP_IntHandlerTIM1_CC,                                      /* 43, INTISR[ 27]  TIM1 Capture Compare Interrupt.     */
    BSP_IntHandlerTIM2,                                         /* 44, INTISR[ 28]  TIM2 Global Interrupt.              */
    BSP_IntHandlerTIM3,                                         /* 45, INTISR[ 29]  TIM3 Global Interrupt.              */
    BSP_IntHandlerTIM4,                                         /* 46, INTISR[ 30]  TIM4 Global Interrupt.              */
    BSP_IntHandlerI2C1_EV,                                      /* 47, INTISR[ 31]  I2C1 Event  Interrupt.              */

    BSP_IntHandlerI2C1_ER,                                      /* 48, INTISR[ 32]  I2C1 Error  Interrupt.              */
    BSP_IntHandlerI2C2_EV,                                      /* 49, INTISR[ 33]  I2C2 Event  Interrupt.              */
    BSP_IntHandlerI2C2_ER,                                      /* 50, INTISR[ 34]  I2C2 Error  Interrupt.              */
    BSP_IntHandlerSPI1,                                         /* 51, INTISR[ 35]  SPI1 Global Interrupt.              */
    BSP_IntHandlerSPI2,                                         /* 52, INTISR[ 36]  SPI2 Global Interrupt.              */
    BSP_IntHandlerUSART1,                                       /* 53, INTISR[ 37]  USART1 Global Interrupt.            */
    BSP_IntHandlerUSART2,                                       /* 54, INTISR[ 38]  USART2 Global Interrupt.            */
    BSP_IntHandlerUSART3,                                       /* 55, INTISR[ 39]  USART3 Global Interrupt.            */
    BSP_IntHandlerEXTI15_10,                                    /* 56, INTISR[ 40]  EXTI Line [15:10] Interrupts.       */
    BSP_IntHandlerRTCAlarm,                                     /* 57, INTISR[ 41]  RTC Alarm EXT Line Interrupt.       */
    BSP_IntHandlerUSBWakeUp,                                    /* 58, INTISR[ 42]  USB Wakeup from Suspend EXTI Int.   */
    BSP_IntHandlerTIM8_BRK,                                     /* 59, INTISR[ 43]  TIM8 Break Interrupt.               */
    BSP_IntHandlerTIM8_UP,                                      /* 60, INTISR[ 44]  TIM8 Update Interrupt.              */
    BSP_IntHandlerTIM8_TRG_COM,                                 /* 61, INTISR[ 45]  TIM8 Trigg/Commutation Interrupts.  */
    BSP_IntHandlerTIM8_CC,                                      /* 62, INTISR[ 46]  TIM8 Capture Compare Interrupt.     */
    BSP_IntHandlerADC3,                                         /* 63, INTISR[ 47]  ADC3 Global Interrupt.              */

    BSP_IntHandlerFSMC,                                         /* 64, INTISR[ 48]  FSMC Global Interrupt.              */
    BSP_IntHandlerSDIO,                                         /* 65, INTISR[ 49]  SDIO Global Interrupt.              */
    BSP_IntHandlerTIM5,                                         /* 66, INTISR[ 50]  TIM5 Global Interrupt.              */
    BSP_IntHandlerSPI3,                                         /* 67, INTISR[ 51]  SPI3 Global Interrupt.              */
    BSP_IntHandlerUART4,                                        /* 68, INTISR[ 52]  UART4 Global Interrupt.             */
    BSP_IntHandlerUART5,                                        /* 69, INTISR[ 53]  UART5 Global Interrupt.             */
    BSP_IntHandlerTIM6,                                         /* 70, INTISR[ 54]  TIM6 Global Interrupt.              */
    BSP_IntHandlerTIM7,                                         /* 71, INTISR[ 55]  TIM7 Global Interrupt.              */
    BSP_IntHandlerDMA2_CH1,                                     /* 72, INTISR[ 56]  DMA2 Channel1 Global Interrupt.     */
    BSP_IntHandlerDMA2_CH2,                                     /* 73, INTISR[ 57]  DMA2 Channel2 Global Interrupt.     */
    BSP_IntHandlerDMA2_CH3,                                     /* 74, INTISR[ 58]  DMA2 Channel3 Global Interrupt.     */
    BSP_IntHandlerDMA2_CH4_5,                                   /* 75, INTISR[ 59]  DMA2 Channel4/5 Global Interrups.   */
};

/*
*********************************************************************************************************
*                                           __low_level_init()
*
* Description : Perform low-level initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : IAR startup code.
*
* Note(s)     : none.
*********************************************************************************************************
*/
#ifdef STM32_EXT_SRAM
#pragma location="ICODE"
__interwork int __low_level_init(void)
{

                                                                /* FSMC Bank1 NOR/SRAM3 is used for the STM3210E-EVAL   */
                                                                /* if another Bank is req'd, adjust the Reg Addrs       */

    *(volatile  CPU_INT32U  *)0x40021014 = 0x00000114;          /* Enable FSMC clock                                    */


    *(volatile  CPU_INT32U  *)0x40021018 = 0x000001E0;          /* Enable GPIOD, GPIOE, GPIOF and GPIOG clocks          */


                                                                /* --------------------- CFG GPIO --------------------- */
                                                                /* SRAM Data lines, NOE and NWE configuration           */
                                                                /* SRAM Address lines configuration                     */
                                                                /* NOE and NWE configuration                            */
                                                                /* NE3 configuration                                    */
                                                                /* NBL0, NBL1 configuration                             */
    *(volatile  CPU_INT32U  *)0x40011400 = 0x44BB44BB;
    *(volatile  CPU_INT32U  *)0x40011404 = 0xBBBBBBBB;

    *(volatile  CPU_INT32U  *)0x40011800 = 0xB44444BB;
    *(volatile  CPU_INT32U  *)0x40011804 = 0xBBBBBBBB;

    *(volatile  CPU_INT32U  *)0x40011C00 = 0x44BBBBBB;
    *(volatile  CPU_INT32U  *)0x40011C04 = 0xBBBB4444;

    *(volatile  CPU_INT32U  *)0x40012000 = 0x44BBBBBB;
    *(volatile  CPU_INT32U  *)0x40012004 = 0x44444B44;


                                                                /* --------------------- CFG FSMC --------------------- */
    *(volatile  CPU_INT32U  *)0xA0000010 = 0x00001011;          /* Enable FSMC Bank1_SRAM Bank                          */
    *(volatile  CPU_INT32U  *)0xA0000014 = 0x00000200;

    return (1);
}
#endif
//void  OS_CPU_PendSVHandler        (void)  { BSP_IntHandler(BSP_INT_ID_WWDG);            }
//void  OS_CPU_SysTickHandler       (void)  { HAL_IncTick();HAL_SYSTICK_IRQHandler();     }
void  BSP_IntHandlerWWDG          (void)  { BSP_IntHandler(BSP_INT_ID_WWDG);            }
void  BSP_IntHandlerPVD           (void)  { BSP_IntHandler(BSP_INT_ID_PVD);             }
void  BSP_IntHandlerTAMPER        (void)  { BSP_IntHandler(BSP_INT_ID_TAMPER);          }
void  BSP_IntHandlerRTC           (void)  { BSP_IntHandler(BSP_INT_ID_RTC);             }
void  BSP_IntHandlerFLASH         (void)  { BSP_IntHandler(BSP_INT_ID_FLASH);           }
void  BSP_IntHandlerRCC           (void)  { BSP_IntHandler(BSP_INT_ID_RCC);             }
void  BSP_IntHandlerEXTI0         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI0);           }
void  BSP_IntHandlerEXTI1         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI1);           }
void  BSP_IntHandlerEXTI2         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI2);           }
void  BSP_IntHandlerEXTI3         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI3);           }
void  BSP_IntHandlerEXTI4         (void)  { BSP_IntHandler(BSP_INT_ID_EXTI4);           }
void  BSP_IntHandlerDMA1_CH1      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH1);        }
void  BSP_IntHandlerDMA1_CH2      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH2);        }
void  BSP_IntHandlerDMA1_CH3      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH3);        }
void  BSP_IntHandlerDMA1_CH4      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH4);        }
void  BSP_IntHandlerDMA1_CH5      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH5);        }
void  BSP_IntHandlerDMA1_CH6      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH6);        }
void  BSP_IntHandlerDMA1_CH7      (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH7);        }
void  BSP_IntHandlerADC1_2        (void)  { BSP_IntHandler(BSP_INT_ID_ADC1_2);          }
void  BSP_IntHandlerUSB_HP_CAN_TX (void)  { BSP_IntHandler(BSP_INT_ID_USB_HP_CAN_TX);   }
void  BSP_IntHandlerUSB_LP_CAN_RX0(void)  { BSP_IntHandler(BSP_INT_ID_USB_LP_CAN_RX0);  }
void  BSP_IntHandlerCAN_RX1       (void)  { BSP_IntHandler(BSP_INT_ID_CAN_RX1);         }
void  BSP_IntHandlerCAN_SCE       (void)  { BSP_IntHandler(BSP_INT_ID_CAN_SCE);         }
void  BSP_IntHandlerEXTI9_5       (void)  { BSP_IntHandler(BSP_INT_ID_EXTI9_5);         }
void  BSP_IntHandlerTIM1_BRK      (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_BRK);        }
void  BSP_IntHandlerTIM1_UP       (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_UP);         }
void  BSP_IntHandlerTIM1_TRG_COM  (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_TRG_COM);    }
void  BSP_IntHandlerTIM1_CC       (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_CC);         }
void  BSP_IntHandlerTIM2          (void)  { BSP_IntHandler(BSP_INT_ID_TIM2);            }
void  BSP_IntHandlerTIM3          (void)  { BSP_IntHandler(BSP_INT_ID_TIM3);            }
void  BSP_IntHandlerTIM4          (void)  { BSP_IntHandler(BSP_INT_ID_TIM4);            }
void  BSP_IntHandlerI2C1_EV       (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_EV);         }
void  BSP_IntHandlerI2C1_ER       (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_ER);         }
void  BSP_IntHandlerI2C2_EV       (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_EV);         }
void  BSP_IntHandlerI2C2_ER       (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_ER);         }
void  BSP_IntHandlerSPI1          (void)  { BSP_IntHandler(BSP_INT_ID_SPI1);            }
void  BSP_IntHandlerSPI2          (void)  { BSP_IntHandler(BSP_INT_ID_SPI2);            }
void  BSP_IntHandlerUSART1        (void)  { BSP_IntHandler(BSP_INT_ID_USART1);          }
void  BSP_IntHandlerUSART2        (void)  { BSP_IntHandler(BSP_INT_ID_USART2);          }
void  BSP_IntHandlerUSART3        (void)  { BSP_IntHandler(BSP_INT_ID_USART3);          }
void  BSP_IntHandlerEXTI15_10     (void)  { BSP_IntHandler(BSP_INT_ID_EXTI15_10);       }
void  BSP_IntHandlerRTCAlarm      (void)  { BSP_IntHandler(BSP_INT_ID_RTCAlarm);        }
void  BSP_IntHandlerUSBWakeUp     (void)  { BSP_IntHandler(BSP_INT_ID_USBWakeUp);       }
void  BSP_IntHandlerTIM8_BRK      (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_BRK);        }
void  BSP_IntHandlerTIM8_UP       (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_UP);         }
void  BSP_IntHandlerTIM8_TRG_COM  (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_TRG_COM);    }
void  BSP_IntHandlerTIM8_CC       (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_CC);         }
void  BSP_IntHandlerADC3          (void)  { BSP_IntHandler(BSP_INT_ID_ADC3);            }
void  BSP_IntHandlerFSMC          (void)  { BSP_IntHandler(BSP_INT_ID_FSMC);            }
void  BSP_IntHandlerSDIO          (void)  { BSP_IntHandler(BSP_INT_ID_SDIO);            }
void  BSP_IntHandlerTIM5          (void)  { BSP_IntHandler(BSP_INT_ID_TIM5);            }
void  BSP_IntHandlerSPI3          (void)  { BSP_IntHandler(BSP_INT_ID_SPI3);            }
void  BSP_IntHandlerUART4         (void)  { BSP_IntHandler(BSP_INT_ID_UART4);           }
void  BSP_IntHandlerUART5         (void)  { BSP_IntHandler(BSP_INT_ID_UART5);           }
void  BSP_IntHandlerTIM6          (void)  { BSP_IntHandler(BSP_INT_ID_TIM6);            }
void  BSP_IntHandlerTIM7          (void)  { BSP_IntHandler(BSP_INT_ID_TIM7);            }
void  BSP_IntHandlerDMA2_CH1      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH1);        }
void  BSP_IntHandlerDMA2_CH2      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH2);        }
void  BSP_IntHandlerDMA2_CH3      (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH3);        }
void  BSP_IntHandlerDMA2_CH4_5    (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH4_5);      }
void  BSP_IntInit (void)
{
    uint32_t  int_id;


    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}
void  BSP_IntVectSet(uint32_t int_id,CPU_FNCT_VOID  isr)
{
#if (CPU_CFG_CRITICAL_METHOD == 3)
    uint32_t   cpu_sr;
#endif


    if (int_id < BSP_INT_SRC_NBR) {
        OS_ENTER_CRITICAL();
        BSP_IntVectTbl[int_id] = isr;
        OS_EXIT_CRITICAL();
    }
}
static  void  BSP_IntHandler (uint32_t  int_id)
{
#if (CPU_CFG_CRITICAL_METHOD == 3)
    uint32_t         cpu_sr;
#endif
    CPU_FNCT_VOID  isr;


    OS_ENTER_CRITICAL();                                       /* Tell uC/OS-II that we are starting an ISR            */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */
}
static  void  BSP_IntHandlerDummy (void)	{HAL_NVIC_SystemReset();}
static  void  App_NMI_ISR (void)			{if(RCC->CR&RCC_CR_HSERDY)HAL_NVIC_SystemReset();while(1){PBout(12)=1;}}
static  void  App_Fault_ISR (void)		{HAL_NVIC_SystemReset();}
static  void  App_BusFault_ISR (void)		{HAL_NVIC_SystemReset();}
static  void  App_UsageFault_ISR (void)	{HAL_NVIC_SystemReset();}
static  void  App_MemFault_ISR (void)		{HAL_NVIC_SystemReset();}
static  void  App_Spurious_ISR (void)		{HAL_NVIC_SystemReset();}