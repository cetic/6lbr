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

#define LOG6LBR_MODULE "NODE"

#include "contiki.h"
#include "node-info.h"
#include "uip-ds6.h"
#include "uip-ds6-route.h"
#include "string.h"
#include "stdlib.h"

#include "log-6lbr.h"

#if CETIC_NODE_INFO_EXPORT
#include "node-info-export.h"
#endif

node_info_t node_info_table[UIP_DS6_ROUTE_NB];          /** \brief Node info table */

static struct uip_ds6_notification node_info_route_notification;

void
node_info_route_notification_cb(int event,
                                uip_ipaddr_t * route,
                                uip_ipaddr_t * nexthop, int num_routes)
{
  if(event == UIP_DS6_NOTIFICATION_ROUTE_ADD) {
    node_info_set_flags(route, NODE_INFO_HAS_ROUTE);
  } else if(event == UIP_DS6_NOTIFICATION_ROUTE_RM) {
    node_info_clear_flags(route, NODE_INFO_HAS_ROUTE);
  }
}

void
node_info_init(void)
{
  memset(node_info_table, 0, sizeof(node_info_table));
  uip_ds6_notification_add(&node_info_route_notification,
                           node_info_route_notification_cb);
#if CETIC_NODE_INFO_EXPORT
  node_info_export_init();
#endif
}

node_info_t *
node_info_add(uip_ipaddr_t * ipaddr)
{
  node_info_t *node = NULL;

  if(uip_ds6_list_loop
     ((uip_ds6_element_t *) node_info_table, UIP_DS6_ROUTE_NB,
      sizeof(node_info_t), ipaddr, 128,
      (uip_ds6_element_t **) & node) == FREESPACE) {
    memset(node, 0, sizeof(node_info_t));
    node->isused = 1;
    uip_ipaddr_copy(&(node->ipaddr), ipaddr);
    node->stats_start = clock_time();
    LOG6LBR_6ADDR(DEBUG, ipaddr, "New node created ");
  } else {
    LOG6LBR_6ADDR(ERROR, ipaddr, "Not enough memory to create node ");
  }
  return node;
}

node_info_t *
node_info_update(uip_ipaddr_t * ipaddr, char * info)
{
  node_info_t *node = NULL;
  char *  sep;
  uip_ipaddr_t ip_parent;

  node = node_info_lookup(ipaddr);
  if (node == NULL) {
    node = node_info_add(ipaddr);
  }
  if ( node != NULL ) {
    node->last_seen = clock_time();
    node->last_message = clock_time();
    uint16_t up_sequence = 0;
    uint16_t down_sequence = 0;

    sep = index(info, '|');
    if (sep != NULL) {
      node->messages_received++;
      up_sequence = atoi(info);
      node->flags |= NODE_INFO_UPSTREAM_VALID;
      *sep = 0;
      info = sep + 1;
      if (*info == ' ') {
        info++;
      }
      sep = index(info, '|');
      if (sep != NULL) {
        *sep = 0;
      }
      if (uiplib_ipaddrconv(info, &ip_parent) == 0) {
        uip_create_unspecified(&ip_parent);
        node->flags &= ~NODE_INFO_PARENT_VALID;
      } else {
        node->flags |= NODE_INFO_PARENT_VALID;
      }
      if(!uip_ipaddr_cmp(&node->ip_parent, &ip_parent)) {
        uip_ipaddr_copy(&(node->ip_parent), &ip_parent);
        if (node->messages_received > 1) {
          node->parent_switch++;
        }
      }
      if (sep != NULL) {
        info = sep + 1;
        down_sequence = atoi(info);
        node->flags |= NODE_INFO_DOWNSTREAM_VALID;
      } else {
        node->flags &= ~NODE_INFO_DOWNSTREAM_VALID;
      }
      if (node->messages_received > 1) {
        uint16_t up_delta = up_sequence - node->last_up_sequence;
        if (up_delta < 100) {
          node->messages_sent += up_delta;
          node->up_messages_lost += up_delta - 1;
          if((node->flags & NODE_INFO_DOWNSTREAM_VALID) != 0 &&
              down_sequence != node->last_down_sequence + 1) {
            node->down_messages_lost += 1;
          }
        } else {
          //Reset statistics
          node->messages_sent = 1;
          node->replies_sent = 0;
          node->up_messages_lost = 0;
          node->down_messages_lost = 0;
        }
      } else {
        node->messages_sent = 1;
        node->replies_sent = 0;
        node->up_messages_lost = 0;
        node->down_messages_lost = 0;
      }
      node->last_up_sequence = up_sequence;
      node->last_down_sequence = down_sequence;
    } else {
      node->flags &= ~NODE_INFO_UPSTREAM_VALID;
      node->flags &= ~NODE_INFO_DOWNSTREAM_VALID;
      node->flags &= ~NODE_INFO_PARENT_VALID;
      node->last_up_sequence = 0;
      node->last_down_sequence = 0;
      uip_create_unspecified(&node->ip_parent);
    }
  }
  return node;
}

void
node_info_node_seen(uip_ipaddr_t * ipaddr, int hop_count)
{
  node_info_t *node = NULL;
  node = node_info_lookup(ipaddr);
  if ( node != NULL ) {
    node->last_seen = clock_time();
    if(hop_count != -1) {
      node->hop_count = hop_count;
    }
  }
}

void
node_info_set_flags(uip_ipaddr_t * ipaddr, uint32_t flags)
{
  node_info_t *node = NULL;
  node = node_info_lookup(ipaddr);
  if(node == NULL) {
    node = node_info_add(ipaddr);
  }
  if ( node != NULL ) {
    node->last_seen = clock_time();
    node->flags |= flags;
  }
}

void
node_info_clear_flags(uip_ipaddr_t * ipaddr, uint32_t flags)
{
  node_info_t *node = NULL;
  node = node_info_lookup(ipaddr);
  if(node == NULL) {
    node = node_info_add(ipaddr);
  }
  if ( node != NULL ) {
    node->last_seen = clock_time();
    node->flags &= ~flags;
  }
}

char const *
node_info_flags_text(uint32_t flags)
{
  if((flags & NODE_INFO_REJECTED) != 0) {
    return "REJECTED";
  } else {
    if((flags & NODE_INFO_HAS_ROUTE) != 0) {
      return "OK";
    } else {
      return "NR";
    }
  }
}

void
node_info_rm(node_info_t *node_info)
{
  if(node_info != NULL) {
    node_info->isused = 0;
    LOG6LBR_6ADDR(DEBUG, &node_info->ipaddr, "Removing node ");
  }
}

void
node_info_rm_by_addr(uip_ipaddr_t * ipaddr)
{
  node_info_t *node_info = node_info_lookup(ipaddr);

  if(node_info != NULL) {
    node_info->isused = 0;
  }
}

node_info_t *
node_info_lookup(uip_ipaddr_t * ipaddr)
{
  node_info_t *node;

  if(uip_ds6_list_loop((uip_ds6_element_t *) node_info_table,
                       UIP_DS6_ROUTE_NB, sizeof(node_info_t), ipaddr, 128,
                       (uip_ds6_element_t **) & node) == FOUND) {
    return node;
  }
  return NULL;
}

void
node_info_reset_statistics(node_info_t * node_info)
{
  node_info->stats_start = clock_time();
  node_info->messages_received = 0;
  node_info->messages_sent = 0;
  node_info->replies_sent = 0;
  node_info->up_messages_lost = 0;
  node_info->down_messages_lost = 0;
  node_info->parent_switch = 0;
}

void
node_info_reset_statistics_all(void)
{
  int i;
  for(i = 0; i < UIP_DS6_ROUTE_NB; ++i) {
    if(node_info_table[i].isused) {
      node_info_reset_statistics(&node_info_table[i]);
    }
  }
}
