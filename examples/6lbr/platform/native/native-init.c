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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "6LBR"

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include "log-6lbr.h"

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "native-rdc.h"
#include "native-config.h"
#include "plugin.h"
#include "6lbr-watchdog.h"
#include "slip-config.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include "signal.h"

static void
reload_trigger(int signal)
{
  process_post(PROCESS_BROADCAST, cetic_6lbr_reload_event, NULL);
}

void
platform_init(void)
{
  slip_config_handle_arguments(contiki_argc, contiki_argv);
  if (watchdog_interval) {
    process_start(&native_6lbr_watchdog, NULL);
  } else {
    LOG6LBR_WARN("6LBR Watchdog disabled\n");
  }
  load_nvm_config();
  native_config_init();

  plugins_load();
  native_config_load();

  struct sigaction action;
  /* Trap SIGUSR1. */
  action.sa_flags = SA_RESTART;
  action.sa_handler = reload_trigger;
  sigaction(SIGUSR1, &action, NULL);
}

void
platform_finalize(void)
{
  plugins_init();
}

void
platform_radio_init(void)
{
  native_rdc_init();
}

void
platform_set_wsn_mac(linkaddr_t * mac_addr)
{
  linkaddr_set_node_addr(mac_addr);
  slip_set_mac(mac_addr);
}

void
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

void
platform_restart(void)
{
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
}
