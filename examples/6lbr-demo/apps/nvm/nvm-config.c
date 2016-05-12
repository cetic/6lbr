/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         Platform-independant interface for parameters in Non-Volatile Memory
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "NVM"

#include "contiki.h"
#include "contiki-lib.h"

#include <string.h>
#include <ctype.h>

#include "nvm-config.h"
#include "nvm-itf.h"

//Temporarily
//#include "log-6lbr.h"
#include "stdio.h"
#define LOG6LBR_INFO printf
#define LOG6LBR_ERROR printf
#define LOG6LBR_FATAL printf

nvm_data_t nvm_data;

#ifdef USER_CONF_NVM_DATA_RESET
extern void USER_CONF_NVM_DATA_RESET(nvm_data_t * nvm_data);
#endif

/*---------------------------------------------------------------------------*/

static void
nvm_reset_version_0(nvm_data_t * nvm_data)
{
  nvm_data->magic = CETIC_6LBR_NVM_MAGIC;
  nvm_data->version = CETIC_6LBR_NVM_VERSION_0;
  nvm_data->size = sizeof(nvm_data_t);

  //REST_RES_DEVICE_NVM_INIT(nvm_data);
  //CORE_INTERFACE_BINDING_TABLE_NVM_INIT(nvm_data);
#ifdef USER_CONF_NVM_DATA_RESET
  USER_CONF_NVM_DATA_RESET(nvm_data);
#endif
}

void
check_nvm(nvm_data_t * nvm_data, int reset)
{
  uint8_t flash = 0;

  if(reset || nvm_data->magic != CETIC_6LBR_NVM_MAGIC
     || nvm_data->version > CETIC_6LBR_NVM_CURRENT_VERSION) {
    //NVM is invalid or we are rollbacking from another version
    //Set all data to default values
    if (!reset) {
      LOG6LBR_ERROR
        ("Invalid NVM magic number or unsupported NVM version, reseting it...\n");
    }
    nvm_reset_version_0(nvm_data);
    flash = 1;
  }
  if (nvm_data->size != sizeof(nvm_data_t)) {
    LOG6LBR_ERROR
      ("Invalid NVM size, reseting it...\n");
    nvm_reset_version_0(nvm_data);
    flash = 1;
  }

  if(flash) {
    nvm_data_write();
  }
}

void
load_nvm_config(void)
{
  nvm_data_read();

  LOG6LBR_INFO("NVM Magic : %x\n", nvm_data.magic);
  LOG6LBR_INFO("NVM Version : %x\n", nvm_data.version);
  LOG6LBR_INFO("NVM Size : %d\n", nvm_data.size);

  check_nvm(&nvm_data, 0);
}

void
store_nvm_config(void)
{
  nvm_data_write();
}
