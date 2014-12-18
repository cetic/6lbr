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

static const char * graph_top =
    "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
    "<script type=\"text/javascript\">"
    "google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});"
    "google.setOnLoadCallback(drawChart);"
    "function drawChart(){var data=google.visualization.arrayToDataTable([";

static const char * graph_bottom =
    "var view = new google.visualization.DataView(data);"
    "view.setColumns([0, 2]);"
    "var chart=new google.visualization.ColumnChart(document.getElementById('chart_div'));"
    "chart.draw(view, options);"
    "var selectHandler = function(e) {"
    "window.location = 'sensor?ip=' + data.getValue(chart.getSelection()[0]['row'], 1 );"
    "};"
    "google.visualization.events.addListener(chart, 'select', selectHandler);"
    "}"
    "</script>"
    "<div id=\"chart_div\" style=\"width: 900px; height: 500px;\"></div>";

static
PT_THREAD(generate_sensors(struct httpd_state *s))
{
  static int i;

  PSOCK_BEGIN(&s->sout);
  add("<br /><h2>Sensors list</h2>");
  add
    ("<table>"
     "<theader><tr class=\"row_first\"><td>Node</td><td>Type</td><td>Web</td><td>Coap</td><td>Parent</td><td>Up PRR</td><td>Last seen</td><td>Status</td></tr></theader>"
     "<tbody>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(node_info_table[i].isused) {
      add("<tr><td>");
      add("<a href=\"sensor?");
      ipaddr_add(&node_info_table[i].ipaddr);
#if CETIC_NODE_CONFIG
      if ( node_config_loaded ) {
        add("\">%s (", node_config_get_name(node_config_find_from_ip(&node_info_table[i].ipaddr)));
        ipaddr_add(&node_info_table[i].ipaddr);
        add(")</a></td>");
      } else
#endif
      {
        add("\">");
        ipaddr_add(&node_info_table[i].ipaddr);
        add("</a></td>");
      }

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
      if(node_info_table[i].messages_received > 0) {
        add("<td>");
#if CETIC_NODE_CONFIG
        if (node_config_loaded) {
          add("%s (", node_config_get_name(node_config_find_from_ip(&node_info_table[i].ip_parent)));
          ipaddr_add(&node_info_table[i].ip_parent);
          add(")");
        } else {
          ipaddr_add(&node_info_table[i].ip_parent);
        }
#else
        ipaddr_add(&node_info_table[i].ip_parent);
#endif
        add("</td>");
        add("<td>%.1f%%</td>", 100.0 * (node_info_table[i].messages_sent - node_info_table[i].up_messages_lost)/node_info_table[i].messages_sent);
      } else {
        add("<td></td><td></td>");
      }
      add("<td>%d</td>",
          (clock_time() - node_info_table[i].last_seen) / CLOCK_SECOND);
      add("<td>%s</td>", node_info_table[i].has_route ? "OK" : "NR");
      add("</tr>");
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }
  add("</tbody></table><br />");

  add("<br /><h2>Actions</h2>");
  add("<form action=\"reset-stats-all\" method=\"get\">");
  add("<input type=\"submit\" value=\"Reset all statistics\"/></form><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  PSOCK_END(&s->sout);
}

static
PT_THREAD(generate_sensors_tree(struct httpd_state *s))
{
  static int i;
  PSOCK_BEGIN(&s->sout);
  add
    ("<center>"
     "<img src=\"http://chart.googleapis.com/chart?cht=gv&chls=1&chl=digraph{");
#if CETIC_NODE_CONFIG
  node_config_t *  my_config = node_config_find(&uip_lladdr);
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
        node_config_t * node_config = node_config_find_from_ip(&node_info_table[i].ipaddr);
        node_config_t * parent_node_config = node_config_find_from_ip(&node_info_table[i].ip_parent);
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

  PSOCK_END(&s->sout);
}

static
PT_THREAD(generate_sensors_prr(struct httpd_state *s))
{
  static int i;

  PSOCK_BEGIN(&s->sout);
  SEND_STRING(&s->sout, graph_top);
  add("['Sensor', 'IP', 'PRR'],");
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(node_info_table[i].isused) {
      float prr = 100.0 * (node_info_table[i].messages_sent - node_info_table[i].up_messages_lost)/node_info_table[i].messages_sent;
#if CETIC_NODE_CONFIG
      if (node_config_loaded) {
        node_config_t * node_config = node_config_find_from_ip(&node_info_table[i].ipaddr);
        add("[\"%s\",", node_config_get_name(node_config));
      } else
#endif
      {
        add("[\"");
        ipaddr_add(&node_info_table[i].ipaddr);
        add("\",");
      }
      add("\"");
      ipaddr_add(&node_info_table[i].ipaddr);
      add("\",%.1f],", prr);
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }
  add("]);var options={vAxis:{minValue: 0,maxValue: 100},legend:{position: \"none\"}};");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  SEND_STRING(&s->sout,graph_bottom);
  PSOCK_END(&s->sout);
}

static
PT_THREAD(generate_sensors_parent_switch(struct httpd_state *s))
{
  static int i;

  PSOCK_BEGIN(&s->sout);
  SEND_STRING(&s->sout, graph_top);
  add("['Sensor', 'IP', 'Parent switch'],");
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(node_info_table[i].isused) {
#if CETIC_NODE_CONFIG
      if (node_config_loaded) {
        node_config_t * node_config = node_config_find_from_ip(&node_info_table[i].ipaddr);
        add("[\"%s\",", node_config_get_name(node_config));
      } else
#endif
      {
        add("[\"");
        ipaddr_add(&node_info_table[i].ipaddr);
        add("\",");
      }
      add("\"");
      ipaddr_add(&node_info_table[i].ipaddr);
      add("\",%d],", node_info_table[i].parent_switch);
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }
  add("]);var options={vAxis:{minValue: 0},legend:{position: \"none\"}};");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  SEND_STRING(&s->sout,graph_bottom);
  PSOCK_END(&s->sout);
}

static
PT_THREAD(generate_sensors_hop_count(struct httpd_state *s))
{
  static int i;

  PSOCK_BEGIN(&s->sout);
  SEND_STRING(&s->sout, graph_top);
  add("['Sensor', 'IP', 'Hop count'],");
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(node_info_table[i].isused) {
#if CETIC_NODE_CONFIG
      if (node_config_loaded) {
        node_config_t * node_config = node_config_find_from_ip(&node_info_table[i].ipaddr);
        add("[\"%s\",", node_config_get_name(node_config));
      } else
#endif
      {
        add("[\"");
        ipaddr_add(&node_info_table[i].ipaddr);
        add("\",");
      }
      add("\"");
      ipaddr_add(&node_info_table[i].ipaddr);
      add("\",%d],", node_info_table[i].hop_count);
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }
  add("]);var options={vAxis:{minValue: 0},legend:{position: \"none\"}};");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  SEND_STRING(&s->sout,graph_bottom);
  PSOCK_END(&s->sout);
}

static httpd_cgi_call_t *
webserver_sensors_reset_stats_all(struct httpd_state *s)
{
  node_info_reset_statistics_all();
  return &webserver_result_page;
}

httpd_cgi_call_t * sensors_group[];
HTTPD_CGI_CALL_GROUP(webserver_sensors, "sensors.html", "Sensors", generate_sensors, 0, sensors_group);
HTTPD_CGI_CALL_GROUP(webserver_sensors_tree, "sensors_tree.html", "Node tree", generate_sensors_tree, WEBSERVER_NOMENU, sensors_group);
HTTPD_CGI_CALL_GROUP(webserver_sensors_prr, "sensors_prr.html", "PRR", generate_sensors_prr, WEBSERVER_NOMENU, sensors_group);
HTTPD_CGI_CALL_GROUP(webserver_sensors_ps, "sensors_ps.html", "Parent switch", generate_sensors_parent_switch, WEBSERVER_NOMENU, sensors_group);
HTTPD_CGI_CALL_GROUP(webserver_sensors_hc, "sensors_hc.html", "Hop count", generate_sensors_hop_count, WEBSERVER_NOMENU, sensors_group);
HTTPD_CGI_CMD(webserver_sensors_reset_stats_all_cmd, "reset-stats-all", webserver_sensors_reset_stats_all, 0);
httpd_cgi_call_t * sensors_group[] = {&webserver_sensors, &webserver_sensors_tree, &webserver_sensors_prr, &webserver_sensors_ps, &webserver_sensors_hc, NULL};
