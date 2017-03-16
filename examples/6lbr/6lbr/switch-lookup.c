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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "SWITCH"

#include "contiki.h"
#include "uip-ds6-nbr.h"
#include "switch-lookup.h"

#include "log-6lbr.h"

void
switch_lookup_learn_addr(const uip_lladdr_t *lladdr, uint8_t ifindex)
{
#if UIP_SWITCH_LOOKUP
  if(lladdr != NULL && !linkaddr_cmp((linkaddr_t *)lladdr, &linkaddr_null)) {
    uip_ds6_nbr_t *nbr;
    nbr = uip_ds6_nbr_ll_lookup(lladdr);
    if(nbr) {
      nbr->ifindex = ifindex;
    } else {
      LOG6LBR_LLADDR(PACKET, lladdr, "No neighbor found for ");
    }
  } else {
    LOG6LBR_DEBUG("Can not learn broadcast or null addr\n");
  }
#endif
}

uint8_t
switch_lookup_get_itf_for(const uip_lladdr_t *lladdr)
{
#if UIP_SWITCH_LOOKUP
  if(lladdr != NULL && !linkaddr_cmp((linkaddr_t *)lladdr, &linkaddr_null)) {
    uip_ds6_nbr_t *nbr;
    nbr = uip_ds6_nbr_ll_lookup(lladdr);
    if(nbr) {
      return nbr->ifindex;
    }
  }
#endif
  return SWITCH_LOOKUP_NO_ITF;
}
