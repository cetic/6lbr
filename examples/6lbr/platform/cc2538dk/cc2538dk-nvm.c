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
 *         NVM Interface for the Econotag platform
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "NVM"

#include "contiki.h"
#include "contiki-lib.h"

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "nvm-itf.h"
#include "log-6lbr.h"

//#include <mc1322x.h>
//#include "config.h"

//#define CETIC_6LBR_NVM_ADDRESS (MC1322X_CONFIG_PAGE + 0x100)

void
nvm_data_read(void)
{
/*
  nvmType_t type = 0;
  nvmErr_t err;
*/

  LOG6LBR_INFO("TODO: Reading 6LBR NVM\n");
/*
  LOG6LBR_INFO("Reading 6LBR NVM\n");
  err = nvm_detect(gNvmInternalInterface_c, &type);
  err =
    nvm_read(gNvmInternalInterface_c, type, (uint8_t *) & nvm_data,
             CETIC_6LBR_NVM_ADDRESS, sizeof(nvm_data_t));
  LOG6LBR_ERROR("err : %d\n", err);
*/
}

void
nvm_data_write(void)
{
/*
  nvmType_t type = 0;
  nvmErr_t err;
*/

  LOG6LBR_INFO("TODO: Flashing 6LBR NVM\n");
/*
  LOG6LBR_INFO("Flashing 6LBR NVM\n");
  mc1322x_config_save(&mc1322x_config);
  err = nvm_detect(gNvmInternalInterface_c, &type);
  err =
    nvm_write(gNvmInternalInterface_c, type, (uint8_t *) & nvm_data,
              CETIC_6LBR_NVM_ADDRESS, sizeof(nvm_data_t));
  LOG6LBR_ERROR("err : %d\n", err);
*/
}
