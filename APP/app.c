#include <includes.h>

static  OS_STK         App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK         TaskGUIStk[Task_GUI_STK_SIZE];
static  OS_STK         Task_Status_Indicator_Stk[Task_Status_Indicator_STK_SIZE];
static  OS_STK         Task_Get_Key_Stk[Task_Get_Key_STK_SIZE];
static  OS_STK         Task_Time_Display_Stk[Task_Time_Display_STK_SIZE];
static  OS_STK         Task_Polling_Stk[Task_Polling_STK_SIZE];
static  OS_STK         Task_RefreshScreen_Stk[Task_RefreshScreen_STK_SIZE];

OS_EVENT *Status_Ack_Message;
OS_EVENT *Key_Mbox_Handle;
OS_EVENT *FuWei_Handle;
OS_EVENT *UsartDevice;
OS_EVENT *W25X40_Device;
OS_EVENT *DS1302_Device;

OS_FLAG_GRP *FLAG_GRP;//标志组

/**********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
**********************************************************************************************************/
static	void	App_TaskStart(void	*p_arg);
static	void	Self_Test_Init(void);
static	void	Reset(void);
static	void	App_EventCreate(void);
static	void	App_TaskCreate(void);

int  main (void)
{
    uint8_t  err;
	
    __disable_interrupt();//关闭全局中断                       	/* Disable all interrupts until we are ready to accept them */
	
    OSInit();                                                  	/* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreateExt((void (*)(void *)) App_TaskStart,        	/* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&App_TaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_SIZE > 5)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);
#endif
	
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}

static  void  App_TaskStart (void *p_arg)
{
    uint8_t   err;
	Key_enum *My_Key_Message;
	
    (void)p_arg;                                                /* See Note #1                                              */

	DriverInit();
	BSP_IntInit ();
	tim6_init();
	BSP_IntVectSet(BSP_INT_ID_USART1,ISR_Uart1_Receive);
	BSP_IntVectSet(BSP_INT_ID_UART4,ISR_Uart4_Receive);
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif
	
	App_EventCreate();                                         	/* Create application events                                */
	Self_Test_Init();
    App_TaskCreate();                                          	/* Create application tasks                                 */

	OSFlagPost(FLAG_GRP,DisplayDateTimeFlag|YunXingFlag|ZhuDianFlag,OS_FLAG_SET,&err);
//	for(uint8_t x=53;x<=57;x++){//擦除第53-62扇区
//		SpiFlashEraseSector(x);
//	}
    while (1) {
		My_Key_Message = OSMboxPend(FuWei_Handle,300,&err);//等待按键
		if(*My_Key_Message==Key_Reset)Reset();
    }
}

static  void  Self_Test_Init(void)
{
	uint8_t i = 0;
	
	Clean_Lcd(ScreenBuffer);
	
	OSTimeDlyHMSM(0, 0, 0, 100);
	config_init();
	record_state_init();
	
	/**全局变量**/
	Fault_Total_Number=0;
	Precedence_Fault_Total_Number=0;
	Fault_Display_Index = 0;
	Precedence_Fault_Display_Index = 0;
	Fault_Device_Tail=(Fault_Device_List_struct*)NULL;
	Precedence_Fault_Device_Tail=(Fault_Device_List_struct*)NULL;
	
	for(i=0; i<CARD_NUM_MAX; i++)
	{
		CardState.Inform[i] = ERROR;
		CardState.Card_lose[i] = 60;
	}
	
}

static void Reset(void)
{
	uint8_t err;
	
	OSTaskSuspend(Task_GUI_PRIO);
	if( SUCCESS!=Password_Check_Reset()){
		OSTaskResume(Task_GUI_PRIO);
		return;
	}
	
	OSTaskResume(Task_GUI_PRIO);
	OSFlagPost(FLAG_GRP,StopPollingFlag,OS_FLAG_SET,&err);
	OSFlagPend(FLAG_GRP,PollTaskEndFlag,OS_FLAG_WAIT_SET_ALL,0,&err);//等待巡检完成
	delay_us(60000);
	send_reset(Terminal1);
	send_reset(Terminal2);
	delay_us(60000);
	HAL_NVIC_SystemReset();
}

/*
*********************************************************************************************************
*                                      App_EventCreate
*
* Description:  Creates the application events.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/
static  void  App_EventCreate (void)
{
	uint8_t  err;
	FLAG_GRP = OSFlagCreate(0u,&err);//创建标识组
	OSFlagNameSet(FLAG_GRP,"FLAG_GRP",&err);
	if(err!=OS_ERR_NONE)while(1);
	
	Key_Mbox_Handle = OSMboxCreate((void *)NULL);
	OSEventNameSet(Key_Mbox_Handle,"Key_Mbox",&err);
	if(err!=OS_ERR_NONE)while(1);
	
	FuWei_Handle = OSMboxCreate((void *)NULL);
	OSEventNameSet(FuWei_Handle,"XiaoYin_FuWei",&err);
	if(err!=OS_ERR_NONE)while(1);
	
	UsartDevice = OSSemCreate(1);//串口设备可用标志
	OSEventNameSet(UsartDevice,"UsartDevice",&err);
	if(err!=OS_ERR_NONE)while(1);
	
	W25X40_Device = OSSemCreate(1);//Flash设备可用标志
	OSEventNameSet(UsartDevice,"W25X40_Device",&err);
	if(err!=OS_ERR_NONE)while(1);
	
	DS1302_Device = OSSemCreate(1);//DS1302设备可用标志
	OSEventNameSet(UsartDevice,"DS1302_Device",&err);
	if(err!=OS_ERR_NONE)while(1);
}

static  void  App_TaskCreate (void)
{
    uint8_t      err;
	
	OSTaskCreateExt((void (*)(void *)) Task_Get_Key,
                    (void           *) 0,
                    (OS_STK         *)&Task_Get_Key_Stk[Task_Get_Key_STK_SIZE - 1],
                    (INT8U           ) Task_Get_Key_PRIO,
                    (INT16U          ) Task_Get_Key_PRIO,
                    (OS_STK         *)&Task_Get_Key_Stk[0],
                    (INT32U          ) Task_Get_Key_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    OSTaskNameSet(Task_Get_Key_PRIO, "KEY", &err);
	if(err!=OS_ERR_NONE)while(1);
    
    OSTaskCreateExt((void (*)(void *)) Task_GUI,
                    (void           *) 0,
                    (OS_STK         *)&TaskGUIStk[Task_GUI_STK_SIZE - 1],
                    (INT8U           ) Task_GUI_PRIO,
                    (INT16U          ) Task_GUI_PRIO,
                    (OS_STK         *)&TaskGUIStk[0],
                    (INT32U          ) Task_GUI_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    OSTaskNameSet(Task_GUI_PRIO, "GUI", &err);
	if(err!=OS_ERR_NONE)while(1);
	
	OSTaskCreateExt((void (*)(void *)) Task_Time_Display,
                    (void           *) 0,
                    (OS_STK         *)&Task_Time_Display_Stk[Task_Time_Display_STK_SIZE - 1],
                    (INT8U           ) Task_Time_Display_PRIO,
                    (INT16U          ) Task_Time_Display_PRIO,
                    (OS_STK         *)&Task_Time_Display_Stk[0],
                    (INT32U          ) Task_Time_Display_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    OSTaskNameSet(Task_Time_Display_PRIO, "Time_Display", &err);
	if(err!=OS_ERR_NONE)while(1);
	
	OSTaskCreateExt((void (*)(void *)) Task_Status_Indicator,
                    (void           *) 0,
                    (OS_STK         *)&Task_Status_Indicator_Stk[Task_Status_Indicator_STK_SIZE-1],
                    (INT8U           ) Task_Status_Indicator_PRIO,
                    (INT16U          ) Task_Status_Indicator_PRIO,
                    (OS_STK         *)&Task_Status_Indicator_Stk[0],
                    (INT32U          ) Task_Status_Indicator_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	OSTaskNameSet(Task_Status_Indicator_PRIO, "Indicator", &err);
	if(err!=OS_ERR_NONE)while(1);
	
	OSTaskCreateExt((void (*)(void *)) Task_Polling,
                    (void           *) 0,
                    (OS_STK         *)&Task_Polling_Stk[Task_Polling_STK_SIZE-1],
                    (INT8U           ) Task_Polling_PRIO,
                    (INT16U          ) Task_Polling_PRIO,
                    (OS_STK         *)&Task_Polling_Stk[0],
                    (INT32U          ) Task_Polling_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	OSTaskNameSet(Task_Polling_PRIO, "Polling", &err);
	if(err!=OS_ERR_NONE)while(1);
	
	OSTaskCreateExt((void (*)(void *)) Task_RefreshScreen,
                    (void           *) 0,
                    (OS_STK         *)&Task_RefreshScreen_Stk[Task_RefreshScreen_STK_SIZE-1],
                    (INT8U           ) Task_RefreshScreen_PRIO,
                    (INT16U          ) Task_RefreshScreen_PRIO,
                    (OS_STK         *)&Task_RefreshScreen_Stk[0],
                    (INT32U          ) Task_RefreshScreen_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	OSTaskNameSet(Task_RefreshScreen_PRIO, "RefreshScreen", &err);
	if(err!=OS_ERR_NONE)while(1);
}

/*
*********************************************************************************************************
*********************************************************************************************************
**                                         uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif
#endif
