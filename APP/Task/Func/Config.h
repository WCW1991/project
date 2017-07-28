#ifndef __CONFIG_H
#define __CONFIG_H

#include "includes.h"

void config_init(void);
void config_store(void);
void record_state_init(void);
void record_state_store(void);
void record_state_store_ds1302(void);

#endif
