#ifndef __PYINPUT_H
#define __PYINPUT_H
#include "stdint.h"
typedef uint8_t u8;
typedef uint16_t u16;
//ƴ�������ƴ���Ķ�Ӧ��
typedef struct
{
  u8 *py_input;//������ַ���
  u8 *py;	   //��Ӧ��ƴ��
  u8 *pymb;	   //���
}py_index;

#define MAX_MATCH_PYMB 	10	//���ƥ����

//ƴ�����뷨
typedef struct
{
  u8(*getpymb)(u8 *instr);			//�ַ����������ȡ����
  py_index *pymb[MAX_MATCH_PYMB];	//������λ��
}pyinput;

extern pyinput t9;
u8 str_match(u8*str1,u8*str2);
u8 get_matched_pymb(u8 *strin,py_index **matchlist);
u8 get_pymb(u8* str);
void PinYin_Input(char *InputHz);
void QuWeiInput(char *InputHz);
#endif







