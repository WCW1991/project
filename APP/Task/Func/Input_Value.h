#ifndef __INPUT_VALUE_H
#define __INPUT_VALUE_H

#include "includes.h"

Key_enum input_value8(uint8_t *value, uint8_t length, uint8_t StartPage, uint8_t StartAddr);
Key_enum input_value16(uint16_t *value, uint8_t length, uint8_t StartPage, uint8_t StartAddr);

#endif
