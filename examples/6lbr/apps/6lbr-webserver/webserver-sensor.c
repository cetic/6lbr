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
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#include "cetic-6lbr.h"
#include "node-info.h"
#include "log-6lbr.h"

#if CETIC_NODE_CONFIG
#include "node-config.h"
#endif

static
PT_THREAD(generate_sensor(struct httpd_state *s))
{
  static uip_ipaddr_t ipaddr;
  node_info_t * node_info;

  PSOCK_BEGIN(&s->sout);
  if(s->query && uiplib_ipaddrconv(s->query, &ipaddr) != 0) {
    node_info = node_info_lookup(&ipaddr);
    if(node_info) {
      add("<h2>Info</h2>");
#if CETIC_NODE_CONFIG
      if ( node_config_loaded ) {
        add("Name: %s<br />", node_config_get_name(node_config_find_from_ip(&ipaddr)));
      }
#endif
      add("IP: ");
      ipaddr_add(&ipaddr);
      add("<br />");
      add("Model: -<br />");
      add("Parent: ");
#if CETIC_NODE_CONFIG
      if (node_config_loaded) {
        add("%s (", node_config_get_name(node_config_find_from_ip(&node_info->ip_parent)));
        ipaddr_add(&node_info->ip_parent);
        add(")");
      } else {
        ipaddr_add(&node_info->ip_parent);
      }
#else
      ipaddr_add(&node_info_table[i].ip_parent);
#endif
      add("<br />");
      add("Downward route: %s<br />", node_info->has_route ? "Yes" : "No");
      SEND_STRING(&s->sout, buf);
      reset_buf();

      add("<h2>Statistics</h2>");
      add("Messages sent: %d<br />", node_info->messages_sent);
      add("Messages lost: %d<br />", node_info->up_messages_lost);
      add("Upstream PRR: %.1f%%<br />", 100.0 * (node_info->messages_sent - node_info->up_messages_lost)/node_info->messages_sent);
      add("Parent switch: %d<br />", node_info->parent_switch);
      add("Last seen : %d<br />",
        (clock_time() - node_info->last_seen) / CLOCK_SECOND);
    } else {
      add("Sensor address unknown");
    }
  } else {
    add("Sensor address missing");
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PSOCK_END(&s->sout);
}

HTTPD_CGI_CALL(webserver_sensor, "sensor", "Sensor", generate_sensor, WEBSERVER_NOMENU);
