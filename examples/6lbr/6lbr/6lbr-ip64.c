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
 *         NAT64 Configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "6LBR"

#include "contiki.h"
#include "contiki-net.h"

#include "log-6lbr.h"
#include "6lbr-network.h"
#include "nvm-config.h"

#include "ip64.h"
#include "ip64-ipv4-dhcp.h"
#include "ip64-dhcpc.h"
#include "ip64-eth.h"
#include "ip64-addr.h"

#if CONTIKI_TARGET_NATIVE
extern void cetic_6lbr_save_ip(void);
#endif

uip_ip4addr_t eth_ip64_addr;
uip_ip4addr_t eth_ip64_netmask;
uip_ip4addr_t eth_ip64_gateway;

const struct ip64_dhcpc_state *cetic_6lbr_ip64_dhcp_state;

/*---------------------------------------------------------------------------*/
void
cetic_6lbr_ip64_dhcpc_configured(const struct ip64_dhcpc_state *s)
{
  cetic_6lbr_ip64_dhcp_state = s;
  LOG6LBR_4ADDR(INFO, &s->ipaddr, "Set IPv4 address : ");
#if CONTIKI_TARGET_NATIVE
  cetic_6lbr_save_ip();
#endif
}
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_ip64_init(void)
{
  if((nvm_data.global_flags & CETIC_GLOBAL_IP64) != 0) {
    LOG6LBR_INFO("Starting IP64\n");
    ip64_eth_addr_set((struct ip64_eth_addr *)&eth_mac_addr);
    if((nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_RFC6052_PREFIX) != 0) {
      uip_ip6addr_t ip64_prefix = {{ 0, 0x64, 0xff, 0x9b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
      ip64_addr_set_prefix(&ip64_prefix, 96);
    }
    ip64_init();
    if((nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_DHCP) == 0) {
      memcpy(&eth_ip64_addr, nvm_data.eth_ip64_addr, sizeof(nvm_data.eth_ip64_addr));
      memcpy(&eth_ip64_netmask, nvm_data.eth_ip64_netmask, sizeof(nvm_data.eth_ip64_netmask));
      memcpy(&eth_ip64_gateway, nvm_data.eth_ip64_gateway, sizeof(nvm_data.eth_ip64_gateway));
      ip64_set_ipv4_address(&eth_ip64_addr, &eth_ip64_netmask);
      ip64_set_draddr(&eth_ip64_gateway);
      LOG6LBR_4ADDR(INFO, &eth_ip64_addr, "IPv4 address : ");
    } else {
      ip64_ipv4_dhcp_init();
    }
  }
}
/*---------------------------------------------------------------------------*/
