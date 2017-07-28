#include "Protocol_Bus.h"
/*
*********************************************************************************************************
*
* �ļ���    : Protocol_Bus.c
* �汾��    : V1.00
* ��  ��    : j
*             
*
* ��Ȩ˵��  : �ɶ�����˹�����豸���޹�˾
* �ļ�����  : ʵ������ͨѶ
* ��������  : 
* �޸�����  : 2017/02/13
*********************************************************************************************************
*/

typedef struct{
	Terminal_enum	terminal;	//����
	uint8_t 		cmd;		//����
	uint8_t			dest;		//Ŀ�ĵ�ַ
	uint16_t 		length;		//���ݳ���
	uint8_t 		*data;		//����
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
			Sen_Rec->DataNumber=0;//0Ϊͨ��ʧ��
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
				Sen_Rec->Data[Sen_Rec->DataNumber]=ReceiveBuffer.Data[ReceiveBuffer.TX];//����
				Sen_Rec->DataNumber++;
				ReceiveBuffer.TX = (ReceiveBuffer.TX+1)&0x3ff;
			}
			if(Sen_Rec->DataNumber>=3) return SUCCESS;
			else{
				Sen_Rec->DataNumber=0;//0Ϊͨ��ʧ��
				return ERROR;
			}
		}
	}
}

/*******************************************************************************
* ������         : build_cmd_bus
* ��������       : ���ݴ������Ϣ�������ڴ��ڷ��͵�ָ��
*
* �������       : -cmd ������Ϣ�ṹ��ָ��
*				  -send ���ָ��ṹ��ָ��
*
* ����ֵ         : ��
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
* ������         : parse_cmd_bus
* ��������       : ��������Ĵ���ָ������
*
* �������       : -send ���봮��ָ��ṹ��ָ��
*				  -cmd ���������Ϣ�ṹ��ָ��
*
* ����ֵ         : ��
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
* ������         : send_reset
* ��������       : �����װ���·�� ����ʼ������ã�
*
* �������       : -terminal ����
* 
* ����ֵ         : ErrorStatus �������
*******************************************************************************/
ErrorStatus send_reset( Terminal_enum terminal )
{
	uint8_t i = 0, err;
	USART_TypeDef * Terminal;
	uint8_t data[] = {0x71, 0x00, 0x71};
	
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	
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
* ������         : report_output
* ��������       : ֪ͨ�װ忪ʼ��ֹͣ���
*
* �������       : -state ���� 0x00ֹͣ��� 0x01��ʼ���
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Lv1 )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv1, &cmd)) return ERROR;
	if(cmd.cmd == 0x30 && cmd.data[0] == state) return SUCCESS;
	else return ERROR;
}

/*******************************************************************************
* ������         : report_record
* ��������       : �ϴ���¼���װ� ��ʹ��Send_Lv2��
*
* �������       : -Record ��Ҫ�ϴ��Ľṹ��ָ��
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Lv2 )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv2, &cmd)) return ERROR;
	if(cmd.cmd!=0x31) return ERROR;

	return SUCCESS;
}

/*******************************************************************************
* ������         : poll_state_board
* ��������       : ��ѯ�װ�״̬
*
* �������       : -state ��ѯ���ָ��
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Lv1 )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv1, &cmd)) return ERROR;
	if(cmd.cmd != 0x10) return ERROR;
	
	*state = cmd.data[0];
	return SUCCESS;
}

/*******************************************************************************
* ������         : poll_data_card
* ��������       : ��ѯ��·������
*
* �������       : -card_id ��ѯ����
*                : -data ��ѯ��������ָ�� ��ָ��Send_Lv1�ṹ���ڣ�
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Lv1 )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv1, &cmd)) return ERROR;
	if(cmd.cmd != 0x01) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	
	*data = cmd.data;
	return SUCCESS;
}

/*******************************************************************************
* ������         : send_info_card
* ��������       : ���߻�·�����еǼǵ�̽�����ĵ�ַ�����͡�ͨ���� ��ʹ��Send_Lv2��
*
* �������       : -Card_ID ��ѯ����
*
* ����ֵ         : ErrorStatus �������
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
					 (uint16_t)sizeof(Detector_Cfg_struct));//����ǰ��¼
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Lv2 )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Lv2, &cmd)) return ERROR;
	if(cmd.cmd != 0x25) return ERROR;
	if(cmd.dest != Card_ID) return ERROR;
	return SUCCESS;
}

/*******************************************************************************
* ������         : charge_addr_card
* ��������       : �޸Ļ�·����ַ���˵����ã�
*
* �������       : -card_id ��·����
*		         : -old_addr �ɵ�ַ
*                : -new_addr �µ�ַ
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	if(cmd.data[0] != 0x01) return ERROR;
	
	return SUCCESS;
}

/*******************************************************************************
* ������         : auto_info_card
* ��������       : ��ʼ�Զ��Ǽǣ��˵����ã�
*
* �������       : -card_id ��·����
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	
	return SUCCESS;
}

/*******************************************************************************
* ������         : get_info_card
* ��������       : ��ȡ�Զ��Ǽ���Ϣ���˵����ã�
*
* �������       : -card_id ��·����
*                : -data ��ȡ�ĵǼ���Ϣ����ָ�� ��ָ��Send_Menu�ṹ���ڣ�
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	
	*data = cmd.data;
	return SUCCESS;
}

/*******************************************************************************
* ������         : get_data_detector
* ��������       : ��ȡ̽������Ϣ���˵����ã�
*
* �������       : -card_id ��·����
*                : -detector_id ̽������
*                : -data ��ȡ�ĵǼ���Ϣ����ָ�� ��ָ��Send_Menu�ṹ���ڣ�
*
* ����ֵ         : ErrorStatus �������
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
	tmp[1] = 0;	//�����е�Դ
	cmd.data = tmp;
	build_cmd_bus(&cmd, &Send_Menu);
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
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
* ������         : send_cmd_common
* ��������       : ����ͨ������������������޷������ݣ�
*
* �������       : -terminal ����
*                : -cmd ����
*                : -dest Ŀ���ַ
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	return SUCCESS;
}

/*******************************************************************************
* ������         : get_threshold_detector
* ��������       : ��ȡ̽������ֵ���˵����ã�
*
* �������       : -card_id ��·����
*                : -detector_id ̽������
*				 : -channel ͨ����
*                : -value ��ȡ��ֵ
*				 : -state ��ȡ��� ��1����ʾ�ɹ�����2����ʾ��ַ�����ڣ�
*								��3����ʾʧ�ܣ���4��ͨ������.
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
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
* ������         : set_threshold_detector
* ��������       : ����̽������ֵ���˵����ã����������֣�
*
* �������       : -card_id ��·����
*                : -detector_id ̽������
*				 : -channel ͨ����
*                : -value ���õ�ֵ
*				 : -state ��ȡ��� ��1����ʾ�ɹ�����2����ʾ��ַ�����ڣ�
*								��3����ʾʧ�ܣ���4��ͨ������.
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x20) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	*state = cmd.data[0];
	
	return SUCCESS;
}

/*******************************************************************************
* ������         : set_threshold_detector
* ��������       : У׼̽������ֵ���˵����ã����������֣���������Դ��
*
* �������       : -card_id ��·����
*                : -detector_id ̽������
*				 : -channel ͨ����(��������)��У׼���ͣ�������Դ 0x11Ϊ��ѹ 0x22Ϊ������
*                : -value ֵ
*				 : -state ��ȡ��� ��1����ʾ�ɹ�����2����ʾ��ַ�����ڣ�
*								��3����ʾʧ�ܣ���4��ͨ������.
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x07) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	*state = cmd.data[0];
	
	return SUCCESS;
}

/*******************************************************************************
* ������         : P_read_threshold_detector
* ��������       : ��ȡ̽������ֵ���˵����ã���������Դ��
*
* �������       : -card_id ��·����
*                : -detector_id ̽������
*                : -type ��ֵ���� 0x11Ϊ��ѹ 0x22Ϊ����
*				 : -ValueL Ƿѹֵ������ֵ
*                : -ValueH ��ѹֵ
*				 : -state ��ȡ��� ��1����ʾ�ɹ�������Ϊʧ�ܡ�
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
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
* ������         : P_set_threshold_detector
* ��������       : ����̽������ֵ���˵����ã���������Դ��
*
* �������       : -card_id ��·����
*                : -detector_id ̽������
*                : -type ��ֵ���� 0x11Ϊ��ѹ 0x22Ϊ����
*				 : -ValueL Ƿѹֵ������ֵ
*                : -ValueH ��ѹֵ
*				 : -state ���ý�� ��1����ʾ�ɹ�������Ϊʧ�ܡ�
*
* ����ֵ         : ErrorStatus �������
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
	OSSemPend(UsartDevice,0,&err);//�ȴ�
	if(SUCCESS != Usart_Communicate( &Send_Menu )){
		OSSemPost(UsartDevice);
		return ERROR;//�շ�
	}
	OSSemPost(UsartDevice);
	if(SUCCESS != parse_cmd_bus(&Send_Menu, &cmd)) return ERROR;
	if(cmd.cmd != 0x20) return ERROR;
	if(cmd.dest != card_id) return ERROR;
	*state = cmd.data[0];
	
	return SUCCESS;
}