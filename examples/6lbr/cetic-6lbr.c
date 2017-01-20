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
 *         Main 6LBR process and initialisation
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "6LBR"

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/ipv6/sicslowpan.h"
#include "net/ipv6/uip-nd6.h"
#include "net/rpl/rpl.h"
#include "net/netstack.h"
#include "net/rpl/rpl.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "log-6lbr.h"

#include "cetic-6lbr.h"
#include "platform-init.h"
#include "packet-filter.h"
#include "eth-drv.h"
#include "nvm-config.h"
#include "rio.h"
#include "6lbr-hooks.h"

#if CETIC_6LBR_IP64
#include "ip64.h"
#include "ip64-ipv4-dhcp.h"
#include "ip64-eth.h"
#include "ip64-addr.h"
#endif

#if CETIC_6LBR_MAC_WRAPPER
#include "mac-wrapper.h"
#endif

#if CETIC_6LBR_LLSEC_WRAPPER
#include "llsec-wrapper.h"
#include "framer-wrapper.h"
#endif

#if WEBSERVER
#include "webserver.h"
#endif

#if UDPSERVER
#include "udp-server.h"
#endif

#if CETIC_NODE_INFO
#include "node-info.h"
#endif

#if CETIC_NODE_CONFIG
#include "node-config.h"
#endif

#if WITH_COAPSERVER
#include "coap-server.h"
#endif

#if WITH_TINYDTLS
#include "dtls.h"
#endif

#if WITH_DTLS_ECHO
#include "dtls-echo.h"
#endif

#if WITH_NVM_PROXY
#include "nvm-proxy.h"
#endif

#if WITH_DNS_PROXY
#include "dns-proxy.h"
#endif

#if CONTIKI_TARGET_NATIVE
extern void cetic_6lbr_save_ip(void);
#endif

//Initialisation flags
int ethernet_ready = 0;
int eth_mac_addr_ready = 0;
int radio_ready = 0;
int radio_mac_addr_ready = 0;

//WSN
uip_lladdr_t wsn_mac_addr;
uip_ip6addr_t wsn_net_prefix;
uint8_t wsn_net_prefix_len;
uip_ipaddr_t wsn_ip_addr;
uip_ipaddr_t wsn_ip_local_addr;
rpl_dag_t *cetic_dag;

// Eth
ethaddr_t eth_mac_addr;
uip_lladdr_t eth_mac64_addr;
uip_ipaddr_t eth_ip_addr;
uip_ipaddr_t eth_net_prefix;
uip_ipaddr_t eth_ip_local_addr;
uip_ipaddr_t eth_dft_router;
uip_ip4addr_t eth_ip64_addr;
uip_ip4addr_t eth_ip64_netmask;
uip_ip4addr_t eth_ip64_gateway;

//Misc
unsigned long cetic_6lbr_startup;

enum cetic_6lbr_restart_type_t cetic_6lbr_restart_type;

//Hooks
cetic_6lbr_allowed_node_hook_t cetic_6lbr_allowed_node_hook = cetic_6lbr_allowed_node_default_hook;

/*---------------------------------------------------------------------------*/
PROCESS_NAME(udp_client_process);

process_event_t cetic_6lbr_restart_event;
process_event_t cetic_6lbr_reload_event;

PROCESS(cetic_6lbr_process, "CETIC Bridge process");

AUTOSTART_PROCESSES(&cetic_6lbr_process);

/*---------------------------------------------------------------------------*/
void
cetic_6lbr_set_prefix(uip_ipaddr_t * prefix, unsigned len,
                      uip_ipaddr_t * ipaddr)
{
#if CETIC_6LBR_SMARTBRIDGE
  int new_prefix = !uip_ipaddr_prefixcmp(&wsn_net_prefix, prefix, len);
  int new_dag_prefix = cetic_dag == NULL || !uip_ipaddr_prefixcmp(&cetic_dag->prefix_info.prefix, prefix, len);
  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    LOG6LBR_DEBUG("Ignoring RA\n");
    return;
  }

  if(new_prefix) {
    LOG6LBR_6ADDR(INFO, prefix, "Setting prefix : ");
    uip_ipaddr_copy(&wsn_ip_addr, ipaddr);
    uip_ipaddr_copy(&wsn_net_prefix, prefix);
    wsn_net_prefix_len = len;
    LOG6LBR_6ADDR(INFO, &wsn_ip_addr, "Tentative global IPv6 address : ");
#if CONTIKI_TARGET_NATIVE
  cetic_6lbr_save_ip();
#endif
  }
  if(new_dag_prefix) {
    if((nvm_data.rpl_config & CETIC_6LBR_MODE_GLOBAL_DODAG) != 0) {
      cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &wsn_ip_addr);
      rpl_set_prefix(cetic_dag, prefix, len);
      LOG6LBR_6ADDR(INFO, &cetic_dag->dag_id, "Configured as DODAG Root ");
    } else {
      rpl_set_prefix(cetic_dag, prefix, len);
      LOG6LBR_6ADDR(INFO, prefix, "Setting DAG prefix : ");
      rpl_repair_root(RPL_DEFAULT_INSTANCE);
    }
  }
#endif
}
/*---------------------------------------------------------------------------*/
void cetic_6lbr_ip64_dhcpc_configured(const struct ip64_dhcpc_state *s)
{
  LOG6LBR_4ADDR(INFO, &s->ipaddr, "Set IPv4 address : ");
}
/*---------------------------------------------------------------------------*/
int cetic_6lbr_allowed_node_default_hook(rpl_dag_t *dag, uip_ipaddr_t *prefix, int prefix_len)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_init(void)
{
  uip_ds6_addr_t *local = uip_ds6_get_link_local(-1);

  uip_ipaddr_copy(&wsn_ip_local_addr, &local->ipaddr);

  LOG6LBR_6ADDR(INFO, &wsn_ip_local_addr, "Tentative local IPv6 address ");

#if CETIC_6LBR_SMARTBRIDGE

  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0)    //Manual configuration
  {
    memcpy(wsn_net_prefix.u8, &nvm_data.wsn_net_prefix,
           sizeof(nvm_data.wsn_net_prefix));
    wsn_net_prefix_len = nvm_data.wsn_net_prefix_len;
    if((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0)  //Address auto configuration
    {
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
    uip_nameserver_update(&dns, UIP_NAMESERVER_INFINITE_LIFETIME);
  } else {                            //End manual configuration
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
  if((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0)    //Address auto configuration
  {
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
  uip_nameserver_update(&dns, UIP_NAMESERVER_INFINITE_LIFETIME);

  //Ethernet network configuration
  memcpy(eth_net_prefix.u8, &nvm_data.eth_net_prefix,
         sizeof(nvm_data.eth_net_prefix));
  memcpy(eth_dft_router.u8, &nvm_data.eth_dft_router,
         sizeof(nvm_data.eth_dft_router));

  if ( !uip_is_addr_unspecified(&eth_dft_router) ) {
    uip_ds6_defrt_add(&eth_dft_router, 0);
  }

  eth_mac64_addr.addr[0] = eth_mac_addr[0];
  eth_mac64_addr.addr[1] = eth_mac_addr[1];
  eth_mac64_addr.addr[2] = eth_mac_addr[2];
  eth_mac64_addr.addr[3] = CETIC_6LBR_ETH_EXT_A;
  eth_mac64_addr.addr[4] = CETIC_6LBR_ETH_EXT_B;
  eth_mac64_addr.addr[5] = eth_mac_addr[3];
  eth_mac64_addr.addr[6] = eth_mac_addr[4];
  eth_mac64_addr.addr[7] = eth_mac_addr[5];

  if((nvm_data.mode & CETIC_MODE_ETH_AUTOCONF) != 0)    //Address auto configuration
  {
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
#if UIP_CONF_IPV6_RPL
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

#if UIP_CONF_IPV6_RPL
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
cetic_6lbr_init_finalize(void)
{
#if UIP_CONF_IPV6_RPL && CETIC_6LBR_DODAG_ROOT
  if((nvm_data.rpl_config & CETIC_6LBR_MODE_MANUAL_DODAG) != 0) {
    //Manual DODAG ID
    cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, (uip_ipaddr_t*)&nvm_data.rpl_dodag_id);
  } else {
    //Automatic DODAG ID
    if((nvm_data.rpl_config & CETIC_6LBR_MODE_GLOBAL_DODAG) != 0) {
#if CETIC_6LBR_SMARTBRIDGE
      if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
#endif
      //DODAGID = global address used !
      cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &wsn_ip_addr);
#if CETIC_6LBR_SMARTBRIDGE
      } else {
        //Not global IP yet configured
        cetic_dag = NULL;
      }
#endif
    } else {
      //DODAGID = link-local address used !
      cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &wsn_ip_local_addr);
    }
  }
#if CETIC_6LBR_SMARTBRIDGE
  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
  }
#else
  rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
#endif
  if(cetic_dag) {
    LOG6LBR_6ADDR(INFO, &cetic_dag->dag_id, "Configured as DODAG Root ");
  }
#endif
  if(!uip_is_addr_unspecified(&wsn_ip_addr)) {
    uip_ds6_addr_add(&wsn_ip_addr, 0, ((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0) ? ADDR_AUTOCONF : ADDR_MANUAL);
  }

#if CETIC_6LBR_IP64
  if((nvm_data.global_flags & CETIC_GLOBAL_IP64) != 0) {
    LOG6LBR_INFO("Starting IP64\n");
    ip64_eth_addr_set((struct ip64_eth_addr *)eth_mac_addr);
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
#endif

#if RESOLV_CONF_SUPPORTS_MDNS
  if((nvm_data.global_flags & CETIC_GLOBAL_MDNS) != 0) {
    LOG6LBR_INFO("Starting MDNS\n");
    process_start(&resolv_process, NULL);
    resolv_set_hostname((char *)nvm_data.dns_host_name);
#if RESOLV_CONF_SUPPORTS_DNS_SD
    if((nvm_data.dns_flags & CETIC_6LBR_DNS_DNS_SD) != 0) {
      resolv_add_service("_6lbr._tcp", "", nvm_data.webserver_port);
    }
#endif
  }
#endif

#if CETIC_6LBR_TRANSPARENTBRIDGE
#if CETIC_6LBR_LEARN_RPL_MAC
  LOG6LBR_INFO("Starting as RPL Relay\n");
#else
  LOG6LBR_INFO("Starting as Full TRANSPARENT-BRIDGE\n");
#endif
#elif CETIC_6LBR_SMARTBRIDGE
  LOG6LBR_INFO("Starting as SMART-BRIDGE\n");
#elif CETIC_6LBR_ROUTER
#if UIP_CONF_IPV6_RPL
  LOG6LBR_INFO("Starting as RPL ROUTER\n");
#else
  LOG6LBR_INFO("Starting as NDP ROUTER\n");
#endif
#elif CETIC_6LBR_6LR
  LOG6LBR_INFO("Starting as 6LR\n");
#else
  LOG6LBR_INFO("Starting in UNKNOWN mode\n");
#endif

#if CONTIKI_TARGET_NATIVE
  cetic_6lbr_save_ip();
#endif
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(cetic_6lbr_process, ev, data)
{
  static struct etimer timer;
  static int addr_number;
  PROCESS_BEGIN();

  //Turn off radio until 6LBR is properly configured
  NETSTACK_MAC.off(0);

  cetic_6lbr_restart_event = process_alloc_event();
  cetic_6lbr_reload_event = process_alloc_event();
  cetic_6lbr_startup = clock_seconds();

  LOG6LBR_NOTICE("Starting 6LBR version " CETIC_6LBR_VERSION " (" CONTIKI_VERSION_STRING ")\n");

  platform_init();
  platform_load_config(CONFIG_LEVEL_LOAD);

#if !LOG6LBR_STATIC
  if(nvm_data.log_level != 0xFF) {
    Log6lbr_level = nvm_data.log_level;
    Log6lbr_services = nvm_data.log_services;
  }
  LOG6LBR_NOTICE("Log level: %d (services: %x)\n", Log6lbr_level, Log6lbr_services);
#else
  LOG6LBR_NOTICE("Log level: %d (services: %x)\n", LOG6LBR_LEVEL, LOG6LBR_SERVICE_DEFAULT);
#endif

#if CETIC_6LBR_MAC_WRAPPER
  mac_wrapper_init();
#endif

#if !CETIC_6LBR_ONE_ITF
  platform_radio_init();
  while(!radio_ready) {
    PROCESS_PAUSE();
  }
#endif

  eth_drv_init();

  while(!ethernet_ready) {
    PROCESS_PAUSE();
  }

  //Turn on radio and keep it always on
  NETSTACK_MAC.off(1);

#if CETIC_6LBR_LLSEC_WRAPPER
  framer_wrapper_init();
  llsec_wrapper_init();
#endif

  //6LoWPAN init
  memcpy(addr_contexts[0].prefix, nvm_data.wsn_6lowpan_context_0, sizeof(addr_contexts[0].prefix));

  //clean up any early packet
  uip_len = 0;
  process_start(&tcpip_process, NULL);

  PROCESS_PAUSE();

#if WEBSERVER
  webserver_init();
#endif

#if CETIC_NODE_INFO
  node_info_init();
#endif

#if CETIC_NODE_CONFIG
  node_config_init();
#endif

  packet_filter_init();

  cetic_6lbr_init();

  //Wait result of DAD on 6LBR addresses
  LOG6LBR_INFO("Checking addresses duplication\n");
  addr_number = uip_ds6_get_addr_number(-1);
  etimer_set(&timer, CLOCK_SECOND);
  while(uip_ds6_get_addr_number(ADDR_TENTATIVE) > 0) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    etimer_set(&timer, CLOCK_SECOND);
  }
  if(uip_ds6_get_addr_number(-1) != addr_number) {
    LOG6LBR_FATAL("Addresses duplication failed");
    cetic_6lbr_restart_type = CETIC_6LBR_RESTART;
    platform_restart();
  }
  cetic_6lbr_init_finalize();
  platform_load_config(CONFIG_LEVEL_NETWORK);

#if UDPSERVER
  udp_server_init();
#endif
#if UDPCLIENT
  process_start(&udp_client_process, NULL);
#endif

#if WITH_TINYDTLS
dtls_init();
#endif

#if WITH_COAPSERVER
  if((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_COAP_SERVER) == 0) {
    coap_server_init();
  }
#endif

#if WITH_DTLS_ECHO
  process_start(&dtls_echo_server_process, NULL);
#endif

#if WITH_NVM_PROXY
  nvm_proxy_init();
#endif

#if WITH_DNS_PROXY
  dns_proxy_init();
#endif

  platform_finalize();
  platform_load_config(CONFIG_LEVEL_APP);

  LOG6LBR_INFO("CETIC 6LBR Started\n");

  PROCESS_WAIT_EVENT_UNTIL(ev == cetic_6lbr_restart_event);

  etimer_set(&timer, CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

  //Turn off radio
  NETSTACK_MAC.off(0);
  platform_restart();

  PROCESS_END();
}
