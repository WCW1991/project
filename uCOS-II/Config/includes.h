/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           MASTER INCLUDES
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : includes.h
* Version       : v1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>

#include    <ucos_ii.h>

#include "stm32f1xx_hal.h"
#include "app_vect_v5.h"

#include "ca7000.h"

#include "Driver.h"

#include "Task_Get_Key.h"
#include "Task_GUI.h"
#include "Task_Polling.h"
#include "Task_RefreshScreen.h"
#include "Task_Status_Indicator.h"
#include "Task_Time_Display.h"

#include "Bujiantongji.h"
#include "CheckRecord.h"
#include "Common.h"
#include "Config.h"
#include "Describe.h"
#include "EnterMenu.h"
#include "Fault_Display.h"
#include "Fault_List.h"
#include "Input_Value.h"
#include "MainBianChen.h"
#include "ModifyAddr.h"
#include "Password.h"
#include "PrinterSet.h"
#include "Protocol_Bus.h"
#include "Record.h"
#include "Register.h"
#include "Sampling.h"
#include "SetThreshold.h"
#include "SetTime.h"
#include "Standy_Display.h"

#if (APP_CFG_LCD_EN > 0)
#include    <arm_comm.h>
#include    <drv_glcd_cnfg.h>
#include    <drv_glcd.h>
#include    <glcd_ll.h>
#include    <drv_spi1.h>
#endif

#endif