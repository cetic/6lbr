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

#include "node-info.h"

#if CONTIKI_TARGET_NATIVE
#include "6lbr-watchdog.h"
#include "slip-config.h"
#include <arpa/inet.h>
#endif

//Initialisation flags
int ethernet_ready = 0;
int eth_mac_addr_ready = 0;

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

//Misc
unsigned long cetic_6lbr_startup;

enum cetic_6lbr_restart_type_t cetic_6lbr_restart_type;

/*---------------------------------------------------------------------------*/
PROCESS_NAME(webserver_nogui_process);
PROCESS_NAME(udp_server_process);
PROCESS(cetic_6lbr_process, "CETIC Bridge process");

AUTOSTART_PROCESSES(&cetic_6lbr_process);

/*---------------------------------------------------------------------------*/

#if CONTIKI_TARGET_NATIVE
static void
cetic_6lbr_save_ip(void)
{
  if (ip_config_file_name) {
    char str[INET6_ADDRSTRLEN];
#if CETIC_6LBR_SMARTBRIDGE
    inet_ntop(AF_INET6, (struct sockaddr_in6 *)&wsn_ip_addr, str, INET6_ADDRSTRLEN);
#else
    inet_ntop(AF_INET6, (struct sockaddr_in6 *)&eth_ip_addr, str, INET6_ADDRSTRLEN);
#endif
    FILE *ip_config_file = fopen(ip_config_file_name, "w");
    fprintf(ip_config_file, "%s\n", str);
    fclose(ip_config_file);
  }
}
#endif

void
cetic_6lbr_set_prefix(uip_ipaddr_t * prefix, unsigned len,
                      uip_ipaddr_t * ipaddr)
{
#if CETIC_6LBR_SMARTBRIDGE
  int new_prefix = cetic_dag != NULL && !uip_ipaddr_prefixcmp(&cetic_dag->prefix_info.prefix, prefix, len);
  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    LOG6LBR_DEBUG("Ignoring RA\n");
    return;
  }
  LOG6LBR_INFO("CETIC_BRIDGE : set_prefix\n");

  uip_ipaddr_copy(&wsn_ip_addr, ipaddr);

  if(cetic_dag != NULL) {
    rpl_set_prefix(cetic_dag, prefix, len);
    uip_ipaddr_copy(&wsn_net_prefix, prefix);
    wsn_net_prefix_len = len;
    if(new_prefix) {
      LOG6LBR_6ADDR(INFO, prefix, "Setting DAG prefix : ");
      rpl_repair_root(RPL_DEFAULT_INSTANCE);
    }
  }
#if CONTIKI_TARGET_NATIVE
  cetic_6lbr_save_ip();
#endif
#endif
}

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
  }                             //End manual configuration
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
#endif

#if UIP_CONF_IPV6_RPL && CETIC_6LBR_DODAG_ROOT
  //DODAGID = link-local address used !
  cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &wsn_ip_local_addr);
#if CETIC_6LBR_SMARTBRIDGE
  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
  }
#else
  rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
#endif
  LOG6LBR_INFO("Configured as DODAG Root\n");
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

#if CONTIKI_TARGET_NATIVE
static struct etimer reboot_timer;
#endif

PROCESS_THREAD(cetic_6lbr_process, ev, data)
{
  PROCESS_BEGIN();

  cetic_6lbr_startup = clock_seconds();

#if CONTIKI_TARGET_NATIVE
  slip_config_handle_arguments(contiki_argc, contiki_argv);
  if (watchdog_interval) {
    process_start(&native_6lbr_watchdog, NULL);
  } else {
    LOG6LBR_WARN("6LBR Watchdog disabled\n");
  }
#endif

  LOG6LBR_INFO("Starting 6LBR version " CETIC_6LBR_VERSION " (" CONTIKI_VERSION_STRING ")\n");

  load_nvm_config();

  platform_init();

  process_start(&eth_drv_process, NULL);

  while(!ethernet_ready) {
    PROCESS_PAUSE();
  }

  //clean up any early packet
  uip_len = 0;
  process_start(&tcpip_process, NULL);

  PROCESS_PAUSE();

#if CETIC_NODE_INFO
  node_info_init();
#endif

  packet_filter_init();
  cetic_6lbr_init();

#if WEBSERVER
  process_start(&webserver_nogui_process, NULL);
#endif
#if UDPSERVER
  process_start(&udp_server_process, NULL);
#endif

  LOG6LBR_INFO("CETIC 6LBR Started\n");

#if CONTIKI_TARGET_NATIVE
  PROCESS_WAIT_EVENT();
  etimer_set(&reboot_timer, CLOCK_SECOND);
  PROCESS_WAIT_EVENT();
  switch (cetic_6lbr_restart_type) {
    case CETIC_6LBR_RESTART:
      LOG6LBR_INFO("Exiting...\n");
      exit(0);
      break;
    case CETIC_6LBR_REBOOT:
      LOG6LBR_INFO("Rebooting...\n");
      system("reboot");
      break;
    case CETIC_6LBR_HALT:
      LOG6LBR_INFO("Halting...\n");
      system("halt");
      break;
    default:
      //We should never end up here...
      exit(1);
  }
  //We should never end up here...
  exit(1);
#endif

  PROCESS_END();
}
