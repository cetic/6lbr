#ifndef NVM_DATA_H_
#define NVM_DATA_H_

#include <stdint.h>

#include "device-resource.h"
#include "binding-table-resource.h"

/*---------------------------------------------------------------------------*/

typedef struct {
  //NVM header
  uint16_t magic;
  uint16_t version;
  uint16_t size;

  //Version 0 Configuration data
  REST_RES_DEVICE_NVM;
  CORE_INTERFACE_BINDING_TABLE_NVM;
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
