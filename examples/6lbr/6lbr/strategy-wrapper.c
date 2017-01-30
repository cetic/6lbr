/*
 * Copyright (c) 2017, CETIC.
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

#include "net/llsec/adaptivesec/adaptivesec.h"
#include "net/llsec/adaptivesec/noncoresec-strategy.h"

#include "nvm-config.h"

#include "log-6lbr.h"

static const struct adaptivesec_strategy *current_strategy = &noncoresec_strategy;

void
llsec_strategy_wrapper_init(void)
{
  if(nvm_data.security_layer == CETIC_6LBR_SECURITY_LAYER_ADAPTIVE_NONCORESEC) {
    LOG6LBR_INFO("Using 'adaptivesec (noncore)' llsec strategy\n");
    current_strategy = &noncoresec_strategy;
  } else if(nvm_data.security_layer == CETIC_6LBR_SECURITY_LAYER_ADAPTIVE_CORESEC) {
    LOG6LBR_ERROR("'adaptivesec (core)' llsec strategy not yet supported\n");
    current_strategy = &noncoresec_strategy;
  }
}
/*---------------------------------------------------------------------------*/
static void
send(mac_callback_t sent, void *ptr)
{
  current_strategy->send(sent, ptr);
}
/*---------------------------------------------------------------------------*/
static int
on_frame_created(void)
{
  return current_strategy->on_frame_created();
}
/*---------------------------------------------------------------------------*/
static enum adaptivesec_verify
verify(struct akes_nbr *sender)
{
  return current_strategy->verify(sender);
}
/*---------------------------------------------------------------------------*/
static uint8_t
get_overhead(void)
{
  return current_strategy->get_overhead();
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
  current_strategy->init();
}
/*---------------------------------------------------------------------------*/
const struct adaptivesec_strategy strategy_wrapper = {
  send,
  on_frame_created,
  verify,
  get_overhead,
  init
};
/*---------------------------------------------------------------------------*/
