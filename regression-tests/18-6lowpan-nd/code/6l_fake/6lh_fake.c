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


#define SHELL 0

#include "contiki.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "sys/etimer.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

void send_packet(uip_ipaddr_t * server_ipaddr);

#if SHELL
#include "../shell-6l.h"
#endif

#include <stdio.h>
#include <string.h>


PROCESS(test_host, "Test process of 6LoWPAN ND host");
AUTOSTART_PROCESSES(&test_host);

/*---------------------------------------------------------------------------*/
void
display_add()
{
#if DEBUG
  int i;
  uint8_t state;

  PRINTF("IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF(" , ");
    }
  }
  PRINTF("\n");
#endif
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_host, ev, data)
{
  static struct etimer periodic_timer;
  char *appdata;

	PROCESS_BEGIN();

#if UIP_CONF_6LN
	PRINTF("STARTING FAKE host (6LN)... \n");
#else
  PRINTF("STARTING unknown device...\n");
#endif
  
#ifdef UIP_CONF_ROUTER
  PRINTF("UIP_CONF_ROUTER:%d\n", UIP_CONF_ROUTER);
#else
  PRINTF("NO UIP_CONF_ROUTER\n");
#endif


#if SHELL
  shell_6l_init();
#else
  printf("Contiki>\n");
#endif

  //Fake mac address
  uip_lladdr.addr[3] = 0x02;
  uip_lladdr.addr[5] = 0x02;
  uip_lladdr.addr[6] = 0x02;
  uip_lladdr.addr[7] = 0x02;

  display_add();

  while(1) {
    PROCESS_YIELD();
  }

  PROCESS_END();
  PRINTF("END PROCESS.\n");
}
