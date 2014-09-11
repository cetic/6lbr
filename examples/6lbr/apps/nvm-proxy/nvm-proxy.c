/*
 * Copyright (c) 2014, CETIC.
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
 *         6LBR NVM configuration proxy
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "NVMPROXY"

#include <string.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include "cetic-6lbr.h"
#include "nvm-itf.h"
#include "log-6lbr.h"
#include "nvm-proxy.h"

#ifndef CETIC_6LBR_NVM_PROXY_PORT
#define CETIC_6LBR_NVM_PROXY_PORT 4000
#endif

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])

static struct uip_udp_conn *server_conn;

PROCESS(nvm_proxy_process, "NVM Proxy process");

static void
tcpip_handler(void)
{
  nvm_data_t * user_nvm = (nvm_data_t *)uip_appdata;
  int send_nvm = 1;
  if(uip_newdata()) {
    if (uip_datalen() >= sizeof(nvm_data_t) && user_nvm->magic == CETIC_6LBR_NVM_MAGIC && user_nvm->version == CETIC_6LBR_NVM_CURRENT_VERSION) {
      if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) {
        LOG6LBR_6ADDR(INFO, &UIP_IP_BUF->srcipaddr, "NVM data received from ");
        memcpy((uint8_t *)&nvm_data, (uint8_t *) user_nvm, sizeof(nvm_data));
        store_nvm_config();
        cetic_6lbr_restart_type = CETIC_6LBR_RESTART;
        process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
      } else {
        LOG6LBR_6ADDR(INFO, &UIP_IP_BUF->srcipaddr, "NVM is in read-only, ignoring NVM data received from ");
      }
      send_nvm = 0;
    } else {
      LOG6LBR_6ADDR(WARN, &UIP_IP_BUF->srcipaddr, "Invalid NVM data received from ");
    }
    if (send_nvm) {
      uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
      server_conn->rport = UIP_UDP_BUF->srcport;
      LOG6LBR_6ADDR(INFO, &UIP_IP_BUF->srcipaddr, "Sending current nvm to ");

      uip_udp_packet_send(server_conn, &nvm_data, sizeof(nvm_data));
      /* Restore server connection to allow data from any node */
      memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
      server_conn->rport = 0;
    }
  }
}

PROCESS_THREAD(nvm_proxy_process, ev, data)
{
  PROCESS_BEGIN();
  LOG6LBR_INFO("NVM Proxy started\n");

  server_conn = udp_new(NULL, 0, NULL);
  udp_bind(server_conn, UIP_HTONS(CETIC_6LBR_NVM_PROXY_PORT));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
  }

  PROCESS_END();
}

void nvm_proxy_init(void)
{
  process_start(&nvm_proxy_process, NULL);
}
