#include "pymb.h"
#include "pyinput.h"
#include "string.h"
#include "Key.h"
#include <includes.h>
//////////////////////////////////////////////////////////////////////////////////
typedef char HzString[3];
void cleanline(u8 line);
u8 mb2Buffer(u8* mb,HzString Buffer[24][7]);
void T9_Py_Display(u8 MatchMun,u8 py_cursor);
void T9_Hz_Display(HzString Buffer[24][7],u8 Group_cursor, u8 Hz_cursor);
//ƴ�����뷨
pyinput t9=
{
	get_pymb,
    0,
};

//�Ƚ������ַ�����ƥ�����
//����ֵ:0xff,��ʾ��ȫƥ��.
//		 ����,ƥ����ַ���
u8 str_match(u8*str1,u8*str2)
{
	u8 i=0;
	while(1)
	{
		if(*str1!=*str2)break;		  //����ƥ��
		if(*str1=='\0'){i=0XFF;break;}//��ȫƥ��
		i++;
		str1++;
		str2++;
	}
	return i;//�����ַ������
}

//��ȡƥ���ƴ�����
//*strin,������ַ���,����:"726"
//**matchlist,�����ƥ���.
//����ֵ:[7],0,��ʾ��ȫƥ�䣻1����ʾ����ƥ�䣨����û����ȫƥ���ʱ��Ż���֣�
//		 [6:0],��ȫƥ���ʱ�򣬱�ʾ��ȫƥ���ƴ������
//			   ����ƥ���ʱ�򣬱�ʾ��Чƥ���λ��				    	 
u8 get_matched_pymb(u8 *strin,py_index **matchlist)
{
	py_index *bestmatch;//���ƥ��
	u16 pyindex_len;
	u16 i;
	u8 temp,mcnt=0,bmcnt=0;
	bestmatch=(py_index*)&py_index3[0];//Ĭ��Ϊa��ƥ��
	pyindex_len=sizeof(py_index3)/sizeof(py_index3[0]);//�õ�py������Ĵ�С.
	for(i=0;i<pyindex_len;i++)
	{
		temp=str_match(strin,(u8*)py_index3[i].py_input);
		if(temp)
		{
			if(temp==0XFF)matchlist[mcnt++]=(py_index*)&py_index3[i];
			else if(temp>bmcnt)//�����ƥ��
			{
				bmcnt=temp;
			    bestmatch=(py_index*)&py_index3[i];//��õ�ƥ��.
			}
		}
	}
	if(mcnt==0&&bmcnt)//û����ȫƥ��Ľ��,�����в���ƥ��Ľ��
	{
		matchlist[0]=bestmatch;
		mcnt=bmcnt|0X80;		//���ز���ƥ�����Чλ��
	}
	return mcnt;//����ƥ��ĸ���
}

//�õ�ƴ�����.
//str:�����ַ���
//����ֵ:ƥ�����.
u8 get_pymb(u8* str)
{
	return get_matched_pymb(str,t9.pymb);
}



//���أ�����
u8 mb2Buffer(u8* mb,HzString Buffer[24][7])
{
	u8 Group=0,Index=0,i=0,j=0,k=0;
	for(i=0;i<24;i++)
		for(j=0;j<7;j++)
			for(k=0;k<3;k++)
				Buffer[i][j][k]=0;
	while(*mb)
	{
		if(Index<7)
		{
			for(i=0;i<3;i++)
			{
				if(i<2)
					Buffer[Group][Index][i]=*(mb++);
				else
					Buffer[Group][Index][i]=0;
			}
			Index++;//һ����ת�����
		}
		else
		{
			Group++;//��һ��
			Index=0;
			for(i=0;i<3;i++)
			{
				if(i<2)
					Buffer[Group][Index][i]=*(mb++);
				else
					Buffer[Group][Index][i]=0;
			}
			Index++;//һ����ת�����
		}
	}
	return Group+1;
}

#if MAIN_BOARD==2
void PinYin_Input(char *InputHz)
{
	uint8_t  err;
	Key_enum *My_Key_Message;
	//char *pInputBuffer=InputHz;
	u8 InputHzByteLen=0;
	
	u8 inputstr[7]={0,0,0,0,0,0,0};		//�������6���ַ�+������
	u8 inputlen=0;
	
	enum{PinYin,ShuZi,ABC}Change=PinYin;
	
	u8 ZiMu='A';
	
	HzString Buffer[24][7];
	
	
	char geshu[7]={0,0,0,0,0,0,0};
	
	u8 Group_cursor=0;
	u8 MatchMun=0,py_cursor=0,Hz_cursor=0;
	u8 i=0,Group_total=0;
	
	InputHzByteLen=strlen((const char *)InputHz);
	for(i=InputHzByteLen;i<17;i++)
		InputHz[i]=0;
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "������������Ϣ��", 0, 0, 1, ScreenBuffer );
	do{//��������״̬
		sprintf(geshu,"%2d",InputHzByteLen);
		StringDisplay( (u8*)geshu, 0, 130, 0, ScreenBuffer );
		switch(Change){
			case PinYin:{
				StringDisplay( "ƴ��", 0, 168, 0, ScreenBuffer );
				MatchMun=t9.getpymb(inputstr);
				if(MatchMun&0X80)
					MatchMun=1;
				
				T9_Py_Display(MatchMun,py_cursor);
				Group_total=mb2Buffer(t9.pymb[py_cursor]->pymb,Buffer);//��ƴ����Ӧ�����ת��Ϊ����������
				T9_Hz_Display(Buffer,Group_cursor,Hz_cursor);//���ݲ�����Buffer�������ʾ����
			}break;
			case ShuZi:{
				StringDisplay( "����", 0, 168, 0, ScreenBuffer );
				if(inputlen==1&&InputHzByteLen<=16){
					InputHz[InputHzByteLen++]=inputstr[0];
					inputstr[0]=0;
					inputlen=0;
				}
			}break;
			case ABC:{
				StringDisplay( "��ĸ", 0, 168, 0, ScreenBuffer );
				inputstr[0]=ZiMu;
				inputstr[1]=0;
				inputlen=1;
				StringDisplay( "����ѡ��", 4, 0, 0, ScreenBuffer );
				StringDisplay( inputstr, 4, 56, 1, ScreenBuffer );
				
			}break;
			default:break;
		}
		cleanline(2);
		StringDisplay((uint8_t*)InputHz, 2, 12, 0, ScreenBuffer );//��ʾ����InputHz
		
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,0,&err);//�ȴ�����
		switch(*My_Key_Message)
		{
			//���»���
			case Key_Up:{
				if(Change!=ABC){
					if(Group_cursor>0)Group_cursor--;
				}else{
					if(ZiMu>'A')
						ZiMu--;
				}
			}break;
			case Key_Down:{
				if(Change!=ABC){
					if(Group_cursor<(Group_total-1))Group_cursor++;
				}else{
					if(ZiMu<'Z')
						ZiMu++;
				}
			}break;
			
			//����ѡ��
			case Key_Lift:{if(Hz_cursor>0)Hz_cursor--;}break;
			case Key_Right:{if(Hz_cursor<6)Hz_cursor++;}break;
			
			//*���л�ƴ��
			case Key_x:{//����ں���ʱ���¼�����Ч
				if(py_cursor==(MatchMun-1))
					py_cursor=0;
				else
					py_cursor++;
				Group_cursor=0;
				Hz_cursor=0;
			}break;
			
			//������ֲ��������
			case Key_Set:{
				if(inputlen==0){
					return;
				}
				if(Change==PinYin){
					if(InputHzByteLen<=15){
						for(i=0;i<2;i++)
							InputHz[InputHzByteLen++]=Buffer[Group_cursor][Hz_cursor][i];
					}
				}
				if(Change==ABC){
					InputHz[InputHzByteLen++]=ZiMu;
				}
				inputlen=0;
				for(i=0;i<7;i++)inputstr[i]=0;
				py_cursor=0;
				Group_cursor=0;
				Hz_cursor=0;
				ZiMu='A';
			}break;
			
			case Key_Return:{return;}
			
			//�л�����
			case Key_j:{
				if(Change==PinYin)
					Change=ShuZi;
				else
					if(Change==ShuZi)
						Change=ABC;
					else
						Change=PinYin;
				inputlen=0;
				for(i=0;i<7;i++)inputstr[i]=0;
				py_cursor=0;
				Group_cursor=0;
				Hz_cursor=0;
				ZiMu='A';
				continue;
			}
			//�������
			case Key_Delete:{
				if(inputlen==0&&InputHzByteLen>0){
					if(InputHz[InputHzByteLen-1]>=0xA1){//ɾ������
						InputHz[InputHzByteLen-2]=0;
						InputHz[InputHzByteLen-1]=0;
						InputHzByteLen-=2;
					}else{//ɾ��ASCII
						InputHz[InputHzByteLen-1]=0;
						InputHzByteLen-=1;
					}
				}else{
					inputlen=0;
					for(i=0;i<7;i++)inputstr[i]=0;
					py_cursor=0;
					Group_cursor=0;
					Hz_cursor=0;
				}
			}break;
			case Key_0:{
				if(Change!=ABC)
				if(Change==ShuZi){
					inputstr[inputlen]=0+'0';//�����ַ�
					if(inputlen<7)inputlen++;
				}
			}break;
			case Key_1:{
				if(Change!=ABC)
				if(Change==ShuZi){
					inputstr[inputlen]=1+'0';//�����ַ�
					if(inputlen<7)inputlen++;
				}
			}break;
			case Key_2:{
				if(Change!=ABC){
				inputstr[inputlen]=2+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_3:{
				if(Change!=ABC){
				inputstr[inputlen]=3+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_4:{
				if(Change!=ABC){
				inputstr[inputlen]=4+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_5:{
				if(Change!=ABC){
				inputstr[inputlen]=5+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_6:{
				if(Change!=ABC){
				inputstr[inputlen]=6+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_7:{
				if(Change!=ABC){
				inputstr[inputlen]=7+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_8:{
				if(Change!=ABC){
				inputstr[inputlen]=8+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_9:{
				if(Change!=ABC){
				inputstr[inputlen]=9+'0';//�����ַ�
				if(inputlen<7)inputlen++;
				}
			}break;
			default:break;
		}
	}while(1);
}
#endif

void T9_Py_Display(u8 MatchMun,u8 py_cursor)
{
	u8 i=0;
	u8 pyStartAddr=6;
	//ƴ��
	cleanline(4);
	for(i=0;i<MatchMun;i++)
	{
		uint8_t Reverse=0;
		if(i==py_cursor)
			Reverse=1;
		else
			Reverse=0;
		StringDisplay( t9.pymb[i]->py, 4, pyStartAddr, Reverse, ScreenBuffer );//��ʾƴ��
		
		pyStartAddr+=strlen((const char *) t9.pymb[i]->py)*6+6;//������һƴ����ʼ��ַ
	}
}

void T9_Hz_Display(HzString Buffer[24][7],u8 Group_cursor, u8 Hz_cursor)
{
	u8 i=0;
	cleanline(6);
	for(i=0;i<7;i++)
	{//����ѭ��
		uint8_t Reverse=0;
		if(i==Hz_cursor)
			Reverse=1;
		else
			Reverse=0;
		StringDisplay( (uint8_t*)Buffer[Group_cursor][i], 6, i*12, Reverse, ScreenBuffer );//��ʾ����
	}
}

void cleanline(u8 line)
{
	u8 x,y;
	if(line==6){
		for(y=line;y<line+2;y++)
			for(x=0;x<192;x++)
				ScreenBuffer[y][x]=0;
	}
	for(y=line;y<line+2;y++)
		for(x=0;x<192;x++)
			ScreenBuffer[y][x]=0;
}

void QuWeiInput(char *InputHz)
{
	Key_enum Key=Key_None;
	u8 InputHzByteLen=0;
	u8 Qu=0,Wei=0,i=0;
	enum{qu,wei}C=qu;
	
	InputHzByteLen=strlen((const char *)InputHz);
	for(i=InputHzByteLen;i<17;i++)
		InputHz[i]=0;
	
	Clean_Lcd(ScreenBuffer);
	StringDisplay( "������������Ϣ��", 0, 0, 1, ScreenBuffer );
	StringDisplay( "����", 4, 0, 0, ScreenBuffer );
	StringDisplay( "λ��", 6, 0, 0, ScreenBuffer );
	StringDisplay((uint8_t*)InputHz, 2, 12, 0, ScreenBuffer );//��ʾ����InputHz
	while(1){
		DisplayValueN(Qu, 4, 22, 0, 2, ScreenBuffer);
		DisplayValueN(Wei, 6, 22, 0, 2, ScreenBuffer);
		if(C==qu){
			Key=input_value8(&Qu, 2, 4, 22);
		}else{
			Key=input_value8(&Wei, 2, 6, 22);
		}
		switch(Key){
			case Key_Set:{
				if(InputHzByteLen<=15&&Qu!=0&&Wei!=0){
					InputHz[InputHzByteLen++]=Qu+0xa0;
					InputHz[InputHzByteLen++]=Wei+0xa0;
				}
				Qu=0;
				Wei=0;
			}break;
			case Key_Delete:{
				if(InputHzByteLen>0){
					if(C==qu){
						if(Qu!=0)
							Qu=0;
						else{
							InputHz[--InputHzByteLen]=0;
							InputHz[--InputHzByteLen]=0;
						}
					}else{
						if(Wei!=0)
							Wei=0;
						else{
							InputHz[--InputHzByteLen]=0;
							InputHz[--InputHzByteLen]=0;
						}
					}
				}
			}break;
			case Key_x:{
				if(C==qu){
					C=wei;
				}else{
					C=qu;
				}
			}break;
			case Key_Return:return;
		}
		cleanline(2);
		StringDisplay((uint8_t*)InputHz, 2, 12, 0, ScreenBuffer );//��ʾ����InputHz
	}
}



























