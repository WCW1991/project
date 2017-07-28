#include <w25x40.h>
#include    <ucos_ii.h>

extern OS_EVENT *W25X40_Device;

uint8_t SPI_FLASH_BUF[4096];

//W25X系列/Q系列芯片列表
#define W25X10  0XEF10    
#define W25X20  0XEF11  
#define W25X40  0XEF12  
#define W25X80  0XEF13  
  
#define SPI_FLASH_CS PCout(4)  //选中FLASH     
  
////////////////////////////////////////////////////////////////////////////  
  
//指令表  
#define W25X_WriteEnable        0x06    //写使能  
#define W25X_WriteDisable       0x04    //写禁止  
#define W25X_ReadStatusReg      0x05    //读状态寄存器  
#define W25X_WriteStatusReg     0x01    //写状态寄存器  
#define W25X_ReadData           0x03    //读数据  
#define W25X_FastReadData       0x0B    //快速读数据  
#define W25X_FastReadDual       0x3B    //快速异步读数据  
#define W25X_PageProgram        0x02    //页编程  
#define W25X_BlockErase         0xD8    //块擦除  
#define W25X_SectorErase        0x20    //扇区擦除  
#define W25X_ChipErase          0xC7    //片擦除  
#define W25X_PowerDown          0xB9    //关机  
#define W25X_ReleasePowerDown	0xAB    //释放关机  
#define W25X_DeviceID           0xAB    //读取设备ID  
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

//读取SPI_FLASH的状态寄存器  
//BIT7  6   5   4   3   2   1   0  
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY  
//SPR:默认0,状态寄存器保护位,配合WP使用  
//TB,BP2,BP1,BP0:FLASH区域写保护设置  
//WEL:写使能锁定  
//BUSY:忙标记位(1,忙;0,空闲)  
//默认:0x00  
static uint8_t SpiFlashReadSR(void)
{    
    uint8_t byte=0;     
    SPI_FLASH_CS=0;                            //使能器件     
    Spi2ReadWriteByte(W25X_ReadStatusReg);     //发送读取状态寄存器命令      
    byte=Spi2ReadWriteByte(0Xff);              //读取一个字节    
    SPI_FLASH_CS=1;                            //取消片选       
    return byte;                              
}   
  
//写SPI_FLASH状态寄存器  
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!  
//static void SpiFlashWriteSR(uint8_t sr)
//{     
//    SPI_FLASH_CS=0;                            //使能器件     
//    Spi2ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令      
//    Spi2ReadWriteByte(sr);               //写入一个字节    
//    SPI_FLASH_CS=1;                            //取消片选               
//}   
    
//SPI_FLASH写使能    
//将WEL置位     
static void SpiFlashWriteEnable(void)
{  
    SPI_FLASH_CS=0;                            //使能器件     
    Spi2ReadWriteByte(W25X_WriteEnable);      //发送写使能    
    SPI_FLASH_CS=1;                            //取消片选               
}   
  
//SPI_FLASH写禁止    
//将WEL清零    
static void SpiFlashWriteDisable(void)
{
    SPI_FLASH_CS=0;                            //使能器件     
    Spi2ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令      
    SPI_FLASH_CS=1;                            //取消片选               
}
//等待空闲  
static void SpiFlashWaitBusy(void)     
{     
    while ((SpiFlashReadSR()&0x01)==0x01);   // 等待BUSY位清空  
}

void spiflah_init(void)
{
	SpiFlashWriteDisable();
}

//flash检测 检测到返回0 失败返回1  
ErrorStatus SpiFlashCheck(void)
{
    uint16_t ReadId = 0;
	SPI_FLASH_CS=0;
    Spi2ReadWriteByte(0x90);//发送读取ID命令
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

//读取SPI FLASH    
//在指定地址开始读取指定长度的数据  
//pBuffer:数据存储区  
//ReadAddr:开始读取的地址(24bit)  
//NumByteToRead:要读取的字节数(最大65535)
void SpiFlashRead(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{   
    uint16_t i;
	uint8_t err;
	OSSemPend(W25X40_Device,0,&err);//等待
    SPI_FLASH_CS=0;                            //使能器件     
    Spi2ReadWriteByte(W25X_ReadData);         //发送读取命令     
    Spi2ReadWriteByte((uint8_t)((ReadAddr)>>16));  //发送24bit地址      
    Spi2ReadWriteByte((uint8_t)((ReadAddr)>>8));     
    Spi2ReadWriteByte((uint8_t)ReadAddr);     
    for(i=0;i<NumByteToRead;i++)  
    {   
        pBuffer[i]=Spi2ReadWriteByte(0XFF);   //循环读数    
    }
    SPI_FLASH_CS=1;                            //取消片选
	SpiFlashWaitBusy();
	OSSemPost(W25X40_Device);
}
  
//SPI在一页(0~65535)内写入少于256个字节的数据  
//在指定地址开始写入最大256字节的数据  
//pBuffer:数据存储区  
//WriteAddr:开始写入的地址(24bit)  
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
static void SPIFlashWritePage(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{  
    uint16_t i;
	SpiFlashWriteEnable();
    SPI_FLASH_CS=0;                            //使能器件     
    Spi2ReadWriteByte(W25X_PageProgram);      //发送写页命令     
    Spi2ReadWriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址      
    Spi2ReadWriteByte((uint8_t)((WriteAddr)>>8));     
    Spi2ReadWriteByte((uint8_t)WriteAddr);     
    for(i=0;i<NumByteToWrite;i++)Spi2ReadWriteByte(pBuffer[i]);//循环写数    
    SPI_FLASH_CS=1;                            //取消片选
    SpiFlashWaitBusy();                    //等待写入结束
	SpiFlashWriteDisable();
}   
  
//无检验写SPI FLASH   
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!  
//具有自动换页功能   
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!  
//pBuffer:数据存储区  
//WriteAddr:开始写入的地址(24bit)  
//NumByteToWrite:要写入的字节数(最大65535)  
//CHECK OK  
void SpiFlashWriteNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{                      
    uint16_t pageremain;
	uint8_t err;
	
	OSSemPend(W25X40_Device,0,&err);//等待	
    pageremain=256-WriteAddr%256; //单页剩余的字节数                  
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节  
    while(1)  
    {
        SPIFlashWritePage(pBuffer,WriteAddr,pageremain);  
        if(NumByteToWrite==pageremain)break;//写入结束了  
        else //NumByteToWrite>pageremain  
        {
            pBuffer+=pageremain;  
            WriteAddr+=pageremain;
            NumByteToWrite-=pageremain;           //减去已经写入了的字节数  
            if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节  
            else pageremain=NumByteToWrite;       //不够256个字节了  
        }
		SpiFlashWaitBusy();
    }
	OSSemPost(W25X40_Device);
}
  
//写SPI FLASH    
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!  
//pBuffer:数据存储区  
//WriteAddr:开始写入的地址(24bit)  
//NumByteToWrite:要写入的字节数(最大65535)            
void SpiFlashWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint32_t secpos;  
    uint16_t secoff;  
    uint16_t secremain;       
    uint16_t i;
    secpos=WriteAddr/4096;//扇区地址 0~128 for w25x40
    secoff=WriteAddr%4096;//在扇区内的偏移  
    secremain=4096-secoff;//扇区剩余空间大小     
      
    if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节  
    while(1)   
    {
        SpiFlashRead(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
        for(i=0;i<secremain;i++)//校验数据  
        {
            if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除       
        }
        if(i<secremain)//需要擦除  
        {
            SpiFlashEraseSector(secpos);//擦除这个扇区  
            for(i=0;i<secremain;i++)    //复制
            {  
                SPI_FLASH_BUF[i+secoff]=pBuffer[i];     
            }  
            SpiFlashWriteNoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区
        }else SpiFlashWriteNoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间.                    
        if(NumByteToWrite==secremain)//写入结束了  
		{
			break;
		}
        else//写入未结束  
        {
            secpos++;//扇区地址增1  
            secoff=0;//偏移位置为0      
              
            pBuffer+=secremain;  //指针偏移  
            WriteAddr+=secremain;//写地址偏移         
            NumByteToWrite-=secremain;              //字节数递减  
            if(NumByteToWrite>4096)secremain=4096;   //下一个扇区还是写不完  
            else secremain=NumByteToWrite;          //下一个扇区可以写完了  
        }
    }
}
  
//擦除整个芯片  
//整片擦除时间:  
//W25X16:25s   
//W25X32:40s   
//W25X64:40s   
//等待时间超长...  
void SpiFlashEraseChip(void)
{
	uint8_t err;
	
	OSSemPend(W25X40_Device,0,&err);//等待
	SpiFlashWriteEnable();                  //SET WEL   
    SpiFlashWaitBusy();     
    SPI_FLASH_CS=0;                            //使能器件     
    Spi2ReadWriteByte(W25X_ChipErase);        //发送片擦除命令    
    SPI_FLASH_CS=1;                            //取消片选
    SpiFlashWaitBusy();                    //等待芯片擦除结束
	SpiFlashWriteDisable();
	OSSemPost(W25X40_Device);
}     
  
//擦除一个扇区  
//Dst_Addr:扇区地址 0~127 for w25x40
//擦除一个山区的最少时间:150ms  
void SpiFlashEraseSector(uint32_t Dst_Addr)
{
	uint8_t err;
	
	OSSemPend(W25X40_Device,0,&err);//等待
    Dst_Addr*=4096;  
    SpiFlashWriteEnable();                  //SET WEL      
    SpiFlashWaitBusy();
    SPI_FLASH_CS=0;                            //使能器件     
    Spi2ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令   
    Spi2ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址      
    Spi2ReadWriteByte((uint8_t)((Dst_Addr)>>8));     
    Spi2ReadWriteByte((uint8_t)Dst_Addr);    
    SPI_FLASH_CS=1;                            //取消片选               
    SpiFlashWaitBusy();                    //等待擦除完成
	SpiFlashWriteDisable();
	OSSemPost(W25X40_Device);
}
  
////进入掉电模式  
//void SpiFlashPowerDown(void)
//{   
//    SPI_FLASH_CS=0;                            //使能器件     
//    Spi2ReadWriteByte(W25X_PowerDown);        //发送掉电命令    
//    SPI_FLASH_CS=1;                            //取消片选
//}
//
////唤醒  
//void SpiFlashWakeUp(void)
//{
//    SPI_FLASH_CS=0;                            //使能器件     
//    Spi2ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB      
//    SPI_FLASH_CS=1;                            //取消片选
//}