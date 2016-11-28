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
  static node_info_t * node_info = NULL;

  PSOCK_BEGIN(&s->sout);
  if(s->query && strncmp(s->query, "ip=", 3) == 0 && uiplib_ipaddrconv(s->query + 3, &ipaddr) != 0) {
    node_info = node_info_lookup(&ipaddr);
    if(node_info) {
      add("<h2>Info</h2>");
#if CETIC_NODE_CONFIG_HAS_NAME
      if ( node_config_loaded ) {
        add("Name: %s<br />", node_config_get_name(node_config_find_by_ip(&ipaddr)));
      }
#endif
      add("IP: ");
      ipaddr_add(&ipaddr);
      add("<br />");
      add("Model: -<br />");
      add("Parent: ");
#if CETIC_NODE_CONFIG_HAS_NAME
      if (node_config_loaded) {
        add("%s (", node_config_get_name(node_config_find_by_ip(&node_info->ip_parent)));
        ipaddr_add(&node_info->ip_parent);
        add(")</a>");
      } else
#endif
      {
        ipaddr_add(&node_info->ip_parent);
        add("</a>");
      }
      add("<br />");
      add("Downward route: %s<br />", node_info_flags_text(node_info->flags & NODE_INFO_HAS_ROUTE) != 0 ? "Yes" : "No");
      SEND_STRING(&s->sout, buf);
      reset_buf();

      add("<br /><h2>Statistics</h2>");
      add("Since : %d s<br />",
          (clock_time() - node_info->stats_start) / CLOCK_SECOND);
      add("Last seen : %d s<br />",
        (clock_time() - node_info->last_seen) / CLOCK_SECOND);
      add("Hop count: %d<br />", node_info->hop_count);
      add("Parent switch: %d<br />", node_info->parent_switch);
      add("<br /><h3>Upstream</h3>");
      add("Last sequence number: %d<br />", node_info->last_up_sequence);
      add("Messages sent: %d<br />", node_info->messages_sent);
      add("Messages lost: %d<br />", node_info->up_messages_lost);
      if(node_info->messages_sent > 0) {
        add("PRR: %.1f%%<br />", 100.0 * (node_info->messages_sent - node_info->up_messages_lost)/node_info->messages_sent);
      } else {
        add("PRR: n/a<br />");
      }
      add("<br /><h3>Downstream</h3>");
      add("Last sequence number: %d<br />", node_info->last_down_sequence);
      add("Messages sent: %d<br />", node_info->replies_sent);
      add("Messages lost: %d<br />", node_info->down_messages_lost);
      if(node_info->replies_sent > 0) {
        add("PRR: %.1f%%<br />", 100.0 * (node_info->replies_sent - node_info->down_messages_lost)/node_info->replies_sent);
      } else {
        add("PRR: n/a<br />");
      }
      SEND_STRING(&s->sout, buf);
      reset_buf();
      add("<br /><h2>Actions</h2>");
      add("<form action=\"reset-stats\" method=\"get\">");
      add("<input type=\"hidden\" name=\"ip\" value=\"");
      ipaddr_add(&node_info->ipaddr);
      add("\"/>");
      add("<input type=\"submit\" value=\"Reset statistics\"/></form><br />");
      add("<form action=\"rm-node\" method=\"get\">");
      add("<input type=\"hidden\" name=\"ip\" value=\"");
      ipaddr_add(&node_info->ipaddr);
      add("\"/>");
      add("<input type=\"submit\" value=\"Delete node\"/></form><br />");
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

static httpd_cgi_call_t *
webserver_sensor_reset_stats(struct httpd_state *s)
{
  static uip_ipaddr_t ipaddr;
  static node_info_t * node_info = NULL;
  webserver_result_title = "Sensor";
  if(s->query && strncmp(s->query, "ip=", 3) == 0 && uiplib_ipaddrconv(s->query + 3, &ipaddr) != 0) {
    node_info = node_info_lookup(&ipaddr);
    if(node_info) {
      node_info_reset_statistics(node_info);
      webserver_result_text = "Sensor statistics reset";
    } else {
      webserver_result_text = "Sensor address unknown";
    }
  } else {
    webserver_result_text = "Sensor address missing";
  }
  return &webserver_result_page;
}

static httpd_cgi_call_t *
webserver_sensor_delete_node(struct httpd_state *s)
{
  static uip_ipaddr_t ipaddr;
  static node_info_t * node_info = NULL;
  webserver_result_title = "Sensor";
  if(s->query && strncmp(s->query, "ip=", 3) == 0 && uiplib_ipaddrconv(s->query + 3, &ipaddr) != 0) {
    node_info = node_info_lookup(&ipaddr);
    if(node_info) {
      node_info_rm(node_info);
      webserver_result_text = "Sensor deleted";
    } else {
      webserver_result_text = "Sensor address unknown";
    }
  } else {
    webserver_result_text = "Sensor address missing";
  }
  return &webserver_result_page;
}

HTTPD_CGI_CALL(webserver_sensor, "sensor", "Sensor", generate_sensor, WEBSERVER_NOMENU);
HTTPD_CGI_CMD(webserver_sensor_reset_stats_cmd, "reset-stats", webserver_sensor_reset_stats, 0);
HTTPD_CGI_CMD(webserver_sensor_delete_node_cmd, "rm-node", webserver_sensor_delete_node, 0);
