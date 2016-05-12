/*
 * Copyright (c) 2016, CETIC.
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
#include "board-peripherals.h"

//Temporarily
//#include "log-6lbr.h"
#include "stdio.h"
#define LOG6LBR_INFO printf
#define LOG6LBR_ERROR printf
#define LOG6LBR_FATAL printf

#define CETIC_6LBR_NVM_SIZE 2048
#define CETIC_6LBR_NVM_ADDRESS 0

void
nvm_data_read(void)
{
  LOG6LBR_INFO("Reading 6LBR NVM\n");
  int rv = ext_flash_open();

  if(!rv) {
    LOG6LBR_ERROR("Could not open external flash\n");
    ext_flash_close();
    return;
  }

  rv = ext_flash_read(CETIC_6LBR_NVM_ADDRESS, sizeof(nvm_data_t),
                      (uint8_t *)&nvm_data);

  ext_flash_close();

  if(!rv) {
    printf("Error loading config\n");
    return;
  }
}

void
nvm_data_write(void)
{
  LOG6LBR_INFO("Writing 6LBR NVM\n");
  int rv;
  rv = ext_flash_open();

  if(!rv) {
    LOG6LBR_ERROR("Could not open external flash\n");
    ext_flash_close();
    return;
  }

  rv = ext_flash_erase(CETIC_6LBR_NVM_ADDRESS, CETIC_6LBR_NVM_SIZE);

  if(!rv) {
    LOG6LBR_ERROR("Error erasing flash\n");
  } else {
    rv = ext_flash_write(CETIC_6LBR_NVM_ADDRESS, sizeof(nvm_data_t),
                         (uint8_t *)&nvm_data);
    if(!rv) {
      LOG6LBR_ERROR("Error writing flash\n");
    }
  }

  ext_flash_close();
}
