#ifndef NVM_DATA_H_
#define NVM_DATA_H_

#include <stdint.h>

/*---------------------------------------------------------------------------*/

#define MAX_DEVICE_NAME_LENGTH 40

/*---------------------------------------------------------------------------*/

typedef struct {
  //NVM header
  uint16_t magic;
  uint16_t version;

  //Version 0 Configuration data
  char device_name[MAX_DEVICE_NAME_LENGTH+1];
} nvm_data_t;

/*---------------------------------------------------------------------------*/

//Header definition
#define CETIC_6LBR_NVM_MAGIC		0x2009

#define CETIC_6LBR_NVM_VERSION_0	0

#define CETIC_6LBR_NVM_CURRENT_VERSION	CETIC_6LBR_NVM_VERSION_0

/*---------------------------------------------------------------------------*/

//Default values

/*---------------------------------------------------------------------------*/

#endif
