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
 *         6LBR Network configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "6LBR"

#include "contiki.h"
#include "contiki-net.h"

#include "log-6lbr.h"

#include "6lbr-network.h"
#include "nvm-config.h"
#include "rio.h"

#if CONTIKI_TARGET_NATIVE
extern void cetic_6lbr_save_ip(void);
#endif

//WSN
uip_lladdr_t wsn_mac_addr;
uip_ip6addr_t wsn_net_prefix;
uint8_t wsn_net_prefix_len;
uip_ipaddr_t wsn_ip_addr;
uip_ipaddr_t wsn_ip_local_addr;

// Eth
uip_eth_addr eth_mac_addr;
uip_lladdr_t eth_mac64_addr;
uip_ipaddr_t eth_ip_addr;
uip_ipaddr_t eth_net_prefix;
uip_ipaddr_t eth_ip_local_addr;
uip_ipaddr_t eth_dft_router;
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_network_init(void)
{
  uip_ds6_addr_t *local = uip_ds6_get_link_local(-1);

  uip_ipaddr_copy(&wsn_ip_local_addr, &local->ipaddr);

  LOG6LBR_6ADDR(INFO, &wsn_ip_local_addr, "Tentative local IPv6 address ");

  eth_mac64_addr.addr[0] = eth_mac_addr.addr[0];
  eth_mac64_addr.addr[1] = eth_mac_addr.addr[1];
  eth_mac64_addr.addr[2] = eth_mac_addr.addr[2];
  eth_mac64_addr.addr[3] = CETIC_6LBR_ETH_EXT_A;
  eth_mac64_addr.addr[4] = CETIC_6LBR_ETH_EXT_B;
  eth_mac64_addr.addr[5] = eth_mac_addr.addr[3];
  eth_mac64_addr.addr[6] = eth_mac_addr.addr[4];
  eth_mac64_addr.addr[7] = eth_mac_addr.addr[5];

#if CETIC_6LBR_SMARTBRIDGE
  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    memcpy(wsn_net_prefix.u8, &nvm_data.wsn_net_prefix,
           sizeof(nvm_data.wsn_net_prefix));
    wsn_net_prefix_len = nvm_data.wsn_net_prefix_len;
    if((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0) {
      uip_ipaddr_copy(&wsn_ip_addr, &wsn_net_prefix);
      uip_ds6_set_addr_iid(&wsn_ip_addr, &uip_lladdr);
      uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_AUTOCONF);
    } else {
      memcpy(wsn_ip_addr.u8, &nvm_data.wsn_ip_addr,
             sizeof(nvm_data.wsn_ip_addr));
      uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_MANUAL);
    }
    LOG6LBR_6ADDR(INFO, &wsn_ip_addr, "Tentative global IPv6 address ");
    memcpy(eth_dft_router.u8, &nvm_data.eth_dft_router,
           sizeof(nvm_data.eth_dft_router));
    if ( !uip_is_addr_unspecified(&eth_dft_router) ) {
      uip_ds6_defrt_add(&eth_dft_router, 0);
    }
    uip_ipaddr_t dns;
    memcpy(dns.u8, &nvm_data.dns_server,
           sizeof(nvm_data.dns_server));
    if(!uip_is_addr_unspecified(&dns)) {
      LOG6LBR_6ADDR(INFO, &dns, "DNS Server ");
      uip_nameserver_update(&dns, UIP_NAMESERVER_INFINITE_LIFETIME);
    }
  } else {
    uip_create_unspecified(&wsn_net_prefix);
    wsn_net_prefix_len = 0;
    uip_create_unspecified(&wsn_ip_addr);
  }
#endif

#if CETIC_6LBR_ROUTER
  //WSN network configuration
  memcpy(wsn_net_prefix.u8, &nvm_data.wsn_net_prefix,
         sizeof(nvm_data.wsn_net_prefix));
  wsn_net_prefix_len = nvm_data.wsn_net_prefix_len;
  if((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0) {
    uip_ipaddr_copy(&wsn_ip_addr, &wsn_net_prefix);
    uip_ds6_set_addr_iid(&wsn_ip_addr, &uip_lladdr);
    uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_AUTOCONF);
  } else {
    memcpy(wsn_ip_addr.u8, &nvm_data.wsn_ip_addr,
           sizeof(nvm_data.wsn_ip_addr));
    uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_MANUAL);
  }
  LOG6LBR_6ADDR(INFO, &wsn_ip_addr, "Tentative global IPv6 address (WSN) ");
  uip_ipaddr_t dns;
  memcpy(dns.u8, &nvm_data.dns_server,
         sizeof(nvm_data.dns_server));
  if(!uip_is_addr_unspecified(&dns)) {
    LOG6LBR_6ADDR(INFO, &dns, "DNS Server ");
    uip_nameserver_update(&dns, UIP_NAMESERVER_INFINITE_LIFETIME);
  }

  //Ethernet network configuration
  memcpy(eth_net_prefix.u8, &nvm_data.eth_net_prefix,
         sizeof(nvm_data.eth_net_prefix));
  memcpy(eth_dft_router.u8, &nvm_data.eth_dft_router,
         sizeof(nvm_data.eth_dft_router));

  if ( !uip_is_addr_unspecified(&eth_dft_router) ) {
    uip_ds6_defrt_add(&eth_dft_router, 0);
  }

  if((nvm_data.mode & CETIC_MODE_ETH_AUTOCONF) != 0) {
    uip_ipaddr_copy(&eth_ip_addr, &eth_net_prefix);
    uip_ds6_set_addr_iid(&eth_ip_addr, &eth_mac64_addr);
    uip_ds6_addr_add(&eth_ip_addr, 0, ADDR_AUTOCONF);
  } else {
    memcpy(eth_ip_addr.u8, &nvm_data.eth_ip_addr,
           sizeof(nvm_data.eth_ip_addr));
    uip_ds6_addr_add(&eth_ip_addr, 0, ADDR_MANUAL);
  }
  LOG6LBR_6ADDR(INFO, &eth_ip_addr, "Tentative global IPv6 address (ETH) ");

  //Ugly hack : in order to set WSN local address as the default address
  //We must add it afterwards as uip_ds6_addr_add allocates addr from the end of the list
  uip_ds6_addr_rm(local);

  uip_create_linklocal_prefix(&eth_ip_local_addr);
  uip_ds6_set_addr_iid(&eth_ip_local_addr, &eth_mac64_addr);
  uip_ds6_addr_add(&eth_ip_local_addr, 0, ADDR_AUTOCONF);

  uip_ds6_addr_add(&wsn_ip_local_addr, 0, ADDR_AUTOCONF);

  //Prefix and RA configuration
#if CETIC_6LBR_WITH_RPL
  uint8_t publish = (nvm_data.ra_prefix_flags & CETIC_6LBR_MODE_SEND_PIO) != 0;
  uip_ds6_prefix_add(&eth_net_prefix, nvm_data.eth_net_prefix_len, publish,
                     nvm_data.ra_prefix_flags,
                     nvm_data.ra_prefix_vtime, nvm_data.ra_prefix_ptime);
#else
  uip_ds6_prefix_add(&eth_net_prefix, nvm_data.eth_net_prefix_len, 0, 0, 0, 0);
  uint8_t publish = (nvm_data.ra_prefix_flags & CETIC_6LBR_MODE_SEND_PIO) != 0;
  uip_ds6_prefix_add(&wsn_net_prefix, nvm_data.wsn_net_prefix_len, publish,
                     nvm_data.ra_prefix_flags,
                     nvm_data.ra_prefix_vtime, nvm_data.ra_prefix_ptime);
#endif

#if CETIC_6LBR_WITH_RPL
  if ((nvm_data.ra_rio_flags & CETIC_6LBR_MODE_SEND_RIO) != 0 ) {
    uip_ds6_route_info_add(&wsn_net_prefix, nvm_data.wsn_net_prefix_len, nvm_data.ra_rio_flags, nvm_data.ra_rio_lifetime);
  }
#endif
  if ((nvm_data.mode & CETIC_MODE_ROUTER_RA_DAEMON) != 0 ) {
    LOG6LBR_INFO("RA Daemon enabled\n");
  } else {
    LOG6LBR_INFO("RA Daemon disabled\n");
  }
#endif
}

void
cetic_6lbr_mdns_init(void)
{
#if RESOLV_CONF_SUPPORTS_MDNS
  if((nvm_data.global_flags & CETIC_GLOBAL_MDNS) != 0) {
    LOG6LBR_INFO("Starting MDNS\n");
    process_start(&resolv_process, NULL);
    resolv_set_hostname((char *)nvm_data.mdns_host_name);
#if RESOLV_CONF_SUPPORTS_DNS_SD
    if((nvm_data.dns_flags & CETIC_6LBR_DNS_DNS_SD) != 0) {
      resolv_add_service("_6lbr._tcp", "", nvm_data.webserver_port);
    }
#endif
  }
#endif
}
