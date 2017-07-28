#include "Protocol_Bus.h"
/*
*********************************************************************************************************
*
* 文件名    : Protocol_Bus.c
* 版本号    : V1.00
* 作  者    : j
*             
*
* 版权说明  : 成都安吉斯智能设备有限公司
* 文件描述  : 实现总线通讯
* 开发环境  : 
* 修改日期  : 2017/02/13
*********************************************************************************************************
*/

typedef struct{
	Terminal_enum	terminal;	//总线
	uint8_t 		cmd;		//命令
	uint8_t			dest;		//目的地址
	uint16_t 		length;		//数据长度
	uint8_t 		*data;		//数据
}TypeDef_CMD_Bus_t;

static ErrorStatus Usart_Communicate( Send_Receive_struct *Sen_Rec );
static void build_cmd_bus(TypeDef_CMD_Bus_t *cmd, Send_Receive_struct *send);
static ErrorStatus parse_cmd_bus(Send_Receive_struct *recv, TypeDef_CMD_Bus_t *cmd);


static ErrorStatus Usart_Communicate( Send_Receive_struct *Sen_Rec )
{
	uint16_t i=0;
	USART_TypeDef * Terminal;
	
	switch(Sen_Rec->Terminal){
	case Terminal1:{
		HAL_NVIC_DisableIRQ(UART4_IRQn);
		Terminal=USART1;
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}break;
	case Terminal2:{
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		Terminal=UART4;
		HAL_NVIC_EnableIRQ(UART4_IRQn);
	}break;
	}
	
	OS_ENTER_CRITICAL();
	for(i=0;i<Sen_Rec->DataNumber;i++){Terminal->DR = Sen_Rec->Data[i];while(!(Terminal->SR&USART_SR_TXE));}
	OS_EXIT_CRITICAL();
	
	ReceiveBuffer.RX = ReceiveBuffer.TX = 0;
	Sen_Rec->DataNumber = 0;
	i=0;
	OneByte=ERROR;
	while(OneByte==ERROR){
		OSTimeDlyHMSM(0, 0, 0, 1);
		i++;
		if(i>1000){
			HAL_NVIC_DisableIRQ(USART1_IRQn);
			HAL_NVIC_DisableIRQ(UART4_IRQn);
			Sen_Rec->DataNumber=0;//0为通信失败
			return ERROR;
		}
	}
	OneByte=ERROR;
	i=0;
	while(1){
		OSTimeDlyHMSM(0, 0, 0, 1);
		if(OneByte==ERROR)
			i++;
		else{
			OneByte=ERROR;
			i=0;
		}
		if(i>20){
			HAL_NVIC_DisableIRQ(USART1_IRQn);
			HAL_NVIC_DisableIRQ(UART4_IRQn);
			while(ReceiveBuffer.RX != ReceiveBuffer.TX)
			{
				Sen_Rec->Data[Sen_Rec->DataNumber]=ReceiveBuffer.Data[ReceiveBuffer.TX];//拷贝
				Sen_Rec->DataNumber++;
				ReceiveBuffer.TX = (ReceiveBuffer.TX+1)&0x3ff;
			}
			if(Sen_Rec->DataNumber>=3) return SUCCESS;
			else{
				Sen_Rec->DataNumber=0;//0为通信失败
				return ERROR;
			}
		}
	}
}

/*******************************************************************************
* 函数名         : build_cmd_bus
* 功能描述       : 根据传入的信息构建用于串口发送的指令
*
* 输入参数       : -cmd 命令信息结构体指针
*				  -send 输出指令结构体指针
*
* 返回值         : 无
*******************************************************************************/
static void build_cmd_bus(TypeDef_CMD_Bus_t *cmd, Send_Receive_struct *send)
{
  	uint16_t i = 0;
  	
	send->Terminal = cmd->terminal;
	send->DataNumber = cmd->length+3;
	send->Data[0] = cmd->cmd;
	send->Data[1] = cmd->dest;
	if(cmd->length) memcpy(&send->Data[2], cmd->data, cmd->length);
	send->Data[send->DataNumber-1] = 0x00;
	for(i=0; i<send->DataNumber-1; i++) send->Data[send->DataNumber-1] += send->Data[i];
}

/*******************************************************************************
* 函数名         : parse_cmd_bus
* 功能描述       : 解析传入的串口指令数据
*
* 输入参数       : -send 输入串口指令结构体指针
*				  -cmd 输出命令信息结构体指针
*
* 返回值         : 无
*******************************************************************************/
static ErrorStatus parse_cmd_bus(Send_Receive_struct *recv, TypeDef_CMD_Bus_t *cmd)
{
  	uint16_t i = 0;
  	uint8_t check_sum = 0;
  	
	if(recv->DataNumber < 3) return ERROR;
	for(i=0; i<recv->DataNumber-1; i++) check_sum += recv->Data[i];
	if(check_sum != recv->Data[recv->DataNumber-1]) return ERROR;
	
	cmd->terminal = recv->Terminal;
	cmd->length = recv->DataNumber-3;
	cmd->cmd = recv->Data[0];
	cmd->dest = recv->Data[1];
	cmd->data = &recv->Data[2];
	
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : send_reset
* 功能描述       : 重启底板或回路板 （起始任务调用）
*
* 输入参数       : -terminal 总线
* 
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus send_reset( Terminal_enum terminal )
{
	uint8_t i = 0, err;
	USART_TypeDef * Terminal;
	uint8_t data[] = {0x71, 0x00, 0x71};
	
	OSSemPend(UsartDevice,0,&err);//等待
	
	switch(terminal){
	case Terminal1:{
		HAL_NVIC_DisableIRQ(UART4_IRQn);
		Terminal=USART1;
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}break;
	case Terminal2:{
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		Terminal=UART4;
		HAL_NVIC_EnableIRQ(UART4_IRQn);
	}break;
	}
	
	OS_ENTER_CRITICAL();
	for(i=0;i<3;i++){Terminal->DR = data[i];while(!(Terminal->SR&USART_SR_TXE));}
	OS_EXIT_CRITICAL();
	
	OSSemPost(UsartDevice);
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : report_output
* 功能描述       : 通知底板开始或停止输出
*
* 输入参数       : -state 开关 0x00停止输出 0x01开始输出
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus report_output(uint8_t state)
{
	uint8_t  err;
	uint8_t data[1];
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal2;
	cmd.length = 1;
	cmd.cmd = 0x30;
	cmd.dest = 0x70;
	data[0] = state;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Lv1);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Lv1 )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv1, &cmd)) return ERROR;
	if(cmd.cmd == 0x30 && cmd.data[0] == state) return SUCCESS;
	else return ERROR;
}

/*******************************************************************************
* 函数名         : report_record
* 功能描述       : 上传记录到底板 （使用Send_Lv2）
*
* 输入参数       : -Record 需要上传的结构体指针
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus report_record( Record_struct *Record )
{
	uint8_t err;
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal2;
	cmd.length = sizeof(Record_struct);
	cmd.cmd = 0x31;
	cmd.dest = 0x70;
	cmd.data = (uint8_t *)Record;
	build_cmd_bus(&cmd, &Send_Lv2);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Lv2 )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv2, &cmd)) return ERROR;
	if(cmd.cmd!=0x31) return ERROR;

	return SUCCESS;
}

/*******************************************************************************
* 函数名         : poll_state_board
* 功能描述       : 查询底板状态
*
* 输入参数       : -state 查询结果指针
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus poll_state_board( uint8_t *state )
{
	uint8_t  err;
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal2;
	cmd.length = 0;
	cmd.cmd = 0x10;
	cmd.dest = 0x70;
	build_cmd_bus(&cmd, &Send_Lv1);	
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Lv1 )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv1, &cmd)) return ERROR;
	if(cmd.cmd != 0x10) return ERROR;
	
	*state = cmd.data[0];
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : poll_data_card
* 功能描述       : 查询回路卡数据
*
* 输入参数       : -card_id 查询卡号
*                : -data 查询到的数据指针 （指向Send_Lv1结构体内）
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus poll_data_card( uint8_t card_id, uint8_t **data )
{
	uint8_t err;
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 1;
	cmd.cmd = 0x01;
	cmd.dest = card_id;
	cmd.data = &Standy_State;
	build_cmd_bus(&cmd, &Send_Lv1);	
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Lv1 )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv1, &cmd)) return ERROR;
	if(cmd.cmd != 0x01) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	
	*data = cmd.data;
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : send_info_card
* 功能描述       : 告诉回路板所有登记的探测器的地址、类型、通道数 （使用Send_Lv2）
*
* 输入参数       : -Card_ID 查询卡号
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus send_info_card( uint8_t Card_ID )
{
	uint8_t err;
	uint8_t data[601];
	uint16_t Detector_Cfg_ID,Index=0;
	Detector_Cfg_struct Detector_Cfg;
	TypeDef_CMD_Bus_t cmd;
	
	for(Detector_Cfg_ID=0;Detector_Cfg_ID<200;Detector_Cfg_ID++){
		SpiFlashRead((uint8_t *)&Detector_Cfg,
					 ((Card_ID-1)*200+Detector_Cfg_ID)*sizeof(Detector_Cfg_struct)+DeviceRecord_StartAddr,
					 (uint16_t)sizeof(Detector_Cfg_struct));//读当前记录
		if(Detector_Cfg.State_type!=0xff&&Detector_Cfg.State_type&0x80){
			data[Index*3+1]=Detector_Cfg_ID+1;
			data[Index*3+2]=Detector_Cfg.State_type&0x7F;
			data[Index*3+3]=Detector_Cfg.channel_num;
			Index++;
		}
	}
	data[0] = Index;
	cmd.terminal = Terminal1;
	cmd.length = Index*3+1;
	cmd.cmd = 0x25;
	cmd.dest = Card_ID;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Lv2);	
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Lv2 )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv2, &cmd)) return ERROR;
	if(cmd.cmd != 0x25) return ERROR;
	if(cmd.dest != Card_ID) return ERROR;
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : charge_addr_card
* 功能描述       : 修改回路卡地址（菜单调用）
*
* 输入参数       : -card_id 回路卡号
*		         : -old_addr 旧地址
*                : -new_addr 新地址
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus charge_addr_card( uint8_t card_id, uint8_t old_addr, uint8_t new_addr )
{
	uint8_t err;
	uint8_t data[2];
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 2;
	cmd.cmd = 0x21;
	cmd.dest = card_id;
	data[0] = old_addr;
	data[1] = new_addr;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Menu);	
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	if(cmd.data[0] != 0x01) return ERROR;
	
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : auto_info_card
* 功能描述       : 开始自动登记（菜单调用）
*
* 输入参数       : -card_id 回路卡号
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus auto_info_card( uint8_t card_id )
{
	uint8_t  err;
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 0;
	cmd.cmd = 0x04;
	cmd.dest = card_id;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : get_info_card
* 功能描述       : 获取自动登记信息（菜单调用）
*
* 输入参数       : -card_id 回路卡号
*                : -data 获取的登记信息数据指针 （指向Send_Menu结构体内）
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus get_info_card( uint8_t card_id, uint8_t **data )
{
	uint8_t  err;
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 0;
	cmd.cmd = 0x06;
	cmd.dest = card_id;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	
	*data = cmd.data;
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : get_data_detector
* 功能描述       : 获取探测器信息（菜单调用）
*
* 输入参数       : -card_id 回路卡号
*                : -detector_id 探测器号
*                : -data 获取的登记信息数据指针 （指向Send_Menu结构体内）
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus get_data_detector( uint8_t card_id, uint8_t detector_id, uint8_t **data )
{
	uint8_t  err;
	uint8_t tmp[2];
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 2;
	cmd.cmd = 0x02;
	cmd.dest = card_id;
	tmp[0] = detector_id;
	tmp[1] = 0;	//查所有电源
	cmd.data = tmp;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd))
	{
		return ERROR;
	}
	if(cmd.dest != card_id)
	{
		return ERROR;
	}
	
	*data = cmd.data;
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : send_cmd_common
* 功能描述       : 发送通用型命令（无数据区，无返回数据）
*
* 输入参数       : -terminal 总线
*                : -cmd 命令
*                : -dest 目标地址
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus send_cmd_common( Terminal_enum terminal, uint8_t cmd, uint8_t dest )
{
	uint8_t  err;
	TypeDef_CMD_Bus_t cmd_bus;
	
	cmd_bus.terminal = terminal;
	cmd_bus.length = 0;
	cmd_bus.cmd = cmd;
	cmd_bus.dest = dest;
	build_cmd_bus(&cmd_bus, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : get_threshold_detector
* 功能描述       : 获取探测器阈值（菜单调用）
*
* 输入参数       : -card_id 回路卡号
*                : -detector_id 探测器号
*				 : -channel 通道号
*                : -value 获取的值
*				 : -state 读取结果 ’1’表示成功，’2’表示地址不存在，
*								‘3’表示失败，‘4’通道错误.
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus get_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t channel, uint16_t *value, uint8_t *state )
{
	uint8_t  err;
	uint8_t data[2];
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 2;
	cmd.cmd = 0x26;
	cmd.dest = card_id;
	data[0] = detector_id;
	data[1] = channel;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x26) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	if(cmd.data[0] != detector_id) return ERROR;
	*state = cmd.data[1];	
	
	*value = cmd.data[2]<<8 | cmd.data[3];
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : set_threshold_detector
* 功能描述       : 设置探测器阈值（菜单调用）（电气火灾）
*
* 输入参数       : -card_id 回路卡号
*                : -detector_id 探测器号
*				 : -channel 通道号
*                : -value 设置的值
*				 : -state 读取结果 ’1’表示成功，’2’表示地址不存在，
*								‘3’表示失败，‘4’通道错误.
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus set_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t channel, uint16_t value, uint8_t *state )
{
	uint8_t  err;
	uint8_t data[4];
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 4;
	cmd.cmd = 0x20;
	cmd.dest = card_id;
	data[0] = detector_id;
	data[1] = channel;
	data[2] = value >> 8;
	data[3] = value;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x20) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	*state = cmd.data[0];
	
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : set_threshold_detector
* 功能描述       : 校准探测器阈值（菜单调用）（电气火灾）（消防电源）
*
* 输入参数       : -card_id 回路卡号
*                : -detector_id 探测器号
*				 : -channel 通道号(电气火灾)，校准类型（消防电源 0x11为电压 0x22为电流）
*                : -value 值
*				 : -state 读取结果 ’1’表示成功，’2’表示地址不存在，
*								‘3’表示失败，‘4’通道错误.
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus calibration_detector( uint8_t card_id, uint8_t detector_id, uint8_t channel, uint16_t value, uint8_t *state )
{
	uint8_t  err;
	uint8_t data[4];
	TypeDef_CMD_Bus_t cmd;

	cmd.terminal = Terminal1;
	cmd.length = 4;
	cmd.cmd = 0x07;
	cmd.dest = card_id;
	data[0] = detector_id;
	data[1] = channel;
	data[2] = value >> 8;
	data[3] = value&0xff;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x07) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	*state = cmd.data[0];
	
	return SUCCESS;
}

/*******************************************************************************
* 函数名         : P_read_threshold_detector
* 功能描述       : 读取探测器阈值（菜单调用）（消防电源）
*
* 输入参数       : -card_id 回路卡号
*                : -detector_id 探测器号
*                : -type 阈值类型 0x11为电压 0x22为电流
*				 : -ValueL 欠压值，过流值
*                : -ValueH 过压值
*				 : -state 读取结果 ’1’表示成功，其它为失败。
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus P_read_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t type, uint16_t *ValueL, uint16_t *ValueH, uint8_t *state )
{
	uint8_t  err;
	uint8_t data[3];
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	cmd.length = 3;
	cmd.cmd = 0x09;
	cmd.dest = card_id;
	data[0] = detector_id;
	data[1] = type;
	data[2] = 0x01;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x09) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	if(cmd.data[0] != detector_id) return ERROR;
	*state = cmd.data[1];
	
	*ValueL = cmd.data[2]<<8 | cmd.data[3];
	if(type == 0x11)
	{
		*ValueH = cmd.data[4]<<8 | cmd.data[5];
	}

	return SUCCESS;
}

/*******************************************************************************
* 函数名         : P_set_threshold_detector
* 功能描述       : 设置探测器阈值（菜单调用）（消防电源）
*
* 输入参数       : -card_id 回路卡号
*                : -detector_id 探测器号
*                : -type 阈值类型 0x11为电压 0x22为电流
*				 : -ValueL 欠压值，过流值
*                : -ValueH 过压值
*				 : -state 设置结果 ’1’表示成功，其它为失败。
*
* 返回值         : ErrorStatus 操作结果
*******************************************************************************/
ErrorStatus P_set_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t type, uint16_t ValueL, uint16_t ValueH, uint8_t *state )
{
	uint8_t  err;
	uint8_t data[7];
	TypeDef_CMD_Bus_t cmd;
	
	cmd.terminal = Terminal1;
	if(type == 0x11) cmd.length = 7;
	else cmd.length = 5;
	cmd.cmd = 0x20;
	cmd.dest = card_id;
	data[0] = detector_id;
	data[1] = type;
	data[2] = 0;
	data[3] = ValueL >> 8;
	data[4] = ValueL;
	data[5] = ValueH >> 8;
	data[6] = ValueH;
	cmd.data = data;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//等待
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//收发
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x20) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	*state = cmd.data[0];
	
	return SUCCESS;
}