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
 *         NVM Interface for the native Linux platform
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "NVM"

#include <contiki.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "log-6lbr.h"
#include "nvm-itf.h"
#include "nvm-config.h"
#include "native-nvm.h"
#include "native-config.h"

#define NVM_SIZE 0x800
static uint8_t nvm_mem[NVM_SIZE];

void
nvm_data_read(void)
{
  int valid = 1;
  LOG6LBR_DEBUG("Opening nvm file '%s'\n", sixlbr_config_nvm_file);
  memset(nvm_mem, 0xff, NVM_SIZE);
  int s = open(sixlbr_config_nvm_file, O_RDONLY);
  if(s > 0) {
    if(read(s, nvm_mem, NVM_SIZE) < 0) {
      LOG6LBR_ERROR("Failed to read NVM");
      valid = 0;
    }
    close(s);
  } else {
    LOG6LBR_ERROR("Could not open nvm file\n");
    valid = 0;
  }
  if(valid) {
    memcpy((uint8_t *) & nvm_data, nvm_mem, sizeof(nvm_data));
  } else {
    nvm_data_reset();
  }
}

void
nvm_data_write(void)
{
  memcpy(nvm_mem, (uint8_t *) & nvm_data, sizeof(nvm_data));
  LOG6LBR_DEBUG("Opening nvm file '%s'\n", sixlbr_config_nvm_file);
  int s = open(sixlbr_config_nvm_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

  if(s > 0) {
    if(write(s, nvm_mem, NVM_SIZE) != NVM_SIZE) {
      LOG6LBR_ERROR("Failed to write to NVM");
    }
    close(s);
  } else {
    LOG6LBR_ERROR("Could not open nvm file\n");
  }
}

int
nvm_data_reset(void)
{
  int force_reset = 0;
  LOG6LBR_DEBUG("Opening nvm factory file '%s'\n", sixlbr_config_factory_nvm_file);
  memset(nvm_mem, 0xff, NVM_SIZE);
  int s = open(sixlbr_config_factory_nvm_file, O_RDONLY);
  if(s > 0) {
    if(read(s, nvm_mem, NVM_SIZE) < 0) {
      LOG6LBR_INFO("Could not read factory NVM");
      force_reset = 1;
    }
    close(s);
  } else {
    LOG6LBR_INFO("Could not open factory nvm file\n");
    force_reset = 1;
  }
  memcpy((uint8_t *) & nvm_data, nvm_mem, sizeof(nvm_data));
  nvm_data_write();
  return force_reset;
}
