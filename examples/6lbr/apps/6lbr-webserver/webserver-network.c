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
 * \file
 *         6LBR Web Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "WEB"

#include "contiki.h"
#include "sicslowpan.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6-route.h"
#include "sicslow-ethernet.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#if CETIC_6LBR_IP64
#include "ip64.h"
#include "ip64-addrmap.h"
#include "ip64-dhcpc.h"
#endif

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "rio.h"
#include "log-6lbr.h"

#if CETIC_NODE_INFO
#include "node-info.h"
#endif

#if CETIC_NODE_CONFIG
#include "node-config.h"
#endif

extern uip_ds6_prefix_t uip_ds6_prefix_list[];

static void add_network_cases(const uint8_t state);

static
PT_THREAD(generate_network(struct httpd_state *s))
{
  static int i;
  static uip_ds6_route_t *r;
  static uip_ds6_defrt_t *dr;
  static uip_ds6_nbr_t *nbr;

  PSOCK_BEGIN(&s->sout);
  add("<br /><h2>Addresses</h2><pre>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    if(uip_ds6_if.addr_list[i].isused) {
      ipaddr_add(&uip_ds6_if.addr_list[i].ipaddr);
      char flag;

      if(uip_ds6_if.addr_list[i].state == ADDR_TENTATIVE) {
        flag = 'T';
      } else if(uip_ds6_if.addr_list[i].state == ADDR_PREFERRED) {
        flag = 'P';
      } else {
        flag = '?';
      }
      add(" %c", flag);
      if(uip_ds6_if.addr_list[i].type == ADDR_MANUAL) {
        flag = 'M';
      } else if(uip_ds6_if.addr_list[i].type == ADDR_DHCP) {
        flag = 'D';
      } else if(uip_ds6_if.addr_list[i].type == ADDR_AUTOCONF) {
        flag = 'A';
      } else {
        flag = '?';
      }
      add(" %c", flag);
      if(!uip_ds6_if.addr_list[i].isinfinite) {
        add(" %u s", stimer_remaining(&uip_ds6_if.addr_list[i].vlifetime));
      }
      add("\n");
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }

  add("</pre><h2>Prefixes</h2><pre>");
  for(i = 0; i < UIP_DS6_PREFIX_NB; i++) {
    if(uip_ds6_prefix_list[i].isused) {
      ipaddr_add(&uip_ds6_prefix_list[i].ipaddr);
      add(" ");
#if UIP_CONF_ROUTER
      if(uip_ds6_prefix_list[i].advertise) {
        add("A");
      }
#else
      if(uip_ds6_prefix_list[i].isinfinite) {
        add("I");
      }
#endif
      add("\n");
    }
  }
  add("</pre>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

#if CETIC_6LBR_IP64
  if((nvm_data.global_flags & CETIC_GLOBAL_IP64) != 0) {
    add("<h2>IP64</h2>");
    if((nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_DHCP) == 0 || ip64_hostaddr_is_configured()) {
      add("Address : ");
      ip4addr_add(ip64_get_hostaddr());
      add("<br />");
      add("Netmask : ");
      ip4addr_add(ip64_get_netmask());
      add("<br />");
      add("Gateway : ");
      ip4addr_add(ip64_get_draddr());
      add("<br />");
      if((nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_DHCP) != 0) {
        extern struct ip64_dhcpc_state *ip64_dhcp_state;
        add("DHCP Server : ");
        ip4addr_add_u8(ip64_dhcp_state->serverid);
        add("<br />");
        add("DHCP lease time : %d s<br />", uip_ntohs(ip64_dhcp_state->lease_time[0])*65536ul + uip_ntohs(ip64_dhcp_state->lease_time[1]));
      }
    } else {
      add("Waiting configuration<br />");
    }
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }
#endif

  add("<br /><h2>Neighbors</h2><pre>");

  for(nbr = nbr_table_head(ds6_neighbors);
      nbr != NULL;
      nbr = nbr_table_next(ds6_neighbors, nbr)) {

    if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) {
      add("[<a href=\"nbr-rm?");
      ipaddr_add(&nbr->ipaddr);
      add("\">del</a>] ");
    }
#if CETIC_NODE_CONFIG
    if ( node_config_loaded ) {
      add("%s : ", node_config_get_name(node_config_find(uip_ds6_nbr_get_ll(nbr))));
    }
#endif
    ipaddr_add(&nbr->ipaddr);
    add(" ");
    lladdr_add(uip_ds6_nbr_get_ll(nbr));
    add(" ");
    add_network_cases(nbr->state);
    add("\n");
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }

  add("</pre><h2>Routes</h2><pre>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  for(r = uip_ds6_route_head(), i = 0; r != NULL;
      r = uip_ds6_route_next(r), ++i) {
    if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) {
      add("[<a href=\"route-rm?");
      ipaddr_add(&r->ipaddr);
      add("\">del</a>] ");
    }
#if CETIC_NODE_CONFIG
    if ( node_config_loaded ) {
      add("%s (", node_config_get_name(node_config_find_from_ip(&r->ipaddr)));
      ipaddr_add(&r->ipaddr);
      add("/%u) via ", r->length);
    } else {
      ipaddr_add(&r->ipaddr);
      add("/%u via ", r->length);
    }
    if ( node_config_loaded ) {
      add("%s (", node_config_get_name(node_config_find_from_ip(uip_ds6_route_nexthop(r))));
      ipaddr_add(uip_ds6_route_nexthop(r));
      add(")");
    } else {
      ipaddr_add(uip_ds6_route_nexthop(r));
    }
#else
    ipaddr_add(&r->ipaddr);
    add("/%u via ", r->length);
    ipaddr_add(uip_ds6_route_nexthop(r));
#endif
    if(1 || (r->state.lifetime < 600)) {
      add(" %lu s\n", r->state.lifetime);
    } else {
      add("\n");
    }
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }

  add("</pre><h2>Default Routers</h2><pre>");

  for(dr = uip_ds6_defrt_list_head(); dr != NULL; dr = list_item_next(r)) {
    ipaddr_add(&dr->ipaddr);
    if(!dr->isinfinite) {
      add(" %u s", stimer_remaining(&dr->lifetime));
    }
    add("\n");
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }

#if UIP_CONF_DS6_ROUTE_INFORMATION
  add("</pre><h2>Route info</h2><pre>");
  for(i = 0; i < UIP_DS6_ROUTE_INFO_NB; i++) {
    if(uip_ds6_route_info_list[i].isused) {
      ipaddr_add(&uip_ds6_route_info_list[i].ipaddr);
      add("/%u (%x) %us\n", uip_ds6_route_info_list[i].length,
          uip_ds6_route_info_list[i].flags,
          uip_ds6_route_info_list[i].lifetime);
    }
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

#if CETIC_6LBR_IP64
  if((nvm_data.global_flags & CETIC_GLOBAL_IP64) != 0) {
    add("</pre><h2>IP64 connections mapping</h2><pre>");
    struct ip64_addrmap_entry *m = NULL;
    for(m = ip64_addrmap_list();
        m != NULL;
        m = list_item_next(m)) {
      if(timer_expired(&m->timer)) continue;
      ipaddr_add(&m->ip6addr);
      add("%%%d (%d) -> ", m->ip6port, m->protocol);
      ip4addr_add(&m->ip4addr);
      add("%%%d : %d (%x) %us\n", m->ip4port, m->mapped_port,
          m->flags, m->timer.interval - (clock_time() - m->timer.start));
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }
#endif

#if SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS > 0
  add("<pre><h2>6LoWPAN Prefix contexts</h2><pre>");
  for(i = 0; i < SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS; i++) {
    if(addr_contexts[i].used == 1) {
      add("%d : %02x%02x:%02x%02x:%02x%02x:%02x%02x\n", addr_contexts[i].number,
          addr_contexts[i].prefix[1], addr_contexts[i].prefix[0],
          addr_contexts[i].prefix[3], addr_contexts[i].prefix[2],
          addr_contexts[i].prefix[5], addr_contexts[i].prefix[4],
          addr_contexts[i].prefix[7], addr_contexts[i].prefix[6]);
    }
  }
  add("</pre><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

#if CETIC_6LBR_TRANSPARENTBRIDGE
  add("<h2>HW Prefixes cache</h2><pre>");
  for(i = 0; i < prefixCounter; i++) {
    add("%02x:%02x:%02x\n", prefixBuffer[i][0], prefixBuffer[i][1],
        prefixBuffer[i][2]);
  }
  SEND_STRING(&s->sout, buf);
  add("</pre><br />");
  reset_buf();
#endif
  PSOCK_END(&s->sout);
}
static void
add_network_cases(const uint8_t state)
{
  switch (state) {
  case NBR_INCOMPLETE:
    add("INCOMPLETE");
    break;
  case NBR_REACHABLE:
    add("REACHABLE");
    break;
  case NBR_STALE:
    add("STALE");
    break;
  case NBR_DELAY:
    add("DELAY");
    break;
  case NBR_PROBE:
    add("NBR_PROBE");
    break;
  }
}

static httpd_cgi_call_t *
webserver_network_route_rm(struct httpd_state *s)
{
  static uip_ds6_route_t *route;
  static uip_ipaddr_t ipaddr;
  webserver_result_title = "Delete route";
  webserver_result_text = "Route not found";
  if(s->query && uiplib_ipaddrconv(s->query, &ipaddr) != 0) {
    route = uip_ds6_route_lookup(&ipaddr);
    if(route) {
      uip_ds6_route_rm(route);
      webserver_result_text = "Route deleted";
    }
  }
  return &webserver_result_page;
}

static httpd_cgi_call_t *
webserver_network_nbr_rm(struct httpd_state *s)
{
  static uip_ds6_nbr_t *neighbor;
  static uip_ipaddr_t ipaddr;
  webserver_result_title = "Delete neighbor";
  webserver_result_text = "Neighbor not found";
  if(s->query && uiplib_ipaddrconv(s->query, &ipaddr) != 0) {
    neighbor = uip_ds6_nbr_lookup(&ipaddr);
    if (neighbor) {
      uip_ds6_nbr_rm(neighbor);
      webserver_result_text = "Neighbor deleted";
    }
  }
  return &webserver_result_page;
}

HTTPD_CGI_CALL(webserver_network, "network.html", "Network", generate_network, 0);
HTTPD_CGI_CMD(webserver_network_route_rm_cmd, "route-rm", webserver_network_route_rm, 0);
HTTPD_CGI_CMD(webserver_network_nbr_rm_cmd, "nbr-rm", webserver_network_nbr_rm, 0);
