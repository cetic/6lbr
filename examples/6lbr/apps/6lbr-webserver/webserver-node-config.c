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
#if CETIC_6LBR_IP64
#include "ip64.h"
#include "net/ip/ip64-addr.h"
#endif

#include "cetic-6lbr.h"
#include "log-6lbr.h"

#include "node-config.h"

#define UIP_IP_BUF ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

static
PT_THREAD(generate_sensors_config(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  add("<br /><h2>Sensors configuration</h2>");
  add
    ("<table>"
     "<theader><tr class=\"row_first\"><td>Node</td>");
#if CETIC_NODE_CONFIG_HAS_NAME
  add("<td>Name</td>");
#endif
  if((nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_SPECIAL_PORTS) != 0) {
    add("<td>CoAP</td><td>HTTP</td>");
  }
  add("</tr></theader><tbody>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  if(node_config_loaded) {
    static node_config_t *  node_config;
    for(node_config = node_config_list_head(); node_config != NULL; node_config = list_item_next(node_config)) {
      add("<tr><td>");
      lladdr_add(&node_config->mac_address);
      add("</td>");
#if CETIC_NODE_CONFIG_HAS_NAME
      add("<td>%s</td>", node_config_get_name(node_config));
#endif
      if((nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_SPECIAL_PORTS) != 0) {
        add("<td>%d</td><td>%d</td>", node_config->coap_port, node_config->http_port);
      }
      add("</tr>");
      SEND_STRING(&s->sout, buf);
      reset_buf();
    }
  }
  add("</tbody></table><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  PSOCK_END(&s->sout);
}

HTTPD_CGI_CALL(webserver_sensors_config, "sensors_config.html", "Sensors", generate_sensors_config, 0);
