/*
 * Copyright (c) 2013, CETIC.
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

#ifndef NODE_INFO_H_
#define NODE_INFO_H_

#include <contiki.h>
#include <contiki-net.h>

/** \brief An entry in the node info table */
typedef struct node_info {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
  clock_time_t last_seen;

  //6lbr-demo udp info
  uint32_t messages_count;
  clock_time_t last_message;
  uint16_t  sequence;
  uip_ipaddr_t ip_parent;
} node_info_t;

extern node_info_t node_info_table[UIP_DS6_ROUTE_NB];          /** \brief Node info table */

void
  node_info_init(void);

node_info_t *node_info_add(uip_ipaddr_t * ipaddr);

void
  node_info_rm(uip_ipaddr_t * ipaddr);

node_info_t *node_info_lookup(uip_ipaddr_t * ipaddr);

node_info_t *
node_info_update(uip_ipaddr_t * ipaddr, char * info);

void
node_info_node_seen(uip_ipaddr_t * ipaddr);

#endif
