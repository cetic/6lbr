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
 *         6LBR Node config
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "NODECFG"

#include "node-config.h"
#include "log-6lbr.h"
#include "uip-ds6-route.h"

#include "node-config-memb.h"
#include "cetic-6lbr.h"

#define COAP_DEFAULT_PORT                    5683

LIST(node_config_list);
MEMB(node_config_memb, node_config_t, UIP_DS6_ROUTE_NB);

static uint16_t node_config_first_coap_port = 20000;
static uint16_t node_config_first_http_port = 25000;
static uint16_t node_config_coap_port;
static uint16_t node_config_http_port;

node_config_t* node_config_new(void)
{
  node_config_t * node_config;
  node_config = memb_alloc(&node_config_memb);

  if(node_config == NULL) {
    return NULL;
  }

  return node_config;
}

node_config_t* node_config_add_node(uip_lladdr_t *mac_address) {
  node_config_t *  node_config = node_config_new();
  if(node_config == NULL) {
    return NULL;
  }

#if CETIC_NODE_CONFIG_HAS_NAME
  node_config->name = NULL;
#endif
  node_config->mac_address = *mac_address;
  node_config->coap_port = node_config_coap_port++;
  node_config->http_port = node_config_http_port++;

  list_add(node_config_list, node_config);
  return node_config;
}

static struct uip_ds6_notification node_config_route_notification;

static void
node_config_route_notification_cb(int event,
                                uip_ipaddr_t * route,
                                uip_ipaddr_t * nexthop, int num_routes)
{
  node_config_t *node_config = NULL;
  if(event == UIP_DS6_NOTIFICATION_ROUTE_ADD) {
    node_config = node_config_find_by_ip(route);
    if(node_config == NULL) {
      LOG6LBR_6ADDR(DEBUG, route, "Adding node config for ");
      uip_lladdr_t ll_addr;
      memcpy(&ll_addr, route->u8 + 8, UIP_LLADDR_LEN);
      ll_addr.addr[0] ^= 0x02;
      node_config = node_config_add_node(&ll_addr);
    }
  }
}

void node_config_add_br(void) {
  node_config_t *  node_config = node_config_new();
  if(node_config == NULL) {
    return;
  }
#if CETIC_NODE_CONFIG_HAS_NAME
  node_config->name = "BR";
#endif
  node_config->mac_address = wsn_mac_addr;
  node_config->coap_port = COAP_DEFAULT_PORT;
  node_config->http_port = 80;
  list_add(node_config_list, node_config);
}

void node_config_impl_init(void) {
  memb_init(&node_config_memb);
  list_init(node_config_list);
  node_config_coap_port = node_config_first_coap_port;
  node_config_http_port = node_config_first_http_port;
  uip_ds6_notification_add(&node_config_route_notification,
      node_config_route_notification_cb);
  node_config_loaded = 1;
  node_config_add_br();
}

node_config_t* node_config_list_head(void)
{
  return list_head(node_config_list);
}
