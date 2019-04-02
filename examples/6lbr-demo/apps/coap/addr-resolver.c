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
 *         Address resolver thread
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"
#include "contiki-net.h"
#include "ip64-addr.h"

#include "addr-resolver.h"

#define DEBUG 0
#include "net/ip/uip-debug.h"

PT_THREAD(gethostbyname(struct pt *pt, process_event_t ev, void *data, char const *host, uip_ipaddr_t *ipaddr))
{
  PT_BEGIN(pt);
  uip_ip4addr_t ip4addr;
  uip_ipaddr_t *resolv_result;
  if(uiplib_ip4addrconv(host, &ip4addr) != 0) {
    ip64_addr_4to6(&ip4addr, ipaddr);
  } else if(uiplib_ip6addrconv(host, ipaddr) != 0) {
    /* IP address already un ipaddr */
  } else {
    resolv_status_t ret = resolv_lookup(host, &resolv_result);
    if(ret == RESOLV_STATUS_UNCACHED || ret == RESOLV_STATUS_EXPIRED) {
      resolv_query(host);
      PRINTF("Resolving host '%s'\n", host);
      while(1) {
        PT_YIELD_UNTIL(pt, ev == resolv_event_found);
        const char *name = data;
        if(data == NULL) {
          PRINTF("Error resolving '%s'\n", host);
          uip_create_unspecified(ipaddr);
          break;
        } else if(strcmp(name, host) == 0) {
          if(resolv_lookup(name, &resolv_result) == RESOLV_STATUS_CACHED) {
            PRINTF("Host '%s' resolved : ", host);
            PRINT6ADDR(resolv_result);
            PRINTF("\n");
            uip_ipaddr_copy(ipaddr, resolv_result);
           break;
          } else {
            PRINTF("Host '%s' not found\n", host);
            uip_create_unspecified(ipaddr);
            break;
          }
        }
      }
    } else if(ret == RESOLV_STATUS_CACHED) {
      PRINTF("Found host '%s' in cache : ", host);
      PRINT6ADDR(resolv_result);
      PRINTF("\n");
      uip_ipaddr_copy(ipaddr, resolv_result);
    } else {
      PRINTF("Host '%s' not found\n", host);
      uip_create_unspecified(ipaddr);
    }
  }
  PT_END(pt);
}
