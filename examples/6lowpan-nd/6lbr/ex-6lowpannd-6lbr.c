/*
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
 *
 * This file is part of the Contiki operating system.
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/ip/uip-debug.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/netstack.h"
#include "sys/etimer.h"

#include <stdio.h>
#include <string.h>

#include "dev/button-sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define SEND_INTERVAL   (1 * CLOCK_SECOND)
#define CHANGING_INTERVAL (10 * 60 * CLOCK_SECOND)

#define PREFIX_INIT 0xbbbb

 static struct uip_udp_conn *server_conn;

/*---------------------------------------------------------------------------*/
PROCESS(test_router, "Test process of 6LoWPAN ND router");
PROCESS(change_prefix, "Test process of changing prefix");
AUTOSTART_PROCESSES(&test_router, &change_prefix);


/*---------------------------------------------------------------------------*/
static uip_ipaddr_t *
set_global_address(uint16_t pref)
{
  static uip_ipaddr_t ipaddr;
  int i;
  uint8_t state;

  uip_ip6addr(&ipaddr, pref, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  printf("IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      printf(" , ");
    }
  }
  printf("\n");

  return &ipaddr;
}

/*---------------------------------------------------------------------------*/
void
rm_global_address(uint16_t pref)
{
  static uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, pref, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  uip_ds6_addr_rm(uip_ds6_addr_lookup(&ipaddr));
}

/*---------------------------------------------------------------------------*/
void
set_prefix_address(uint16_t pref)
{
  static uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, pref, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_prefix_add(&ipaddr, 64, 1, 0xc0, 86400, 14400);

}

/*---------------------------------------------------------------------------*/
void
set_context_prefix_address(uint16_t pref)
{
  static uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, pref, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_context_pref_add(&ipaddr, 16, 30, -1);
}

/*---------------------------------------------------------------------------*/
void
rm_context_prefix_address(uint16_t pref)
{
  static uip_ipaddr_t ipaddr;
  uip_ds6_context_pref_t *context;
  uip_ip6addr(&ipaddr, pref, 0, 0, 0, 0, 0, 0, 0);

  context = uip_ds6_context_pref_lookup(&ipaddr);
  uip_ds6_context_pref_rm(context);
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_router, ev, data)
{
  uip_ipaddr_t *ipaddr;
  static struct etimer periodic_timer;
  char *appdata;
  static uint16_t pref = PREFIX_INIT;

	PROCESS_BEGIN();
 
#if UIP_CONF_6LBR
  printf("STARTING router (6LBR)...  \n");
#else
  printf("STARTING unknown device...\n");
#endif	

/*
#if UIP_ND6_SEND_RA
  printf("---->%d\n", UIP_ND6_SEND_RA);
#else
  printf("MACRO no define\n");
#endif
*/

  ipaddr = set_global_address(pref);
  set_prefix_address(pref);
  set_context_prefix_address(pref);
  uip_ds6_br_config();

  server_conn = udp_new(NULL, UIP_HTONS(UDP_CLIENT_PORT), NULL);
  if(server_conn == NULL) {
    printf("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(server_conn, UIP_HTONS(UDP_SERVER_PORT));

  printf("Created a server connection with remote address ");
  PRINT6ADDR(&server_conn->ripaddr);
  printf(" local/remote port %u/%u\n", UIP_HTONS(server_conn->lport),
         UIP_HTONS(server_conn->rport));

  //routing table
  uip_ipaddr_t prefix;
  uip_ipaddr_t ipaddr6LR2;
  uip_ip6addr(&prefix, pref, 0, 0, 0, 0, 0, 0, 0);
  uip_ip6addr(&ipaddr6LR2, pref, 0, 0, 0, 0x212, 0x7402, 0x2, 0x202);
  uip_ds6_route_add(&prefix, 32, &ipaddr6LR2);

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      if(uip_newdata()) {
        appdata = (char *)uip_appdata;
        appdata[uip_datalen()] = 0;
        printf("DATA recv '%s'\n", appdata);
      }
    }else if (ev == sensors_event && data == &button_sensor) {
      printf("------> TEST EVENT <-------\n");
    }
  }


  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(change_prefix, ev, data)
{
  static struct etimer periodic_timer;
  PROCESS_BEGIN();

  static uint16_t pref;

  pref = PREFIX_INIT;

  etimer_set(&periodic_timer, CHANGING_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);
    //CHANGING
    printf("---- CHANGING VERSION -----");
    rm_global_address(pref);
    rm_context_prefix_address(pref);
    pref += 0x1111;
    printf("--> %x\n", pref);
    set_global_address(pref);
    set_context_prefix_address(pref);
    etimer_set(&periodic_timer, CHANGING_INTERVAL);
  }

  PROCESS_END();
}
