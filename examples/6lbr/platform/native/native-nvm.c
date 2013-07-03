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

#include <contiki.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "nvm-itf.h"
#include "nvm-config.h"
#include "native-nvm.h"

#define NVM_SIZE 0x100
static uint8_t nvm_mem[NVM_SIZE];
char const *nvm_file = NULL;

void
nvm_data_read(void)
{
  printf("Opening nvm file '%s'\n", nvm_file);
  int s = open(nvm_file, O_RDONLY);

  if(s > 0) {
    read(s, nvm_mem, NVM_SIZE);
    close(s);
  } else {
    printf("Warning: could not open nvm file\n");
    memset(nvm_mem, 0xff, NVM_SIZE);
  }
  memcpy((uint8_t *) & nvm_data, nvm_mem, sizeof(nvm_data));
}

void
nvm_data_write(void)
{
  memcpy(nvm_mem, (uint8_t *) & nvm_data, sizeof(nvm_data));
  printf("Opening nvm file '%s'\n", nvm_file);
  int s = open(nvm_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

  if(s > 0) {
    write(s, nvm_mem, NVM_SIZE);
    close(s);
  } else {
    printf("Error: could not open nvm file\n");
  }
}
