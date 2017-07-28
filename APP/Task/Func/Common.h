#ifndef __COMMON_H
#define __COMMON_H

#include <includes.h>

void delay_us( uint16_t N );
uint8_t	CRC8_Calculate(uint8_t *pdata, uint32_t len);
uint16_t crc_16(uint8_t *q, uint16_t len);

#endif
