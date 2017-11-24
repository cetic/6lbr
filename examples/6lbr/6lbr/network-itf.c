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
#include "network-itf.h"
#include "net/linkaddr.h"
#include <string.h>

#include "log-6lbr.h"

static network_itf_t network_itf_type_table[NETWORK_ITF_NBR];

void
network_itf_init(void)
{
  memset(network_itf_type_table, 0, sizeof(network_itf_type_table));
}

uint8_t
network_itf_register(uint8_t itf_type, const struct mac_driver *mac)
{
  int i = 0;
  while(i < NETWORK_ITF_NBR && network_itf_type_table[i].itf_type != NETWORK_ITF_TYPE_NONE) {
    i++;
  }
  if(i < NETWORK_ITF_NBR) {
    network_itf_type_table[i].itf_type = itf_type;
    network_itf_type_table[i].mac = mac;
    return i;
  } else {
    return -1;
  }
}

network_itf_t *
network_itf_get_itf(uint8_t ifindex)
{
  if(ifindex == NETWORK_ITF_UNKNOWN) {
    return NULL;
  }
  if(ifindex < NETWORK_ITF_NBR) {
    return &network_itf_type_table[ifindex];
  } else {
    return NULL;
  }
}

void
network_itf_set_mac(uint8_t ifindex, uip_lladdr_t *mac_address)
{
  network_itf_t *network_itf = network_itf_get_itf(ifindex);
  if(network_itf != NULL) {
    linkaddr_copy((linkaddr_t *)&network_itf->mac_addr, (linkaddr_t *)mac_address);
  }
}

int
network_itf_known_mac(uip_lladdr_t *mac_address)
{
  int i = 0;
  while(i < NETWORK_ITF_NBR && network_itf_type_table[i].itf_type != NETWORK_ITF_TYPE_NONE) {
    if(linkaddr_cmp((linkaddr_t *)&network_itf_type_table[i].mac_addr, (linkaddr_t *)mac_address)) {
      return 1;
    }
    i++;
  }
  return 0;
}
