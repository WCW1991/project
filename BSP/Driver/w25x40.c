#include <w25x40.h>
#include    <ucos_ii.h>

extern OS_EVENT *W25X40_Device;

uint8_t SPI_FLASH_BUF[4096];

//W25Xϵ��/Qϵ��оƬ�б�
#define W25X10  0XEF10    
#define W25X20  0XEF11  
#define W25X40  0XEF12  
#define W25X80  0XEF13  
  
#define SPI_FLASH_CS PCout(4)  //ѡ��FLASH     
  
////////////////////////////////////////////////////////////////////////////  
  
//ָ���  
#define W25X_WriteEnable        0x06    //дʹ��  
#define W25X_WriteDisable       0x04    //д��ֹ  
#define W25X_ReadStatusReg      0x05    //��״̬�Ĵ���  
#define W25X_WriteStatusReg     0x01    //д״̬�Ĵ���  
#define W25X_ReadData           0x03    //������  
#define W25X_FastReadData       0x0B    //���ٶ�����  
#define W25X_FastReadDual       0x3B    //�����첽������  
#define W25X_PageProgram        0x02    //ҳ���  
#define W25X_BlockErase         0xD8    //�����  
#define W25X_SectorErase        0x20    //��������  
#define W25X_ChipErase          0xC7    //Ƭ����  
#define W25X_PowerDown          0xB9    //�ػ�  
#define W25X_ReleasePowerDown	0xAB    //�ͷŹػ�  
#define W25X_DeviceID           0xAB    //��ȡ�豸ID  
#define W25X_ManufactDeviceID	0x90    //  
#define W25X_JedecDeviceID      0x9F    //  

#pragma inline
static uint8_t Spi2ReadWriteByte( uint8_t Arg ){
	uint16_t retry=0;
	while((SPI1->SR&1<<1)==0){
		retry++;
		if(retry>2000)return 0;
	}			  
	SPI1->DR=Arg;
	retry=0;
	while((SPI1->SR&1<<0)==0)
	{
		retry++;
		if(retry>2000)return 0;
	}	  						    
	return SPI1->DR;
}

//��ȡSPI_FLASH��״̬�Ĵ���  
//BIT7  6   5   4   3   2   1   0  
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY  
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��  
//TB,BP2,BP1,BP0:FLASH����д��������  
//WEL:дʹ������  
//BUSY:æ���λ(1,æ;0,����)  
//Ĭ��:0x00  
static uint8_t SpiFlashReadSR(void)
{    
    uint8_t byte=0;     
    SPI_FLASH_CS=0;                            //ʹ������     
    Spi2ReadWriteByte(W25X_ReadStatusReg);     //���Ͷ�ȡ״̬�Ĵ�������      
    byte=Spi2ReadWriteByte(0Xff);              //��ȡһ���ֽ�    
    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ       
    return byte;                              
}   
  
//дSPI_FLASH״̬�Ĵ���  
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!  
//static void SpiFlashWriteSR(uint8_t sr)
//{     
//    SPI_FLASH_CS=0;                            //ʹ������     
//    Spi2ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������      
//    Spi2ReadWriteByte(sr);               //д��һ���ֽ�    
//    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ               
//}   
    
//SPI_FLASHдʹ��    
//��WEL��λ     
static void SpiFlashWriteEnable(void)
{  
    SPI_FLASH_CS=0;                            //ʹ������     
    Spi2ReadWriteByte(W25X_WriteEnable);      //����дʹ��    
    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ               
}   
  
//SPI_FLASHд��ֹ    
//��WEL����    
static void SpiFlashWriteDisable(void)
{
    SPI_FLASH_CS=0;                            //ʹ������     
    Spi2ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��      
    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ               
}
//�ȴ�����  
static void SpiFlashWaitBusy(void)     
{     
    while ((SpiFlashReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���  
}

void spiflah_init(void)
{
	SpiFlashWriteDisable();
}

//flash��� ��⵽����0 ʧ�ܷ���1  
ErrorStatus SpiFlashCheck(void)
{
    uint16_t ReadId = 0;
	SPI_FLASH_CS=0;
    Spi2ReadWriteByte(0x90);//���Ͷ�ȡID����
    Spi2ReadWriteByte(0x00);
    Spi2ReadWriteByte(0x00);
    Spi2ReadWriteByte(0x00);
    ReadId|=((uint16_t)Spi2ReadWriteByte(0xFF))<<8;
    ReadId|=Spi2ReadWriteByte(0xFF);
    SPI_FLASH_CS=1;
	if(ReadId == W25X40){
        return SUCCESS;  
	}else{
        return ERROR;
	}
}

//��ȡSPI FLASH    
//��ָ����ַ��ʼ��ȡָ�����ȵ�����  
//pBuffer:���ݴ洢��  
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)  
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SpiFlashRead(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{   
    uint16_t i;
	uint8_t err;
	OSSemPend(W25X40_Device,0,&err);//�ȴ�
    SPI_FLASH_CS=0;                            //ʹ������     
    Spi2ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����     
    Spi2ReadWriteByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ      
    Spi2ReadWriteByte((uint8_t)((ReadAddr)>>8));     
    Spi2ReadWriteByte((uint8_t)ReadAddr);     
    for(i=0;i<NumByteToRead;i++)  
    {   
        pBuffer[i]=Spi2ReadWriteByte(0XFF);   //ѭ������    
    }
    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ
	SpiFlashWaitBusy();
	OSSemPost(W25X40_Device);
}
  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����  
//��ָ����ַ��ʼд�����256�ֽڵ�����  
//pBuffer:���ݴ洢��  
//WriteAddr:��ʼд��ĵ�ַ(24bit)  
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
static void SPIFlashWritePage(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{  
    uint16_t i;
	SpiFlashWriteEnable();
    SPI_FLASH_CS=0;                            //ʹ������     
    Spi2ReadWriteByte(W25X_PageProgram);      //����дҳ����     
    Spi2ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ      
    Spi2ReadWriteByte((uint8_t)((WriteAddr)>>8));     
    Spi2ReadWriteByte((uint8_t)WriteAddr);     
    for(i=0;i<NumByteToWrite;i++)Spi2ReadWriteByte(pBuffer[i]);//ѭ��д��    
    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ
    SpiFlashWaitBusy();                    //�ȴ�д�����
	SpiFlashWriteDisable();
}   
  
//�޼���дSPI FLASH   
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!  
//�����Զ���ҳ����   
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!  
//pBuffer:���ݴ洢��  
//WriteAddr:��ʼд��ĵ�ַ(24bit)  
//NumByteToWrite:Ҫд����ֽ���(���65535)  
//CHECK OK  
void SpiFlashWriteNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{                      
    uint16_t pageremain;
	uint8_t err;
	
	OSSemPend(W25X40_Device,0,&err);//�ȴ�	
    pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���                  
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�  
    while(1)  
    {
        SPIFlashWritePage(pBuffer,WriteAddr,pageremain);  
        if(NumByteToWrite==pageremain)break;//д�������  
        else //NumByteToWrite>pageremain  
        {
            pBuffer+=pageremain;  
            WriteAddr+=pageremain;
            NumByteToWrite-=pageremain;           //��ȥ�Ѿ�д���˵��ֽ���  
            if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�  
            else pageremain=NumByteToWrite;       //����256���ֽ���  
        }
		SpiFlashWaitBusy();
    }
	OSSemPost(W25X40_Device);
}
  
//дSPI FLASH    
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!  
//pBuffer:���ݴ洢��  
//WriteAddr:��ʼд��ĵ�ַ(24bit)  
//NumByteToWrite:Ҫд����ֽ���(���65535)            
void SpiFlashWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint32_t secpos;  
    uint16_t secoff;  
    uint16_t secremain;       
    uint16_t i;
    secpos=WriteAddr/4096;//������ַ 0~128 for w25x40
    secoff=WriteAddr%4096;//�������ڵ�ƫ��  
    secremain=4096-secoff;//����ʣ��ռ��С     
      
    if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�  
    while(1)   
    {
        SpiFlashRead(SPI_FLASH_BUF,secpos*4096,4096);//������������������
        for(i=0;i<secremain;i++)//У������  
        {
            if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����       
        }
        if(i<secremain)//��Ҫ����  
        {
            SpiFlashEraseSector(secpos);//�����������  
            for(i=0;i<secremain;i++)    //����
            {  
                SPI_FLASH_BUF[i+secoff]=pBuffer[i];     
            }  
            SpiFlashWriteNoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������
        }else SpiFlashWriteNoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.                    
        if(NumByteToWrite==secremain)//д�������  
		{
			break;
		}
        else//д��δ����  
        {
            secpos++;//������ַ��1  
            secoff=0;//ƫ��λ��Ϊ0      
              
            pBuffer+=secremain;  //ָ��ƫ��  
            WriteAddr+=secremain;//д��ַƫ��         
            NumByteToWrite-=secremain;              //�ֽ����ݼ�  
            if(NumByteToWrite>4096)secremain=4096;   //��һ����������д����  
            else secremain=NumByteToWrite;          //��һ����������д����  
        }
    }
}
  
//��������оƬ  
//��Ƭ����ʱ��:  
//W25X16:25s   
//W25X32:40s   
//W25X64:40s   
//�ȴ�ʱ�䳬��...  
void SpiFlashEraseChip(void)
{
	uint8_t err;
	
	OSSemPend(W25X40_Device,0,&err);//�ȴ�
	SpiFlashWriteEnable();                  //SET WEL   
    SpiFlashWaitBusy();     
    SPI_FLASH_CS=0;                            //ʹ������     
    Spi2ReadWriteByte(W25X_ChipErase);        //����Ƭ��������    
    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ
    SpiFlashWaitBusy();                    //�ȴ�оƬ��������
	SpiFlashWriteDisable();
	OSSemPost(W25X40_Device);
}     
  
//����һ������  
//Dst_Addr:������ַ 0~127 for w25x40
//����һ��ɽ��������ʱ��:150ms  
void SpiFlashEraseSector(uint32_t Dst_Addr)
{
	uint8_t err;
	
	OSSemPend(W25X40_Device,0,&err);//�ȴ�
    Dst_Addr*=4096;  
    SpiFlashWriteEnable();                  //SET WEL      
    SpiFlashWaitBusy();
    SPI_FLASH_CS=0;                            //ʹ������     
    Spi2ReadWriteByte(W25X_SectorErase);      //������������ָ��   
    Spi2ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ      
    Spi2ReadWriteByte((uint8_t)((Dst_Addr)>>8));     
    Spi2ReadWriteByte((uint8_t)Dst_Addr);    
    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ               
    SpiFlashWaitBusy();                    //�ȴ��������
	SpiFlashWriteDisable();
	OSSemPost(W25X40_Device);
}
  
////�������ģʽ  
//void SpiFlashPowerDown(void)
//{   
//    SPI_FLASH_CS=0;                            //ʹ������     
//    Spi2ReadWriteByte(W25X_PowerDown);        //���͵�������    
//    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ
//}
//
////����  
//void SpiFlashWakeUp(void)
//{
//    SPI_FLASH_CS=0;                            //ʹ������     
//    Spi2ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB      
//    SPI_FLASH_CS=1;                            //ȡ��Ƭѡ
//}