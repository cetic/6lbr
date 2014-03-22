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


#define SHELL 1

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

#define UDP_PORT 8765
#define MAX_PAYLOAD_LEN   30

#define SEND_INTERVAL   (60 * CLOCK_SECOND)

#define BUFLEN 100

static struct uip_udp_conn *client_conn;



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
void
send_packet(uip_ipaddr_t * server_ipaddr)
{
  static int seq_id;
  char buf[MAX_PAYLOAD_LEN];

  seq_id++;
  printf("DATA 'Hello %d' send to %d \n",
         server_ipaddr->u8[sizeof(server_ipaddr->u8) - 1], seq_id);
  sprintf(buf, "Hello %d from the client", seq_id);
  uip_udp_packet_sendto(client_conn, buf, strlen(buf),
                        server_ipaddr, UIP_HTONS(UDP_PORT));
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_host, ev, data)
{
  static struct etimer periodic_timer;
  char *appdata;

	PROCESS_BEGIN();

#if UIP_CONF_6LN
	PRINTF("STARTING host (6LN)... \n");
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
#endif

  display_add();

  client_conn = udp_new(NULL, UIP_HTONS(UDP_PORT), NULL); 
  if(client_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(client_conn, UIP_HTONS(UDP_PORT)); 

  PRINTF("Created a connection with the server ");
  PRINT6ADDR(&client_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n",
  UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));


  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event && uip_newdata()) {
      appdata = (char *)uip_appdata;
      appdata[uip_datalen()] = 0;
      printf("DATA recv '%s'\n", appdata);
    }
  }

  PROCESS_END();
  PRINTF("END PROCESS.\n");
}
