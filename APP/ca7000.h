#ifndef __CA7000_H
#define __CA7000_H

#include "stm32f1xx_hal.h"
#include "Usart.h"
#include "DS1302.h"
#include "SG19264.h"
#include <ucos_ii.h>


#define	PROGRAM_TYPE 1			//�������Ͷ��� 1��������Դ 	2����������

#define	MAIN_BOARD 2			//Ӳ�����Ͷ��� 1����Ӳ��	2����Ӳ��

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

/*W25X40�洢��ַ����*/
/*
***************************************************************
* �С��ڡ��ֱ�ʾ��1��ʼ�����ġ�
* �洢оƬ����FlashоƬW25X40������128��������ÿ������4KB��
* ǰ52��������0x00000-0x33FFF���洢��
* 	GB2312�����ֿ�12*16����ʵ��ռ���ֽ���0x336ff.
* 
* ��53������0x34000-0x34FFF���б���������Ϣ
* 
* ��54������0x35000-0x35FFF���б��棺1��2��3��4��5�Ż�·��̽������Ϣ
* 	0x35000-0x3531F��1�Ż�·��̽�����Ǽ���Ϣ
* 	0x35320-0x3563F��2�Ż�·��̽�����Ǽ���Ϣ
* 	0x35640-0x3595F��3�Ż�·��̽�����Ǽ���Ϣ
* 	0x35960-0x35C7F��4�Ż�·��̽�����Ǽ���Ϣ
* 	0x35C80-0x35F9F��5�Ż�·��̽�����Ǽ���Ϣ
* 
* ��55������0x36000-0x36FFF���б��棺1�Ż�·��̽����������Ϣ
* ��56������0x37000-0x37FFF���б��棺2�Ż�·��̽����������Ϣ
* ��57������0x38000-0x38FFF���б��棺3�Ż�·��̽����������Ϣ
* ��58������0x39000-0x39FFF���б��棺4�Ż�·��̽����������Ϣ
* ��59������0x3A000-0x3AFFF���б��棺5�Ż�·��̽����������Ϣ
*
* ��60������0x3b000-0x3bFFF���б����¼״̬1
* ��61������0x3c000-0x3cFFF���б����¼״̬2
* ��62������0x3d000-0x3dFFF���б����¼״̬3
*
****************************************************************
*/
#define CfgRecord_StartAddr				0x034000u	//��53������0x34000-0x34FFF���б���������Ϣ
#define CfgRecord_EndAddr				0x034fffu

#define DeviceRecord_StartAddr			0x035000u	//��54������0x35000-0x35FFF���б��棺1��2��3��4��5�Ż�·��̽������Ϣ
#define DeviceRecord_EndAddr			0x037000u

#define DescriptionRecord_StartAddr		0x036000u	//��54-59������0x36000-0x3AFFF���б��棺1��2��3��4��5�Ż�·��̽����������Ϣ
#define DescriptionRecord_EndAddr		0x03afffu

#define RecordState_StartAddr			0x03b000u	//��60-62������0x3b000-0x3dFFF���б����¼״̬
#define RecordState_EndAddr				0x03dfffu

#define FireRecord_StartAddr			0x040000u	//�𾯼�¼ 128KB
#define FireRecord_EndAddr				0x05ffffu
#define FaultRecord_Start				0x060000u	//���ϼ�¼ 128KB
#define FaultRecord_EndAddr				0x07ffffu

/*DS1302�洢��ַ����*/
#define RecordState_In_1302RAM	0x00	//1302��¼״̬

//��·������
#define CARD_NUM_MAX	4

/*Type ���ݽṹ����*/
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

/*��ǰ��·���״̬*/
typedef struct{
	ErrorStatus Inform[CARD_NUM_MAX];
	uint8_t Card_lose[CARD_NUM_MAX];
}CardState_struct;

/*���������ṹ*/
typedef uint8_t Position_Typedef[16];

/*==========================================*/
typedef enum{
	Object_Detector =1,
	Object_Card,
	Object_SubBoard
}Device_Object;

/*ȫ�ֱ�������*/
#define MenuDelay  60000
#define TotalFault	4096
#define PageFault	256		//һ�������Ĺ�����

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
#pragma pack(1)			//����1�ֽڶ���

/*Flash������Ϣ�ṹ*/
typedef struct{
	uint8_t password[4];
	uint8_t card_state;//�Ƿ�Ǽ�
	uint8_t print_state;
}Config_TypeDef;
/*Flash������Ϣ�洢�ṹ*/
typedef struct{
	Config_TypeDef config;
	uint16_t crc16;
}Config_Store;

/*�洢��¼״̬*/
typedef struct
{
	uint16_t Index;
	uint16_t FullNumber;
}Record_State_struct;

/*��¼״̬�ṹ*/
typedef struct{
	Record_State_struct fault;
	Record_State_struct fire;
}Record_State_TypeDef;
/*��¼״̬�洢�ṹ*/
typedef struct{
	Record_State_TypeDef record_state;
	uint16_t crc16;
}Record_State_Store;

#pragma pack(pop)

extern OS_EVENT *Status_Ack_Message;
extern OS_EVENT *Key_Mbox_Handle;
extern OS_EVENT *FuWei_Handle;
extern OS_EVENT *UsartDevice;

extern OS_FLAG_GRP *FLAG_GRP;//��־��

extern CardState_struct CardState;

extern ScreenBuffS ScreenBuffer;

extern SYSTEMTIME systime;

extern Send_Receive_struct Send_Lv1, Send_Lv2;
extern Send_Receive_struct Send_Menu;

extern uint8_t Standy_State;

extern Config_TypeDef Config;
extern Record_State_TypeDef Record_State;

#endif
