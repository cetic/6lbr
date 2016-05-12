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
 *         6LBR Web Server (Light version well-suited for Econotag)
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-route.h"
#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/netstack.h"
#include "net/rpl/rpl.h"

#include "sicslow-ethernet.h"
#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "rio.h"
#if CETIC_NODE_INFO
#include "node-info.h"
#endif

#if CONTIKI_TARGET_ECONOTAG
#include "mc1322x.h"
#include "contiki-maca.h"
#endif
#if CONTIKI_TARGET_CC2538DK
#include "reg.h"
#include "dev/cc2538-rf.h"
#endif

#include <stdio.h>              /* For printf() */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
extern uip_ds6_prefix_t uip_ds6_prefix_list[];

extern rpl_instance_t instance_table[RPL_MAX_INSTANCES];

int count = 0;

/*---------------------------------------------------------------------------*/
/* Use simple webserver with only one page for minimum footprint.
 * Multiple connections can result in interleaved tcp segments since
 * a single static buffer is used for all segments.
 */
#include "httpd-simple.h"
/* The internal webserver can provide additional information if
 * enough program flash is available.
 */
#ifndef WEBSERVER_CONF_LOADTIME
#define WEBSERVER_CONF_LOADTIME 0
#endif
#ifndef WEBSERVER_CONF_FILESTATS
#define WEBSERVER_CONF_FILESTATS 0
#endif

#ifndef WEBSERVER_CONF_INFO
#define WEBSERVER_CONF_INFO 1
#endif
#ifndef WEBSERVER_CONF_SENSORS
#define WEBSERVER_CONF_SENSORS 0
#endif
#ifndef WEBSERVER_CONF_RPL
#define WEBSERVER_CONF_RPL 0
#endif
#ifndef WEBSERVER_CONF_NETWORK
#define WEBSERVER_CONF_NETWORK 1
#endif
#ifndef WEBSERVER_CONF_CONFIG
#define WEBSERVER_CONF_CONFIG 1
#endif

#define BUF_USES_STACK 0
/*---------------------------------------------------------------------------*/
PROCESS(httpd_process, "Web server");
PROCESS_THREAD(httpd_process, ev, data)
{
  PROCESS_BEGIN();

  httpd_init();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
webserver_init(void)
{
  process_start(&httpd_process, NULL);
}
/*---------------------------------------------------------------------------*/
#define BUF_SIZE 256
static const char *TOP =
  "<html><head><title>6LBR lite</title><style type=\"text/css\">"
  "body{font-family:Verdana;color:#333333;padding:20px;}"
  "h1,h2{margin:40px 0 0;padding:0;font-weight:bold;}"
  "h1{font-size:16px;line-height:18px;}"
  "h2{font-size:14px;color:#669934;line-height:16px;}"
  "h3{font-size:12px;font-weight:bold;line-height:14px;}"
  "#h{margin:0;}"
  "</style></head>";
static const char *BODY = "<body>";
static const char *BOTTOM =
  "<hr/>"
  "<small>6LBR By CETIC (<a href=\"http://cetic.github.com/6lbr\">doc</a>)</small>"
  "</body></html>";

#if BUF_USES_STACK
static char *bufptr, *bufend;
#else
static char buf[BUF_SIZE];
static int blen;
#endif
/*---------------------------------------------------------------------------*/
/*OPTIMIZATIONS to gain space : prototypes*/
static void add(char *str, ...);
void add_network_cases(const uint8_t state);
static void reset_buf();

/*End optimizations*/
/*---------------------------------------------------------------------------*/
static void
ipaddr_add(const uip_ipaddr_t * addr)
{
  uint16_t a;
  int i, f;

  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0)
        add("::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        add(":");
      }
      add("%x", a);
    }
  }
}
static void
lladdr_add(const uip_lladdr_t * addr)
{
  int i;

  for(i = 0; i < sizeof(uip_lladdr_t); i++) {
    if(i > 0) {
      add(":");
    }
    add("%x", addr->addr[i]);
  }
}
static void
ethaddr_add(ethaddr_t * addr)
{
  int i;

  for(i = 0; i < 6; i++) {
    if(i > 0) {
      add(":");
    }

    add("%x", (*addr)[i]);
  }
}
/*---------------------------------------------------------------------------*/

#if CONTIKI_TARGET_ECONOTAG
extern void _start;

//Code
extern void _etext;

//RO data
extern void __data_start;

//Initialised data
extern void _edata;

//Stack
extern void __bss_start;

//Zero initialised data
extern void _bss_end__;

//Heap
extern void _end;
#endif

/*---------------------------------------------------------------------------*/
static
PT_THREAD(generate_index(struct httpd_state *s))
{
  static int i;
  //static int j;
  static uip_ds6_nbr_t *nbr;
  static uip_ds6_route_t *r;
  static uip_ds6_defrt_t *dr;

#if BUF_USES_STACK
  char buf[BUF_SIZE];
#endif
#if WEBSERVER_CONF_LOADTIME
  static clock_time_t numticks;

  numticks = clock_time();
#endif

  PSOCK_BEGIN(&s->sout);

  SEND_STRING(&s->sout, TOP);
  SEND_STRING(&s->sout, BODY);
  reset_buf();
  add("<h1 id=\"h\">6LBR <i>lite</i></h1><h2 id=\"h\">Border Router Status</h2><hr>");
#if WEBSERVER_CONF_INFO
  add("<h1>Info</h1>");
  add("<h2>6LBR</h2>");
  add("Version : " CETIC_6LBR_VERSION " (" CONTIKI_VERSION_STRING ")<br>");
  add("Mode : ");
#if CETIC_6LBR_SMARTBRIDGE
  add("SMART BRIGDE");
#endif
#if CETIC_6LBR_TRANSPARENTBRIDGE
#if CETIC_6LBR_LEARN_RPL_MAC
  add("RPL Relay");
#else
  add("FULL TRANSPARENT BRIGDE");
#endif
#endif
#if CETIC_6LBR_ROUTER
#if UIP_CONF_IPV6_RPL
  add("RPL ROUTER");
#else
  add("NDP ROUTER");
#endif
#endif
#if CETIC_6LBR_6LR
  add("6LR");
#endif
  add("<br>");
  i = clock_seconds() - cetic_6lbr_startup;
  add("Uptime : %dh %dm %ds<br>", i / 3600, (i / 60) % 60, i % 60);
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<h2>WSN</h2>");
  add("MAC: %s<br>RDC: %s (%d Hz)<br>",
      NETSTACK_MAC.name,
      NETSTACK_RDC.name,
      (NETSTACK_RDC.channel_check_interval() ==
       0) ? 0 : CLOCK_SECOND / NETSTACK_RDC.channel_check_interval());
#if UIP_CONF_IPV6_RPL
  add("Prefix : ");
  ipaddr_add(&cetic_dag->prefix_info.prefix);
  add("/%d", cetic_dag->prefix_info.length);
  add("<br>");
#endif
  add("HW address : ");
  lladdr_add(&uip_lladdr);
  add("<br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<h2>Ethernet</h2>");
#if CETIC_6LBR_ROUTER
  add("Address : ");
  ipaddr_add(&eth_ip_addr);
  add("<br>");
  add("Local address : ");
  ipaddr_add(&eth_ip_local_addr);
  add("<br>");
#endif
  add("HW address : ");
  ethaddr_add(&eth_mac_addr);
  add("<br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

#if CONTIKI_TARGET_ECONOTAG
  add("<h2>Memory</h2>");

  add("Global : %d (%d %%)<br><br>", &_end - &_start,
      (100 * (&_end - &_start)) / (96 * 1024));

  add("Code : %d<br>", &_etext - &_start);
  add("Initialised data : %d<br><br>", &_edata - &_etext);
  add("Data : %d<br>", &_bss_end__ - &__bss_start);
  add("Stack : %d<br>", &__bss_start - &_edata);
  add("Heap : %d<br>", &_end - &_bss_end__);
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#endif

#if WEBSERVER_CONF_SENSORS
  add("<h1>Sensors</h1>");
  add
    ("<table>"
     "<theader><tr class=\"row_first\"><td>Node</td><td>Type</td><td>Web</td><td>Coap</td><td>Sequence</td><td>Parent</td><td>Last seen</td></tr></theader>"
     "<tbody>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(node_info_table[i].isused) {
      add("<tr><td>");
#if CETIC_NODE_CONFIG
      if ( node_config_loaded ) {
        add("%s (", node_config_get_name(node_config_find_by_ip(&node_info_table[i].ipaddr)));
        ipaddr_add(&node_info_table[i].ipaddr);
        add(")</a></td>");
      } else {
        ipaddr_add(&node_info_table[i].ipaddr);
        add("</a></td>");
      }
#else
      ipaddr_add(&node_info_table[i].ipaddr);
      add("</a></td>");
#endif

      if(0) {
      } else if(node_info_table[i].ipaddr.u8[8] == 0x02
         && node_info_table[i].ipaddr.u8[9] == 0x12
         && (node_info_table[i].ipaddr.u8[10] == 0x74 ||
             node_info_table[i].ipaddr.u8[10] == 0x75)) {
        add("<td>Moteiv Telos</td>");
      } else if(node_info_table[i].ipaddr.u8[8] == 0x02
         && node_info_table[i].ipaddr.u8[9] == 0x1A
         && node_info_table[i].ipaddr.u8[10] == 0x4C) {
        add("<td>Crossbow Sky</td>");
      } else if(node_info_table[i].ipaddr.u8[8] == 0xC3
         && node_info_table[i].ipaddr.u8[9] == 0x0C
         && node_info_table[i].ipaddr.u8[10] == 0x00) {
        add("<td>Zolertia Z1</td>");
      } else if(node_info_table[i].ipaddr.u8[8] == 0x02
         && node_info_table[i].ipaddr.u8[9] == 0x80
         && node_info_table[i].ipaddr.u8[10] == 0xE1) {
        add("<td>STMicro</td>");
      } else if(node_info_table[i].ipaddr.u8[8] == 0x02
         && node_info_table[i].ipaddr.u8[9] == 0x12
         && node_info_table[i].ipaddr.u8[10] == 0x4B) {
        add("<td>TI</td>");
      } else if(node_info_table[i].ipaddr.u8[8] == 0x02
                && node_info_table[i].ipaddr.u8[9] == 0x50
                && node_info_table[i].ipaddr.u8[10] == 0xC2
                && node_info_table[i].ipaddr.u8[11] == 0xA8
                && (node_info_table[i].ipaddr.u8[12] & 0XF0) == 0xC0) {
        add("<td>Redwire Econotag I</td>");
      } else if(node_info_table[i].ipaddr.u8[8] == 0x02
                && node_info_table[i].ipaddr.u8[9] == 0x05
                && node_info_table[i].ipaddr.u8[10] == 0x0C
                && node_info_table[i].ipaddr.u8[11] == 0x2A
                && node_info_table[i].ipaddr.u8[12] == 0x8C) {
        add("<td>Redwire Econotag I</td>");
      } else if(node_info_table[i].ipaddr.u8[8] == 0xEE
                && node_info_table[i].ipaddr.u8[9] == 0x47
                && node_info_table[i].ipaddr.u8[10] == 0x3C) {
        if(node_info_table[i].ipaddr.u8[11] == 0x4D
           && node_info_table[i].ipaddr.u8[12] == 0x12) {
          add("<td>Redwire M12</td>");
        } else {
          add("<td>Redwire Unknown</td>");
        }
      } else if((node_info_table[i].ipaddr.u8[8] & 0x02) == 0) {
        add("<td>User defined</td>");
      } else {
        add("<td>Unknown</td>");
      }
      SEND_STRING(&s->sout, buf);
      reset_buf();
      add("<td><a href=http://[");
      ipaddr_add(&node_info_table[i].ipaddr);
      add("]/>web</a></td>");
      add("<td><a href=coap://[");
      ipaddr_add(&node_info_table[i].ipaddr);
      add("]:5683/>coap</a></td>");
      if(node_info_table[i].messages_count > 0) {
        add("<td>%d</td><td>", node_info_table[i].sequence);
#if CETIC_NODE_CONFIG
        if (node_config_loaded) {
          add("%s (", node_config_get_name(node_config_find_by_ip(&node_info_table[i].ip_parent)));
          ipaddr_add(&node_info_table[i].ip_parent);
          add(")");
        } else {
          ipaddr_add(&node_info_table[i].ip_parent);
        }
#else
        ipaddr_add(&node_info_table[i].ip_parent);
#endif
        add("</td>");
      } else {
        add("<td></td><td></td>");
      }
      add("<td>%d</td>",
          (clock_time() - node_info_table[i].last_seen) / CLOCK_SECOND);
      add("</tr>");
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }
  add("</tbody></table><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add
    ("<center>"
     "<img src=\"http://chart.googleapis.com/chart?cht=gv&chls=1&chl=digraph{");
#if CETIC_NODE_CONFIG
  node_config_t *  my_config = node_config_find_by_lladdr(&uip_lladdr);
  if (my_config) {
    add("%s;", node_config_get_name(my_config));
  } else {
   add("_%04x;",
     (uip_lladdr.addr[6] << 8) + uip_lladdr.addr[7]);
  }
#else
  add("_%04x;",
    (uip_lladdr.addr[6] << 8) + uip_lladdr.addr[7]);
#endif
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(node_info_table[i].isused) {
      if(! uip_is_addr_unspecified(&node_info_table[i].ip_parent)) {
#if CETIC_NODE_CONFIG
        node_config_t * node_config = node_config_find_by_ip(&node_info_table[i].ipaddr);
        node_config_t * parent_node_config = node_config_find_by_ip(&node_info_table[i].ip_parent);
        if ( node_config ) {
          if ( parent_node_config ) {
            add("%s->%s;",
                node_config_get_name(node_config),
                node_config_get_name(parent_node_config));
          } else {
            add("%s->_%04hx;",
                node_config_get_name(node_config),
                (node_info_table[i].ip_parent.u8[14] << 8) +
                node_info_table[i].ip_parent.u8[15]);
          }
        } else {
          if (parent_node_config) {
            add("_%04hx->%s;",
                (node_info_table[i].ipaddr.u8[14] << 8) +
                node_info_table[i].ipaddr.u8[15],
                node_config_get_name(parent_node_config));
          } else {
            add("_%04hx->_%04hx;",
                (node_info_table[i].ipaddr.u8[14] << 8) +
                node_info_table[i].ipaddr.u8[15],
                (node_info_table[i].ip_parent.u8[14] << 8) +
                node_info_table[i].ip_parent.u8[15]);
          }
        }
#else
        add("_%04hx->_%04hx;",
            (node_info_table[i].ipaddr.u8[14] << 8) +
            node_info_table[i].ipaddr.u8[15],
            (node_info_table[i].ip_parent.u8[14] << 8) +
            node_info_table[i].ip_parent.u8[15]);
#endif
      }
    }
  }
  add("}\"alt=\"\" /></center>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

#if WEBSERVER_CONF_RPL
  add("<h1>RPL</h1>");
  add("<h2>Configuration</h2>");
  add("Lifetime : %d (%d x %d s)<br>",
      RPL_CONF_DEFAULT_LIFETIME * RPL_CONF_DEFAULT_LIFETIME_UNIT,
      RPL_CONF_DEFAULT_LIFETIME, RPL_CONF_DEFAULT_LIFETIME_UNIT);
  add("<br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  for(i = 0; i < RPL_MAX_INSTANCES; ++i) {
    if(instance_table[i].used) {
      add("<h2>Instance %d</h2>", instance_table[i].instance_id);
      for(j = 0; j < RPL_MAX_DAG_PER_INSTANCE; ++j) {
        if(instance_table[i].dag_table[j].used) {
          add("<h3>DODAG %d</h3>", j);
          add("DODAG ID : ");
          ipaddr_add(&instance_table[i].dag_table[j].dag_id);
          add("<br>Version : %d", instance_table[i].dag_table[j].version);
          add("<br>Grounded : %s",
              instance_table[i].dag_table[j].grounded ? "Yes" : "No");
          add("<br>Preference : %d",
              instance_table[i].dag_table[j].preference);
          add("<br>Mode of Operation : %u", instance_table[i].mop);
          add("<br>Current DIO Interval [%u-%u] : %u",
              instance_table[i].dio_intmin,
              instance_table[i].dio_intmin + instance_table[i].dio_intdoubl,
              instance_table[i].dio_intcurrent);
          add("<br>Objective Function Code Point : %u",
              instance_table[i].of->ocp);
          add("<br>Joined : %s",
              instance_table[i].dag_table[j].joined ? "Yes" : "No");
          add("<br>Rank : %d", instance_table[i].dag_table[j].rank);
          add("<br>");
          SEND_STRING(&s->sout, buf);
          reset_buf();
        }
      }
    }
  }
  add("<h3>Actions</h3>");
  add("<a href=\"rpl-gr\">Trigger global repair</a><br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

#if WEBSERVER_CONF_NETWORK
  add("<h1>Network</h1>");
  add("<h2>Addresses</h2><pre>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    if(uip_ds6_if.addr_list[i].isused) {
      ipaddr_add(&uip_ds6_if.addr_list[i].ipaddr);
      char flag;

      if(uip_ds6_if.addr_list[i].state == ADDR_TENTATIVE)
        flag = 'T';
      else if(uip_ds6_if.addr_list[i].state == ADDR_PREFERRED)
        flag = 'P';
      else
        flag = '?';
      add(" %c", flag);
      if(uip_ds6_if.addr_list[i].type == ADDR_MANUAL)
        flag = 'M';
      else if(uip_ds6_if.addr_list[i].type == ADDR_DHCP)
        flag = 'D';
      else if(uip_ds6_if.addr_list[i].type == ADDR_AUTOCONF)
        flag = 'A';
      else
        flag = '?';
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
      if(uip_ds6_prefix_list[i].advertise)
        add("*");
#else
      if(uip_ds6_prefix_list[i].isinfinite)
        add("I");
#endif
      add("\n");
    }
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("</pre><h2>Neighbors</h2><pre>");
  for(nbr = nbr_table_head(ds6_neighbors);
      nbr != NULL;
      nbr = nbr_table_next(ds6_neighbors, nbr)) {
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
    ipaddr_add(&r->ipaddr);
    add("/%u via ", r->length);
    ipaddr_add(uip_ds6_route_nexthop(r));
    if(1 || (r->state.lifetime < 600)) {
      add("%lu s\n", r->state.lifetime);
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

#if CETIC_6LBR_TRANSPARENTBRIDGE
  add("</pre><h2>HW Prefixes cache</h2><pre>");
  for(i = 0; i < prefixCounter; i++) {
    add("%02x:%02x:%02x\n", prefixBuffer[i][0], prefixBuffer[i][1],
        prefixBuffer[i][2]);
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
  add("</pre><br>");
#endif

#if WEBSERVER_CONF_CONFIG
  add("<h1>Config</h1>");
  add("<h2>WSN Network</h2>");
  add("<h3>WSN configuration</h3>");
  add("Channel : %d<br>", nvm_data.channel);
#if CONTIKI_TARGET_ECONOTAG
  add("PanID : 0x%x<br>", *MACA_MACPANID);
#endif
#if CONTIKI_TARGET_CC2538DK
  add("PanID : 0x%x<br>", (REG(RFCORE_FFSM_PAN_ID1)<<8) + REG(RFCORE_FFSM_PAN_ID0));
#endif
  add("<br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<h3>IP configuration</h3>");
#if CETIC_6LBR_SMARTBRIDGE || CETIC_6LBR_TRANSPARENTBRIDGE
  add("Network configuration : ");
  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) != 0) {
    add(" autoconfiguration<br>");
  } else {
    add(" static<br>");
    add("Prefix : ");
    ipaddr_add(&wsn_net_prefix);
    add("<br>");
  }
#elif CETIC_6LBR_ROUTER
  add("Prefix : ");
  ipaddr_add(&wsn_net_prefix);
  add("<br>");
#endif
  add("Address : ");
  if((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0) {
    add("autoconfiguration");
  } else {
    ipaddr_add(&wsn_ip_addr);
  }
  add("<br>");
#if CETIC_6LBR_SMARTBRIDGE || CETIC_6LBR_TRANSPARENTBRIDGE
  add("Default router : ");
  ipaddr_add(&eth_dft_router);
  add("<br>");
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();

#if CETIC_6LBR_ROUTER
  add("<h2>Eth Network</h2>");
  add("<h3>IP configuration</h3>");
  add("Prefix : ");
  ipaddr_add(&eth_net_prefix);
  add("<br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("Address : ");
  ipaddr_add(&eth_ip_addr);
  add("<br>");
  add("Peer router : ");
  ipaddr_add(&eth_dft_router);
  add("<br>");
  add("RA Daemon : ");
  if((nvm_data.mode & CETIC_MODE_ROUTER_RA_DAEMON) != 0) {
    add("active (Lifetime : %d)", UIP_CONF_ROUTER_LIFETIME);
  } else {
    add("inactive");
  }
  add("<br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#if CETIC_6LBR_ROUTER
  add("<h3>Packet filtering</h3>");
  add("Address rewrite : ");
  if((nvm_data.mode & CETIC_MODE_REWRITE_ADDR_MASK) != 0) {
    add("enabled<br>");
  } else {
    add("disabled<br>");
  }
#endif
  add("<br>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

#if WEBSERVER_CONF_FILESTATS
  static uint16_t numtimes;

  add("<br><i>This page sent %u times</i>", ++numtimes);
#endif

#if WEBSERVER_CONF_LOADTIME
  numticks = clock_time() - numticks + 1;
  add(" <i>(%u.%02u sec)</i>", numticks / CLOCK_SECOND,
      (100 * (numticks % CLOCK_SECOND) / CLOCK_SECOND));
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

  SEND_STRING(&s->sout, BOTTOM);
  PSOCK_END(&s->sout);
}

/*---------------------------------------------------------------------------*/
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{
  //static uip_ds6_route_t *r;
  //static int i;

  if(strcmp(name, "index.html") == 0 || strcmp(name, "") == 0) {
    return generate_index;
  } else {
    return NULL;
  }
}
/*---------------------------------------------------------------------------*/
void
add_network_cases(const uint8_t state)
{
  switch (state) {
  case NBR_INCOMPLETE:
    add("I");
    break;
  case NBR_REACHABLE:
    add("R");
    break;
  case NBR_STALE:
    add("S");
    break;
  case NBR_DELAY:
    add("D");
    break;
  case NBR_PROBE:
    add("P");
    break;
  }
}
/*---------------------------------------------------------------------------*/
/*Macro redefined : RESET_BUF()*/
/*---------------------------------------------------------------------------*/
static void
reset_buf()
{
#if BUF_USES_STACK
  bufptr = buf;
  bufend = bufptr + sizeof(buf);
#else
  blen = 0;
#endif
}
/*---------------------------------------------------------------------------*/
/*Macro redefined : ADD()*/
/*---------------------------------------------------------------------------*/
static void
add(char *str, ...)
{
  va_list arg;

  va_start(arg, str);
  //ADD(str, arg); //TODO : bug while formating
#if BUF_USES_STACK
  bufptr += vsnprintf(bufptr, bufend - bufptr, str, arg);
#else
  blen += vsnprintf(&buf[blen], sizeof(buf) - blen, str, arg);
#endif
  va_end(arg);
}
/*---------------------------------------------------------------------------*/
