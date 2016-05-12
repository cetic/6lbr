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

#include "nvm-config.h"
#include "nvm-itf.h"

//Temporarily
//#include "log-6lbr.h"
#include "stdio.h"
#define LOG6LBR_INFO printf
#define LOG6LBR_ERROR printf
#define LOG6LBR_FATAL printf

#include "rom-util.h"

#define CETIC_6LBR_NVM_SIZE 2048
// We use the penultimate flash page as our nvm
//TODO: this must be updated for non 512K CC2538
#define CETIC_6LBR_NVM_ADDRESS (0x00280000 - (2*CETIC_6LBR_NVM_SIZE))

void
nvm_data_read(void)
{
  LOG6LBR_INFO("Reading 6LBR NVM\n");
  rom_util_memcpy( (void *)&nvm_data,
   (void *)CETIC_6LBR_NVM_ADDRESS, sizeof(nvm_data_t));
}

void
nvm_data_write(void)
{
  long err;
  int retry = 4;
  while (retry > 0 ) {
    LOG6LBR_INFO("Flashing 6LBR NVM\n");
    err = rom_util_page_erase(CETIC_6LBR_NVM_ADDRESS, CETIC_6LBR_NVM_SIZE);
    if ( err != 0 ) {
      LOG6LBR_ERROR("erase error : %ld\n", err);
    }
    rom_util_program_flash( (uint32_t*)&nvm_data,
     CETIC_6LBR_NVM_ADDRESS, (sizeof(nvm_data_t)/4+1)*4);
    if ( err != 0 ) {
      LOG6LBR_ERROR("write error : %ld\n", err);
    }
    if(rom_util_memcmp( (void *)&nvm_data, (void *)CETIC_6LBR_NVM_ADDRESS, sizeof(nvm_data_t)) == 0) {
      break;
    }
    LOG6LBR_ERROR("verify NVM failed, retry\n");
    retry--;
  }
  if(retry == 0) {
    LOG6LBR_FATAL("Could not program 6LBR NVM !\n");
  } else {
    LOG6LBR_INFO("Flashing 6LBR NVM done\n");
  }
}
