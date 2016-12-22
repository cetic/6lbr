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

#define LOG6LBR_MODULE "DNS"

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "log-6lbr.h"
#include "ip64-addr.h"

#include <string.h>

static struct uip_udp_conn *server_conn;

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])
extern uint16_t uip_slen;

PROCESS(dns_proxy_process, "DNS proxy process");

static void
tcpip_handler(void)
{
  if(uip_newdata()) {
    uip_ipaddr_t * nameserver = uip_nameserver_get(0);
    if(!uip_is_addr_unspecified(nameserver)) {
      LOG6LBR_6ADDR(DEBUG, &UIP_IP_BUF->srcipaddr, "Forwarding DNS request from ");
      uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, nameserver);
      uip_slen = uip_len;
      uip_len = uip_slen + UIP_IPUDPH_LEN;
      UIP_UDP_BUF->udpchksum = 0;
  #if UIP_UDP_CHECKSUMS
      /* Calculate UDP checksum. */
      UIP_UDP_BUF->udpchksum = ~(uip_udpchksum());
      if(UIP_UDP_BUF->udpchksum == 0) {
        UIP_UDP_BUF->udpchksum = 0xffff;
      }
  #endif /* UIP_UDP_CHECKSUMS */
      uip_flags = 0;
      tcpip_ipv6_output();
      uip_slen = 0;
    } else {
      LOG6LBR_DEBUG( "DNS request dropped, not name server configured\n");
    }
  }
}

PROCESS_THREAD(dns_proxy_process, ev, data)
{
  PROCESS_BEGIN();
  LOG6LBR_INFO("DNS proxy started\n");

  server_conn = udp_new(NULL, 0, NULL);
  udp_bind(server_conn, UIP_HTONS(53));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
  }

  PROCESS_END();
}

void dns_proxy_init(void)
{
  if((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_DNS_PROXY) == 0) {
    process_start(&dns_proxy_process, NULL);
  }
}
