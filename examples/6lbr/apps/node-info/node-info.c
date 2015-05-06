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

#include "contiki.h"
#include "node-info.h"
#include "uip-ds6.h"
#include "uip-ds6-route.h"
#include "string.h"
#include "stdlib.h"

node_info_t node_info_table[UIP_DS6_ROUTE_NB];          /** \brief Node info table */

static struct uip_ds6_notification node_info_route_notification;

void
node_info_route_notification_cb(int event,
                                uip_ipaddr_t * route,
                                uip_ipaddr_t * nexthop, int num_routes)
{
  if(event == UIP_DS6_NOTIFICATION_ROUTE_ADD) {
    node_info_t *node = NULL;
    node = node_info_lookup(route);
    if(node == NULL) {
      node = node_info_add(route);
    }
    if(node != NULL) {
      node->last_seen = clock_time();
    }
 } else if(event == UIP_DS6_NOTIFICATION_ROUTE_RM) {
    node_info_rm(route);
  }
}

void
node_info_init(void)
{
  memset(node_info_table, 0, sizeof(node_info_table));
  uip_ds6_notification_add(&node_info_route_notification,
                           node_info_route_notification_cb);
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
  }
  return node;
}

node_info_t *
node_info_update(uip_ipaddr_t * ipaddr, char * info)
{
  node_info_t *node = NULL;
  char *  sep;
  node = node_info_lookup(ipaddr);
  if (node == NULL) {
    node = node_info_add(ipaddr);
  }
  if ( node != NULL ) {
    node->last_seen = clock_time();
    node->last_message = clock_time();
    node->messages_count++;
    sep = index(info, '|');
    if (sep != NULL && sep - info > 0) {
      *sep = 0;
      node->sequence = atoi(info);
      info = sep + 1;
      if (*info == ' ') {
        info++;
      }
      sep = index(info, '|');
      if (sep != NULL) {
        *sep = 0;
      }
      if (uiplib_ipaddrconv(info, &node->ip_parent) == 0) {
        uip_create_unspecified(&node->ip_parent);
      }
    } else {
      node->sequence = 0;
      uip_create_unspecified(&node->ip_parent);
    }
  }
  return node;
}

void
node_info_node_seen(uip_ipaddr_t * ipaddr)
{
  node_info_t *node = NULL;
  node = node_info_lookup(ipaddr);
  if ( node != NULL ) {
    node->last_seen = clock_time();
  }
}

void
node_info_rm(uip_ipaddr_t * ipaddr)
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
