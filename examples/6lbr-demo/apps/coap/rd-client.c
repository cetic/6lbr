/*
 * Copyright (c) 2015, CETIC.
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
 *         Simple CoAP Library
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"

#include "er-coap-engine.h"

#include "coap-common.h"
#include "core-interface.h"
#include "rd-client.h"

#if WITH_NVM
#include "nvm-config.h"
#endif

#include <string.h>

#define DEBUG 1
#include "net/ip/uip-debug.h"

PROCESS(rd_client_process, "RD Client");

static uip_ipaddr_t rd_server_ipaddr;

#define REGISTRATION_NAME_MAX_SIZE 64
static char registration_name[REGISTRATION_NAME_MAX_SIZE+1];

#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xbbbb, 0, 0, 0, 0xa, 0xbff, 0xfe0c, 0xd0e)

static uint8_t registered = 0;
/*---------------------------------------------------------------------------*/
void
client_registration_response_handler(void *response)
{
  if(response != NULL && ((coap_packet_t *)response)->code == CREATED_2_01) {
    const char *str=NULL;
    int len = coap_get_header_location_path(response, &str);
    if(len > 0) {
      registration_name[0] = '/';
      memcpy(registration_name+1, str, len);
      registration_name[len+1] = '\0';
      PRINTF("Registration name: %s\n", registration_name);
      registered = 1;
    } else {
      PRINTF("Location-path missing\n");
    }
  } else {
    PRINTF("Timeout or error status\n");
  }
}
/*---------------------------------------------------------------------------*/
void
client_update_response_handler(void *response)
{
  if(response != NULL && ((coap_packet_t *)response)->code == CHANGED_2_04) {
    PRINTF("Updated\n");
    registered = 1;
  } else {
    PRINTF("Timeout or error status\n");
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rd_client_process, ev, data)
{
  static struct etimer et;
  static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */

  PROCESS_BEGIN();
  PRINTF("RD client started\n");
  SERVER_NODE(&rd_server_ipaddr);
  while(1) {
    while(!registered) {
      coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
      coap_set_header_uri_path(request, "/rd");
      coap_set_header_uri_query(request, "?ep=test&b=U&lt=60");
      static const char msg[] = "";
      coap_set_payload(request, (uint8_t *) msg, sizeof(msg) - 1);

      COAP_BLOCKING_REQUEST(coap_default_context, &rd_server_ipaddr, UIP_HTONS(COAP_DEFAULT_PORT), request, client_registration_response_handler);
    }
    etimer_set(&et, RD_CLIENT_LIFETIME * CLOCK_SECOND);
    PROCESS_YIELD_UNTIL(etimer_expired(&et));

    registered = 0;
    coap_init_message(request, COAP_TYPE_CON, COAP_PUT, 0);
    coap_set_header_uri_path(request, registration_name);
    coap_set_header_uri_query(request, "?b=U&lt=60");

    COAP_BLOCKING_REQUEST(coap_default_context, &rd_server_ipaddr, UIP_HTONS(COAP_DEFAULT_PORT), request, client_update_response_handler);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
rd_client_init(void)
{
  process_start(&rd_client_process, NULL);
}
/*---------------------------------------------------------------------------*/
