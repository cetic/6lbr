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
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/netstack.h"
#include "sys/etimer.h"

#include <stdio.h>
#include <string.h>

#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

void send_packet(uip_ipaddr_t * server_ipaddr);

#if SHELL
#include "../shell-6l.h"
#endif

#define UDP_PORT 8765
#define MAX_PAYLOAD_LEN   30

#define SEND_INTERVAL   (1 * CLOCK_SECOND)

 static struct uip_udp_conn *server_conn;

/*---------------------------------------------------------------------------*/
PROCESS(test_router, "Test process of 6LoWPAN ND router");
AUTOSTART_PROCESSES(&test_router);

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
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
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
  uip_udp_packet_sendto(server_conn, buf, strlen(buf),
                        server_ipaddr, UIP_HTONS(UDP_PORT));
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_router, ev, data)
{
  uip_ipaddr_t *ipaddr;
  static struct etimer periodic_timer;
  char *appdata;

  PROCESS_BEGIN();
 
#if UIP_CONF_6LR
  printf("STARTING router (6LR)...  \n");
#else
  printf("STARTING unknown device...\n");
#endif  

#if SHELL
  shell_6l_init();
#endif

/*
#if UIP_ND6_SEND_RA
  printf("---->%d\n", UIP_ND6_SEND_RA);
#else
  printf("MACRO no define\n");
#endif
*/

  display_add();

  server_conn = udp_new(NULL, UIP_HTONS(UDP_PORT), NULL);
  if(server_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(server_conn, UIP_HTONS(UDP_PORT));

  PRINTF("Created a server connection with remote address ");
  PRINT6ADDR(&server_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n", UIP_HTONS(server_conn->lport),
         UIP_HTONS(server_conn->rport));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event && uip_newdata()) {
        appdata = (char *)uip_appdata;
        appdata[uip_datalen()] = 0;
        printf("DATA recv '%s'\n", appdata);
    }
  }


  PROCESS_END();
  PRINTF("END PROCESS :() \n");
}
