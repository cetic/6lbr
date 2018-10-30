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
#include "uip.h"
#include "sicslowpan.h"
#include "uip-nd6.h"
#include "netstack.h"

#include "log-6lbr.h"

#include "6lbr-main.h"
#include "platform-init.h"
#include "packet-forwarding-engine.h"
#include "network-itf.h"
#include "eth-drv.h"
#include "nvm-config.h"

#include "6lbr-network.h"

#if CETIC_6LBR_WITH_RPL
#include "6lbr-rpl.h"
#endif

#if CETIC_6LBR_WITH_IP64
#include "6lbr-ip64.h"
#endif

#if CETIC_6LBR_FRAMER_WRAPPER
#include "framer-wrapper.h"
#endif

#if CETIC_6LBR_MAC_WRAPPER
#include "mac-wrapper.h"
#endif

#if CETIC_6LBR_MULTI_RADIO
extern const struct mac_driver CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC;
#endif

#if CETIC_6LBR_LLSEC_WRAPPER
#include "llsec-wrapper.h"
#if CETIC_6LBR_WITH_ADAPTIVESEC
#include "strategy-wrapper.h"
#endif
#endif

#if CETIC_6LBR_MULTICAST_WRAPPER
#include "multicast-wrapper.h"
#endif

#if CETIC_6LBR_WITH_WEBSERVER
#include "webserver.h"
#endif

#if CETIC_6LBR_WITH_UDPSERVER
#include "udp-server.h"
#endif

#if CETIC_6LBR_NODE_INFO
#include "node-info.h"
#endif

#if CETIC_6LBR_NODE_CONFIG
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

#if CETIC_6LBR_WITH_NVM_PROXY
#include "nvm-proxy.h"
#endif

#if CETIC_6LBR_WITH_DNS_PROXY
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

//Misc
unsigned long cetic_6lbr_startup;
uint8_t cetic_6lbr_reboot_needed;

enum cetic_6lbr_restart_type_t cetic_6lbr_restart_type;
/*---------------------------------------------------------------------------*/
PROCESS_NAME(udp_client_process);

process_event_t cetic_6lbr_restart_event;
process_event_t cetic_6lbr_reload_event;

PROCESS(cetic_6lbr_process, "CETIC 6LBR process");

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(cetic_6lbr_process, ev, data)
{
  static struct etimer timer;
  static int addr_number;
  PROCESS_BEGIN();

  /* Step 0: Basic infrastructure initialization */

  LOG6LBR_NOTICE("Starting 6LBR version " CETIC_6LBR_VERSION " (" CONTIKI_VERSION_STRING ")\n");

  //Turn off radio until 6LBR is properly configured
#if WITH_CONTIKI
  NETSTACK_MAC.off(0);
#else
  NETSTACK_MAC.off();
#endif

  cetic_6lbr_restart_event = process_alloc_event();
  cetic_6lbr_reload_event = process_alloc_event();
  cetic_6lbr_startup = clock_seconds();

#if CETIC_6LBR_MULTI_RADIO
  network_itf_init();
#endif

  /* Step 1: Platform specific initialization */

  platform_init();

  /* Step 2: Register configuration hooks and set default configuration */

#if CETIC_6LBR_NODE_INFO
  node_info_config();
#endif

  /* Step 3: Load configuration from NVM and configuration file */

  platform_load_config(CONFIG_LEVEL_BOOT);

#if !LOG6LBR_STATIC
  if(nvm_data.log_level != 0xFF) {
    Log6lbr_level = nvm_data.log_level;
    Log6lbr_services = nvm_data.log_services;
  }
  LOG6LBR_NOTICE("Log level: %d (services: %x)\n", Log6lbr_level, Log6lbr_services);
#else
  LOG6LBR_NOTICE("Log level: %d (services: %x)\n", LOG6LBR_LEVEL, LOG6LBR_SERVICE_DEFAULT);
#endif

  /* Step 4: Initialize radio and network interfaces */
#if CETIC_6LBR_FRAMER_WRAPPER
  framer_wrapper_init();
#endif

#if CETIC_6LBR_MAC_WRAPPER
  mac_wrapper_init();
#endif

#if CETIC_6LBR_MULTI_RADIO
  CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC.init();
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
#if WITH_CONTIKI
  NETSTACK_MAC.off(1);
#else
  NETSTACK_MAC.on();
#endif

  /* Step 5: Initialize Network stack */

#if CETIC_6LBR_LLSEC_WRAPPER
#if CETIC_6LBR_WITH_ADAPTIVESEC
  llsec_strategy_wrapper_init();
#endif
  llsec_wrapper_init();
#endif

#if CETIC_6LBR_MULTICAST_WRAPPER
  multicast_wrapper_init();
#endif
  //6LoWPAN init
  memcpy(addr_contexts[0].prefix, nvm_data.wsn_6lowpan_context_0, sizeof(addr_contexts[0].prefix));

  //clean up any early packet
  uip_len = 0;
  process_start(&tcpip_process, NULL);

  PROCESS_PAUSE();

  /* Step 6: Configure network interfaces */

  pfe_init();
  cetic_6lbr_network_init();

  //Wait result of DAD on 6LBR addresses
  addr_number = uip_ds6_get_addr_number(-1);
  LOG6LBR_INFO("Checking addresses duplication\n");
  etimer_set(&timer, CLOCK_SECOND);
  while(uip_ds6_get_addr_number(ADDR_TENTATIVE) > 0) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    etimer_set(&timer, CLOCK_SECOND);
  }
  //Can not use equality as autoconf address could be created when running DAD
  if(uip_ds6_get_addr_number(-1) < addr_number) {
    LOG6LBR_FATAL("Addresses duplication failed\n");
    cetic_6lbr_restart_type = CETIC_6LBR_RESTART;
    platform_restart();
  }

  /* Step 7: Finalize configuration of network interfaces */
#if CETIC_6LBR_WITH_RPL
  cetic_6lbr_init_rpl();
#endif

#if CETIC_6LBR_WITH_IP64
  cetic_6lbr_ip64_init();
#endif

  cetic_6lbr_mdns_init();

  LOG6LBR_INFO("Starting as " CETIC_6LBR_MODE "\n");

#if CONTIKI_TARGET_NATIVE
  cetic_6lbr_save_ip();
#endif

  /* Step 8: Initialize 6LBR core and base applications */

  platform_load_config(CONFIG_LEVEL_CORE);
  PROCESS_PAUSE();

#if CETIC_6LBR_WITH_WEBSERVER
  webserver_init();
#endif

#if CETIC_6LBR_NODE_INFO
  node_info_init();
#endif

#if CETIC_6LBR_NODE_CONFIG
  node_config_init();
#endif

  /* Step 9: Initialize and configure 6LBR applications */

  platform_load_config(CONFIG_LEVEL_BASE);
  PROCESS_PAUSE();

#if CETIC_6LBR_WITH_UDPSERVER
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

#if CETIC_6LBR_WITH_NVM_PROXY
  nvm_proxy_init();
#endif

#if CETIC_6LBR_WITH_DNS_PROXY
  dns_proxy_init();
#endif

  /* Step 10: Finalize platform configuration and load runtime configuration */

  platform_finalize();
  platform_load_config(CONFIG_LEVEL_APP);

  LOG6LBR_INFO("CETIC 6LBR Started\n");

  PROCESS_WAIT_EVENT_UNTIL(ev == cetic_6lbr_restart_event);

  etimer_set(&timer, CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

  /* Shutdown 6LBR */

  //Turn off radio
#if WITH_CONTIKI
  NETSTACK_MAC.off(0);
#else
  NETSTACK_MAC.off();
#endif
  platform_restart();

  PROCESS_END();
}
