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
  uint8_t hop_count;
  uint32_t flags;

  //6lbr-demo udp info
  uint32_t messages_received;
  uint32_t messages_sent;
  uint32_t replies_sent;
  clock_time_t last_message;
  uint16_t  last_up_sequence;
  uint16_t  last_down_sequence;
  uip_ipaddr_t ip_parent;

  //Stats
  clock_time_t stats_start;
  uint32_t up_messages_lost;
  uint32_t down_messages_lost;
  uint16_t parent_switch;
} node_info_t;

#define NODE_INFO_HAS_ROUTE 1
#define NODE_INFO_UPSTREAM_VALID 2
#define NODE_INFO_DOWNSTREAM_VALID 4
#define NODE_INFO_PARENT_VALID 8
#define NODE_INFO_REJECTED 0x10

extern node_info_t node_info_table[UIP_DS6_ROUTE_NB];          /** \brief Node info table */

void
node_info_init(void);

node_info_t *node_info_add(uip_ipaddr_t * ipaddr);

void
node_info_rm(node_info_t *node_info);

void
node_info_rm_by_addr(uip_ipaddr_t * ipaddr);

node_info_t *node_info_lookup(uip_ipaddr_t * ipaddr);

node_info_t *
node_info_update(uip_ipaddr_t * ipaddr, char * info);

void
node_info_node_seen(uip_ipaddr_t * ipaddr, int hop_count);

void
node_info_set_flags(uip_ipaddr_t * ipaddr, uint32_t flags);

void
node_info_clear_flags(uip_ipaddr_t * ipaddr, uint32_t flags);

char const *
node_info_flags_text(uint32_t flags);

void
node_info_reset_statistics(node_info_t * node_info);

void
node_info_reset_statistics_all(void);

#endif
