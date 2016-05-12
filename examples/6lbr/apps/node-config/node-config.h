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

#ifndef NODE_CONFIG_H_
#define NODE_CONFIG_H_

#include "contiki-net.h"

/* Temporarily until more complete support of node-config */
#if CONTIKI_TARGET_NATIVE
#define CETIC_NODE_CONFIG_HAS_NAME 1
#else
#define CETIC_NODE_CONFIG_HAS_NAME 0
#endif

struct node_config {
  struct node_config * next;
  uip_lladdr_t mac_address;
#if CETIC_NODE_CONFIG_HAS_NAME
  char const * name;
#endif
  /* Temporarily hardcoded list of ports */
  uint16_t coap_port;
  uint16_t http_port;
};

typedef struct node_config node_config_t;
extern uint8_t node_config_loaded;

void node_config_init(void);
node_config_t* node_config_list_head(void);

node_config_t * node_config_find_by_ip(uip_ipaddr_t const * ipaddr);
node_config_t * node_config_find_by_lladdr(uip_lladdr_t const * node_addr);

node_config_t * node_config_find_by_port(uint16_t port);
#if CETIC_NODE_CONFIG_HAS_NAME
char const *  node_config_get_name(node_config_t const * node_config);
#endif

#endif /* NODE_CONFIG_H_ */
