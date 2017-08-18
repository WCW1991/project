#ifndef __CA7000_H
#define __CA7000_H

#include "stm32f1xx_hal.h"
#include "Usart.h"
#include "DS1302.h"
#include "SG19264.h"
#include <ucos_ii.h>


#define	PROGRAM_TYPE 1			//程序类型定义 1：消防电源 	2：电气火灾

#define	MAIN_BOARD 2			//硬件类型定义 1：旧硬件	2：新硬件

#define	MainVersion         1
#define	SubVersion          2

#define	Task_Polling_PRIO				3
#define	Task_Status_Indicator_PRIO		6
#define	Task_RefreshScreen_PRIO			8
#define	APP_CFG_TASK_START_PRIO			10
#define	Task_Get_Key_PRIO				12
#define	Task_GUI_PRIO					14
#define	Task_Time_Display_PRIO			19
	
#define	APP_CFG_TASK_START_STK_SIZE   	400
#define	Task_GUI_STK_SIZE            	1500
#define	Task_Status_Indicator_STK_SIZE 	300
#define	Task_Get_Key_STK_SIZE			300
#define	Task_RefreshScreen_STK_SIZE		200
#define	Task_Time_Display_STK_SIZE  	300
#define	Task_Polling_STK_SIZE			2000

/*W25X40存储地址定义*/
/*
***************************************************************
* 有“第”字表示从1开始计数的。
* 存储芯片采用Flash芯片W25X40，具有128个扇区，每个扇区4KB。
* 前52个扇区（0x00000-0x33FFF）存储：
* 	GB2312中文字库12*16点阵，实际占用字节数0x336ff.
* 
* 第53扇区（0x34000-0x34FFF）中保存配置信息
* 
* 第54扇区（0x35000-0x35FFF）中保存：1、2、3、4、5号回路板探测器信息
* 	0x35000-0x3531F：1号回路板探测器登记信息
* 	0x35320-0x3563F：2号回路板探测器登记信息
* 	0x35640-0x3595F：3号回路板探测器登记信息
* 	0x35960-0x35C7F：4号回路板探测器登记信息
* 	0x35C80-0x35F9F：5号回路板探测器登记信息
* 
* 第55扇区（0x36000-0x36FFF）中保存：1号回路板探测器中文信息
* 第56扇区（0x37000-0x37FFF）中保存：2号回路板探测器中文信息
* 第57扇区（0x38000-0x38FFF）中保存：3号回路板探测器中文信息
* 第58扇区（0x39000-0x39FFF）中保存：4号回路板探测器中文信息
* 第59扇区（0x3A000-0x3AFFF）中保存：5号回路板探测器中文信息
*
* 第60扇区（0x3b000-0x3bFFF）中保存记录状态1
* 第61扇区（0x3c000-0x3cFFF）中保存记录状态2
* 第62扇区（0x3d000-0x3dFFF）中保存记录状态3
*
****************************************************************
*/
#define CfgRecord_StartAddr				0x034000u	//第53扇区（0x34000-0x34FFF）中保存配置信息
#define CfgRecord_EndAddr				0x034fffu

#define DeviceRecord_StartAddr			0x035000u	//第54扇区（0x35000-0x35FFF）中保存：1、2、3、4、5号回路板探测器信息
#define DeviceRecord_EndAddr			0x037000u

#define DescriptionRecord_StartAddr		0x036000u	//第54-59扇区（0x36000-0x3AFFF）中保存：1、2、3、4、5号回路板探测器中文信息
#define DescriptionRecord_EndAddr		0x03afffu

#define RecordState_StartAddr			0x03b000u	//第60-62扇区（0x3b000-0x3dFFF）中保存记录状态
#define RecordState_EndAddr				0x03dfffu

#define FireRecord_StartAddr			0x040000u	//火警记录 128KB
#define FireRecord_EndAddr				0x05ffffu
#define FaultRecord_Start				0x060000u	//故障记录 128KB
#define FaultRecord_EndAddr				0x07ffffu

/*DS1302存储地址定义*/
#define RecordState_In_1302RAM	0x00	//1302记录状态

//回路板数量
#define CARD_NUM_MAX	4

/*Type 数据结构定义*/
typedef struct{
	uint8_t event_type;
	uint8_t device_type;
	uint8_t area_code;
	uint8_t area_machine;
	uint8_t card_code;
	uint8_t address;
	uint8_t channel;
	uint8_t fault_type;
	uint16_t description;
}event_struct;

typedef struct{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}RecordTime_struct;

typedef struct
{
	event_struct event;
	RecordTime_struct time;
}Record_struct;

typedef struct
{
	uint8_t card_code;
	uint8_t address;
	uint8_t channel_num;
	uint8_t State_type;
}Detector_Cfg_struct;

/*当前回路板的状态*/
typedef struct{
	ErrorStatus Inform[CARD_NUM_MAX];
	uint8_t Card_lose[CARD_NUM_MAX];
}CardState_struct;

/*中文描述结构*/
typedef uint8_t Position_Typedef[16];

/*==========================================*/
typedef enum{
	Object_Detector =1,
	Object_Card,
	Object_SubBoard
}Device_Object;

/*全局变量申明*/
#define MenuDelay  60000
#define TotalFault	4096
#define PageFault	256		//一个扇区的故障数

/*================================================================
 *================================================================
 */
typedef enum 
{
	DisplayDateTimeFlag=0x0001u,
	ZhuDianFlag=0x0002u,
	YunXingFlag=0x0004u,
	BaoJingFlag=0x0008u,
	GuZhangFlag=0x0010u,
	BeiDianFlag=0x0020u,
	TongXinFlag=0x0040u,
	XiaoYinFlag=0x0080u,
	XiTongFultFlag=0x0100u,
	SelfTestFlag=0x0200u,
	StopGuiFlag=0x0400u,
	ScreenRefreshFlag=0x0800u,
	PollTaskEndFlag=0x1000u,
	BaseBoardOutPutFlag=0x2000u,
	PrinterNoPaperFlag=0x4000u,
	StopPollingFlag=0x8000u,
}FLAG_GRP_enum;

#pragma pack(push)
#pragma pack(1)			//设置1字节对齐

/*Flash配置信息结构*/
typedef struct{
	uint8_t password[4];
	uint8_t card_state;//是否登记
	uint8_t print_state;
}Config_TypeDef;
/*Flash配置信息存储结构*/
typedef struct{
	Config_TypeDef config;
	uint16_t crc16;
}Config_Store;

/*存储记录状态*/
typedef struct
{
	uint16_t Index;
	uint16_t FullNumber;
}Record_State_struct;

/*记录状态结构*/
typedef struct{
	Record_State_struct fault;
	Record_State_struct fire;
}Record_State_TypeDef;
/*记录状态存储结构*/
typedef struct{
	Record_State_TypeDef record_state;
	uint16_t crc16;
}Record_State_Store;

#pragma pack(pop)

extern OS_EVENT *Status_Ack_Message;
extern OS_EVENT *Key_Mbox_Handle;
extern OS_EVENT *FuWei_Handle;
extern OS_EVENT *UsartDevice;

extern OS_FLAG_GRP *FLAG_GRP;//标志组

extern CardState_struct CardState;

extern ScreenBuffS ScreenBuffer;

extern SYSTEMTIME systime;

extern Send_Receive_struct Send_Lv1, Send_Lv2;
extern Send_Receive_struct Send_Menu;

extern uint8_t Standy_State;

extern Config_TypeDef Config;
extern Record_State_TypeDef Record_State;

#endif
