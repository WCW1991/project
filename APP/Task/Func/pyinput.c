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
//拼音输入法
pyinput t9=
{
	get_pymb,
    0,
};

//比较两个字符串的匹配情况
//返回值:0xff,表示完全匹配.
//		 其他,匹配的字符数
u8 str_match(u8*str1,u8*str2)
{
	u8 i=0;
	while(1)
	{
		if(*str1!=*str2)break;		  //部分匹配
		if(*str1=='\0'){i=0XFF;break;}//完全匹配
		i++;
		str1++;
		str2++;
	}
	return i;//两个字符串相等
}

//获取匹配的拼音码表
//*strin,输入的字符串,形如:"726"
//**matchlist,输出的匹配表.
//返回值:[7],0,表示完全匹配；1，表示部分匹配（仅在没有完全匹配的时候才会出现）
//		 [6:0],完全匹配的时候，表示完全匹配的拼音个数
//			   部分匹配的时候，表示有效匹配的位数				    	 
u8 get_matched_pymb(u8 *strin,py_index **matchlist)
{
	py_index *bestmatch;//最佳匹配
	u16 pyindex_len;
	u16 i;
	u8 temp,mcnt=0,bmcnt=0;
	bestmatch=(py_index*)&py_index3[0];//默认为a的匹配
	pyindex_len=sizeof(py_index3)/sizeof(py_index3[0]);//得到py索引表的大小.
	for(i=0;i<pyindex_len;i++)
	{
		temp=str_match(strin,(u8*)py_index3[i].py_input);
		if(temp)
		{
			if(temp==0XFF)matchlist[mcnt++]=(py_index*)&py_index3[i];
			else if(temp>bmcnt)//找最佳匹配
			{
				bmcnt=temp;
			    bestmatch=(py_index*)&py_index3[i];//最好的匹配.
			}
		}
	}
	if(mcnt==0&&bmcnt)//没有完全匹配的结果,但是有部分匹配的结果
	{
		matchlist[0]=bestmatch;
		mcnt=bmcnt|0X80;		//返回部分匹配的有效位数
	}
	return mcnt;//返回匹配的个数
}

//得到拼音码表.
//str:输入字符串
//返回值:匹配个数.
u8 get_pymb(u8* str)
{
	return get_matched_pymb(str,t9.pymb);
}



//返回：组数
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
			Index++;//一个字转换完成
		}
		else
		{
			Group++;//下一组
			Index=0;
			for(i=0;i<3;i++)
			{
				if(i<2)
					Buffer[Group][Index][i]=*(mb++);
				else
					Buffer[Group][Index][i]=0;
			}
			Index++;//一个字转换完成
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
	
	u8 inputstr[7]={0,0,0,0,0,0,0};		//最大输入6个字符+结束符
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
	StringDisplay( "请输入描述信息：", 0, 0, 1, ScreenBuffer );
	do{//进入输入状态
		sprintf(geshu,"%2d",InputHzByteLen);
		StringDisplay( (u8*)geshu, 0, 130, 0, ScreenBuffer );
		switch(Change){
			case PinYin:{
				StringDisplay( "拼音", 0, 168, 0, ScreenBuffer );
				MatchMun=t9.getpymb(inputstr);
				if(MatchMun&0X80)
					MatchMun=1;
				
				T9_Py_Display(MatchMun,py_cursor);
				Group_total=mb2Buffer(t9.pymb[py_cursor]->pymb,Buffer);//把拼音对应的码表转化为单个字数组
				T9_Hz_Display(Buffer,Group_cursor,Hz_cursor);//根据参数把Buffer里的字显示出来
			}break;
			case ShuZi:{
				StringDisplay( "数字", 0, 168, 0, ScreenBuffer );
				if(inputlen==1&&InputHzByteLen<=16){
					InputHz[InputHzByteLen++]=inputstr[0];
					inputstr[0]=0;
					inputlen=0;
				}
			}break;
			case ABC:{
				StringDisplay( "字母", 0, 168, 0, ScreenBuffer );
				inputstr[0]=ZiMu;
				inputstr[1]=0;
				inputlen=1;
				StringDisplay( "上下选择：", 4, 0, 0, ScreenBuffer );
				StringDisplay( inputstr, 4, 56, 1, ScreenBuffer );
				
			}break;
			default:break;
		}
		cleanline(2);
		StringDisplay((uint8_t*)InputHz, 2, 12, 0, ScreenBuffer );//显示汉字InputHz
		
		My_Key_Message = OSMboxPend(Key_Mbox_Handle,0,&err);//等待按键
		switch(*My_Key_Message)
		{
			//上下换组
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
			
			//左右选字
			case Key_Lift:{if(Hz_cursor>0)Hz_cursor--;}break;
			case Key_Right:{if(Hz_cursor<6)Hz_cursor++;}break;
			
			//*号切换拼音
			case Key_x:{//光标在汉字时上下键才有效
				if(py_cursor==(MatchMun-1))
					py_cursor=0;
				else
					py_cursor++;
				Group_cursor=0;
				Hz_cursor=0;
			}break;
			
			//捡出汉字并清空输入
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
			
			//切换输入
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
			//清空输入
			case Key_Delete:{
				if(inputlen==0&&InputHzByteLen>0){
					if(InputHz[InputHzByteLen-1]>=0xA1){//删除汉字
						InputHz[InputHzByteLen-2]=0;
						InputHz[InputHzByteLen-1]=0;
						InputHzByteLen-=2;
					}else{//删除ASCII
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
					inputstr[inputlen]=0+'0';//输入字符
					if(inputlen<7)inputlen++;
				}
			}break;
			case Key_1:{
				if(Change!=ABC)
				if(Change==ShuZi){
					inputstr[inputlen]=1+'0';//输入字符
					if(inputlen<7)inputlen++;
				}
			}break;
			case Key_2:{
				if(Change!=ABC){
				inputstr[inputlen]=2+'0';//输入字符
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_3:{
				if(Change!=ABC){
				inputstr[inputlen]=3+'0';//输入字符
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_4:{
				if(Change!=ABC){
				inputstr[inputlen]=4+'0';//输入字符
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_5:{
				if(Change!=ABC){
				inputstr[inputlen]=5+'0';//输入字符
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_6:{
				if(Change!=ABC){
				inputstr[inputlen]=6+'0';//输入字符
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_7:{
				if(Change!=ABC){
				inputstr[inputlen]=7+'0';//输入字符
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_8:{
				if(Change!=ABC){
				inputstr[inputlen]=8+'0';//输入字符
				if(inputlen<7)inputlen++;
				}
			}break;
			case Key_9:{
				if(Change!=ABC){
				inputstr[inputlen]=9+'0';//输入字符
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
	//拼音
	cleanline(4);
	for(i=0;i<MatchMun;i++)
	{
		uint8_t Reverse=0;
		if(i==py_cursor)
			Reverse=1;
		else
			Reverse=0;
		StringDisplay( t9.pymb[i]->py, 4, pyStartAddr, Reverse, ScreenBuffer );//显示拼音
		
		pyStartAddr+=strlen((const char *) t9.pymb[i]->py)*6+6;//计算下一拼音起始地址
	}
}

void T9_Hz_Display(HzString Buffer[24][7],u8 Group_cursor, u8 Hz_cursor)
{
	u8 i=0;
	cleanline(6);
	for(i=0;i<7;i++)
	{//字数循环
		uint8_t Reverse=0;
		if(i==Hz_cursor)
			Reverse=1;
		else
			Reverse=0;
		StringDisplay( (uint8_t*)Buffer[Group_cursor][i], 6, i*12, Reverse, ScreenBuffer );//显示汉字
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
	StringDisplay( "请输入描述信息：", 0, 0, 1, ScreenBuffer );
	StringDisplay( "区：", 4, 0, 0, ScreenBuffer );
	StringDisplay( "位：", 6, 0, 0, ScreenBuffer );
	StringDisplay((uint8_t*)InputHz, 2, 12, 0, ScreenBuffer );//显示汉字InputHz
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
		StringDisplay((uint8_t*)InputHz, 2, 12, 0, ScreenBuffer );//显示汉字InputHz
	}
}



























