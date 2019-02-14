/*
 * Copyright (c) 2018, CETIC.
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
 *         Simple MAC address translation
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"
#include "contiki-net.h"
#include "6lbr-main.h"
#include "6lbr-network.h"

#include "mactrans-simple.h"

static uint8_t
eth_to_lowpan(uip_lladdr_t * lowpan, const uip_eth_addr * ethernet)
{
  if(memcmp((uint8_t *) &eth_mac_addr, ethernet, 6) == 0) {
    memcpy((uint8_t *) lowpan, (uint8_t *) &uip_lladdr, UIP_LLADDR_LEN);
    return 1;
  }
  lowpan->addr[0] = ethernet->addr[0];
  lowpan->addr[1] = ethernet->addr[1];
  lowpan->addr[2] = ethernet->addr[2];
  lowpan->addr[3] = CETIC_6LBR_ETH_EXT_A;
  lowpan->addr[4] = CETIC_6LBR_ETH_EXT_B;
  lowpan->addr[5] = ethernet->addr[3];
  lowpan->addr[6] = ethernet->addr[4];
  lowpan->addr[7] = ethernet->addr[5];

  return 1;
}

static uint8_t
lowpan_to_eth(uip_eth_addr * ethernet, const uip_lladdr_t * lowpan)
{
  if(eth_mac_addr_ready && memcmp((uint8_t *) &uip_lladdr, (uint8_t *) lowpan, UIP_LLADDR_LEN) == 0) {
    memcpy(ethernet, &eth_mac_addr, 6);
    return 1;
  }
  ethernet->addr[0] = lowpan->addr[0];
  ethernet->addr[1] = lowpan->addr[1];
  ethernet->addr[2] = lowpan->addr[2];
  ethernet->addr[3] = lowpan->addr[5];
  ethernet->addr[4] = lowpan->addr[6];
  ethernet->addr[5] = lowpan->addr[7];

  return 1;
}

const struct mactrans_driver mactrans_simple = {
  eth_to_lowpan,
  lowpan_to_eth
};
