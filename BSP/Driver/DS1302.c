/**************************************************************************
  
                     THE REAL TIMER DS1302 DRIVER LIB
  
               COPYRIGHT (c)   2016 BY WCW.
                         --  ALL RIGHTS RESERVED  --
  
   File Name:       DS1302.h
   Author:          WCW
   Created:         2016/7/21
   Modified:  NO
   Revision:   1.0
  
***************************************************************************/
#include <DS1302.h>
#include "bsp_periph.h"
#include "MyLib.h"
#include    <ucos_ii.h>

#define	INPUT	GPIOC->CRL = GPIOC->CRL&0xffffff0f|0x00000040//PC1��������
#define	OUTPUT	GPIOC->CRL = GPIOC->CRL&0xffffff0f|0x00000030//PC1�������50MHz
#define  DS1302_CLK		PCout(0)              //ʵʱʱ��ʱ�������� 
#define  DS1302_O		PCout(1)              //ʵʱʱ������������
#define  DS1302_I		PCin(1)               //ʵʱʱ������������
#define  DS1302_RST		PCout(2)              //ʵʱʱ�Ӹ�λ������
#define  dly  10

#define DS1302_SECOND 0x80
#define DS1302_MINUTE 0x82
#define DS1302_HOUR  0x84 
#define DS1302_WEEK  0x8A
#define DS1302_DAY  0x86
#define DS1302_MONTH 0x88
#define DS1302_YEAR  0x8C

extern OS_EVENT *DS1302_Device;

static void DS1302InputByte(unsigned char d)  //ʵʱʱ��д��һ�ֽ�(�ڲ�����)
{ 
    unsigned char i;
    for(i=8u; i>0u; i--)
    {
		if(d&0x01u)
			DS1302_O = 1u;
		else
			DS1302_O = 0u;
		d = d>>1;
        DS1302_CLK = 1u;//����������
		Delay_NOP(dly);
        DS1302_CLK = 0u;
    }
	Delay_NOP(dly);
}

static unsigned char DS1302OutputByte(void)  //ʵʱʱ�Ӷ�ȡһ�ֽ�(�ڲ�����)
{ 
    unsigned char i, Data=0;
	INPUT;
    for(i=8u; i>0u; i--)
    {
        if( DS1302_I )
			Data = Data>>1|0x80u;
		else
			Data = Data>>1u;
        DS1302_CLK = 1;
		Delay_NOP(dly);
        DS1302_CLK = 0;
		Delay_NOP(dly);
    }
	OUTPUT;
    return(Data); 
}

static void Write1302(unsigned char ucAddr, unsigned char ucDa) //ucAddr: DS1302��ַ, ucData: Ҫд������
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
	Delay_NOP(dly);
    DS1302InputByte(ucAddr);        // ��ַ������ 
    DS1302InputByte(ucDa);        // д1Byte����
    DS1302_CLK = 0;
    DS1302_RST = 0;
}

static unsigned char Read1302(unsigned char ucAddr) //��ȡDS1302ĳ��ַ������
{
    unsigned char ucData;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
	Delay_NOP(dly);
    DS1302InputByte(ucAddr|0x01);        // ��ַ������ 
    ucData = DS1302OutputByte();         // ��1Byte����
    DS1302_CLK = 0;
    DS1302_RST = 0;
    return(ucData);
}

static void DS1302_SetProtect(unsigned char flag)        //�Ƿ�д����
{
 if(flag)
  Write1302(0x8E,0x80);
 else
  Write1302(0x8E,0x00);
}
/*
********************************************************************************
* ��������	����ʼ��DS1302��ǰ����Ӧ��IO�ڿ���
********************************************************************************
*/
void Initial_DS1302(void)
{
	DS1302_SetProtect(0);
	Write1302(0x90,0xA5);				//�򿪳�繦�ܣ�һ��������+2K�����裩
	Write1302(0x84,Read1302(0x85)&0x7F);//��Ϊ24H��
	Write1302(0x80,Read1302(0x81)&0x7F);//ȡ����ͣ
	DS1302_SetProtect(1);
}

/*
********************************************************************************
* ��������	������DS1302ʱ��
* ����		��
*	time�������õ�ʱ��
********************************************************************************
*/
void DS1302_SetTime(SYSTEMTIME time)
{
	uint8_t err;
	
	OSSemPend(DS1302_Device,0,&err);//�ȴ�
	DS1302_SetProtect(0);
	if(time.Second<60)
		 Write1302(0x80, ((((time.Second/10)<<4)&0x70) | (time.Second%10))); 
	if(time.Minute<60)
		 Write1302(0x82, ((((time.Minute/10)<<4)&0x70) | (time.Minute%10))); 
	if(time.Hour<24)
		 Write1302(0x84, ((((time.Hour/10)<<4)&0x30) | (time.Hour%10)));
	if(time.Day<32)
		 Write1302(0x86, ((((time.Day/10)<<4)&0x30) | (time.Day%10))); 
	if(time.Month<13)
		 Write1302(0x88, ((time.Month/10)<<4 | (time.Month%10))); 
	if(time.Year<99)
		 Write1302(0x8C, ((time.Year/10)<<4 | (time.Year%10))); 
	DS1302_SetProtect(1);
	OSSemPost(DS1302_Device);
}

/*
********************************************************************************
* ��������	������DS1302ʱ��
* ����		��
*	Time��������ʱ��
********************************************************************************
*/
void DS1302_GetTime(SYSTEMTIME *Time)
{
	uint8_t err;
	unsigned char ReadValue;
	
	OSSemPend(DS1302_Device,0,&err);//�ȴ�
	
	ReadValue = Read1302(DS1302_HOUR);
	Time->Hour = ((ReadValue&0x30)>>4)*10 + (ReadValue&0x0f);
	
	ReadValue = Read1302(DS1302_MINUTE);
	Time->Minute = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0f);
	
	
	ReadValue = Read1302(DS1302_SECOND);
	Time->Second = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0f);
	
	
	ReadValue = Read1302(DS1302_DAY);
	Time->Day = ((ReadValue&0x30)>>4)*10 + (ReadValue&0x0f); 
	
	ReadValue = Read1302(DS1302_MONTH);
	Time->Month = ((ReadValue&0x10)>>4)*10 + (ReadValue&0x0f);
	
	ReadValue = Read1302(DS1302_YEAR);
	Time->Year = ((ReadValue&0xf0)>>4)*10 + (ReadValue&0x0f);
	
	OSSemPost(DS1302_Device);
}

/*
********************************************************************************
* ��������	����DS1302��RAMд����
* ����		��
*	Buffer	����д�뻺��
*	Addr	��0-30
*	N		��д���ֽ���
********************************************************************************
*/
void Write1302RAM(unsigned char *Buffer,unsigned char Addr,unsigned char N)
{
	uint8_t err;
    unsigned char i;
	if(Addr+N>31)return;
    
	OSSemPend(DS1302_Device,0,&err);//�ȴ�
	DS1302_SetProtect(0);          // ��������,WP=0,д����
	
	for (i = N; i>0; i--){
		DS1302_RST = 0;  
		DS1302_CLK = 0;
		DS1302_RST = 1;
		Delay_NOP(dly);
		DS1302InputByte(0xC0|(Addr++)<<1);         // RAM��ַ��ʼ��ַ
		DS1302InputByte(*Buffer);  // д1Byte����
		Buffer++;
		DS1302_CLK = 0;
		DS1302_RST = 0;
	}
	
	DS1302_SetProtect(1);
	OSSemPost(DS1302_Device);
}
/*
********************************************************************************
* ��������	����DS1302��RAMд����
* ����		��
*	Buffer	�����뻺��
*	Addr	��0-30
*	N		�������ֽ���
********************************************************************************
*/
void Read1302RAM(unsigned char *Buffer,unsigned char Addr,unsigned char N)
{
	uint8_t err;
	
	if(Addr+N>31)return;
	
	OSSemPend(DS1302_Device,0,&err);//�ȴ�
	for(; N>0; N--){
		DS1302_RST = 0;
		DS1302_CLK = 0;
		DS1302_RST = 1;
		Delay_NOP(dly);
		DS1302InputByte(0xC1|(Addr++)<<1);         // RAM��ַ��ʼ��ַ
		*Buffer = DS1302OutputByte();   // ��1Byte����
		Buffer++;
		DS1302_CLK = 0;
		DS1302_RST = 0;
	}
	OSSemPost(DS1302_Device);
}

//void DS1302_TimeStop(bit flag)           // �Ƿ�ʱ��ֹͣ
//{
// unsigned char Data;
// Data=Read1302(DS1302_SECOND);
// DS1302_SetProtect(0);
// if(flag)
//  Write1302(DS1302_SECOND, Data|0x80);
// else
//  Write1302(DS1302_SECOND, Data&0x7F);
//}