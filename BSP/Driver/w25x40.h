#ifndef __W25X40_H
#define __W25X40_H
#include "stm32f1xx_hal.h"
#include "bsp_periph.h"
#include "MyLib.h"

#define SPI_FLASH_SECTOR_SIZE	4096

void spiflah_init(void);
ErrorStatus SpiFlashCheck(void);
void SpiFlashRead(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
void SpiFlashWriteNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SpiFlashWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SpiFlashEraseChip(void);
void SpiFlashEraseSector(uint32_t Dst_Addr);

#endif