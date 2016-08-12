/*
 * Copyright (c) 2014, CETIC.
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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "LLSEC"

#include "contiki.h"
#include "llsec-wrapper.h"
#include "nullsec.h"
#include "noncoresec/noncoresec.h"

#include "nvm-config.h"

#include "log-6lbr.h"

static const struct llsec_driver *current_llsec_driver = &nullsec_driver;

void
llsec_wrapper_init(void)
{
  if(nvm_data.security_layer == CETIC_6LBR_SECURITY_LAYER_NONE) {
    LOG6LBR_INFO("Using 'nullsec' llsec driver\n");
    current_llsec_driver = &nullsec_driver;
  } else if(nvm_data.security_layer == CETIC_6LBR_SECURITY_LAYER_NONCORESEC) {
    LOG6LBR_INFO("Using 'noncoresec' llsec driver\n");
    current_llsec_driver = &noncoresec_driver;
  } else {
    LOG6LBR_ERROR("Unknown llsec driver, using 'nullsec' instead\n");
    current_llsec_driver = &nullsec_driver;
  }
  current_llsec_driver->init();
}
/*---------------------------------------------------------------------------*/
char const * llsec_wrapper_name(void)
{
  return current_llsec_driver->name;
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
  //init is deferred until 6lbr config is read
}
/*---------------------------------------------------------------------------*/
static void
send(mac_callback_t sent, void *ptr)
{
  current_llsec_driver->send(sent, ptr);
}
/*---------------------------------------------------------------------------*/
static void
input(void)
{
  current_llsec_driver->input();
}
/*---------------------------------------------------------------------------*/
const struct llsec_driver llsec_wrapper_driver = {
  "llsec-wrapper",
  init,
  send,
  input,
};
/*---------------------------------------------------------------------------*/
