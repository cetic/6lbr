#ifndef NVM_CONFIG_H_
#define NVM_CONFIG_H_

#include "nvm-data.h"

extern nvm_data_t nvm_data;

void load_nvm_config(void);
void store_nvm_config(void);

#endif
