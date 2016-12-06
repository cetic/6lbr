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

#if CONTIKI_TARGET_NATIVE
#include "node-config-file.h"
#else
#include "node-config-memb.h"
#endif
#if CETIC_NODE_INFO
#include "node-info.h"
#endif
#include "nvm-data.h"
#include "net/rpl/rpl.h"
#include "6lbr-hooks.h"
#include "cetic-6lbr.h"

uint8_t node_config_loaded = 0;

#if CETIC_NODE_CONFIG_HAS_NAME
static char const * unknown_name = "(Unknown)";
#endif

node_config_t * node_config_find_by_ip(uip_ipaddr_t const * ipaddr) {
  if(ipaddr != NULL) {
    uip_lladdr_t ll_addr;
    memcpy(&ll_addr, ipaddr->u8 + 8, UIP_LLADDR_LEN);
    ll_addr.addr[0] ^= 0x02;
    return node_config_find_by_lladdr(&ll_addr);
  } else {
    return NULL;
  }
}

node_config_t * node_config_find_by_port(uint16_t port) {
  node_config_t *  node_config;
  for(node_config = node_config_list_head(); node_config != NULL; node_config = list_item_next(node_config)) {
    if(port == node_config->coap_port || port == node_config->http_port) {
      return node_config;
    }
  }
  return NULL;
}

node_config_t * node_config_find_by_lladdr(uip_lladdr_t const * node_addr) {
  if(node_addr != NULL) {
    node_config_t *  node_config;
    for (node_config = node_config_list_head(); node_config != NULL; node_config = list_item_next(node_config)) {
      if ( memcmp(node_addr, &node_config->mac_address, sizeof(uip_lladdr_t)) == 0 ) {
        return node_config;
      }
    }
  }
  return NULL;
}

#if CETIC_NODE_CONFIG_HAS_NAME
char const *  node_config_get_name(node_config_t const *  node_config) {
  return node_config && node_config->name ? node_config->name : unknown_name;
}
#endif

int node_config_allowed_node_hook(rpl_dag_t *dag, uip_ipaddr_t *prefix, int prefix_len)
{
  /* Test if MAC of incoming node is allowed. */
  int allowed = 0;
  if(dag != NULL) {
    if(node_config_find_by_ip(prefix) == NULL) {
      LOG6LBR_6ADDR(INFO, prefix, "Node has been rejected : ");
    } else {
      LOG6LBR_6ADDR(DEBUG, prefix, "Node has been accepted : ");
      allowed = 1;
    }
  } else {
    if(uip_is_addr_mcast(prefix) || uip_is_addr_linklocal(prefix) || uip_ds6_is_my_addr(prefix) || uip_ds6_is_my_aaddr(prefix) ||
      node_config_find_by_ip(prefix)) {
      allowed = 1;
    }
  }
#if CETIC_NODE_INFO
  if(dag != NULL && allowed) {
    //As control traffic is always allowed, set the flag only when it's coming from RPL
    node_info_clear_flags(prefix, NODE_INFO_REJECTED);
  }
  if(!allowed) {
    node_info_set_flags(prefix, NODE_INFO_REJECTED);
  }
#endif
  return allowed;
}

void node_config_init(void) {
  LOG6LBR_INFO("Node Config init\n");
  node_config_impl_init();
  if((nvm_data.global_flags & CETIC_GLOBAL_FILTER_NODES) != 0) {
    cetic_6lbr_allowed_node_hook = node_config_allowed_node_hook;
  }
}
