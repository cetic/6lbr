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
#include "node-info-export.h"
#include "log-6lbr.h"
#include <stdlib.h>

#if CETIC_NODE_CONFIG
#include "node-config.h"
#endif

HTTPD_CGI_CALL_NAME(webserver_node_info_export);

static
PT_THREAD(generate_node_info_export(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  add("<h2>Status</h2>");
  add("Status : %s<br />", node_info_export_enable ? "Enabled" : "Disabled");
  add("Mode : %s<br />", node_info_export_global ? "Global" : "Per node");
  add("Interval : %d s<br />", node_info_export_interval);
  if(node_info_export_global) {
    add("File : %s<br />", node_info_export_file_name);
  } else {
    add("Path : %s<br />", node_info_export_path);
  }
  add("<form action=\"node-info-export-toggle\" method=\"get\">");
  add("<br /><input type=\"submit\" value=\"%s\"/></form><br />", node_info_export_enable ? "Disable" : "Enable");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<br /><h2>Configuration</h2>");
  add("<form action=\"node-info-export-config\" method=\"get\">");
  add("Interval: <input type=\"text\" name=\"interval\" value=\"%d\" /><br />", node_info_export_interval);
  add("File: <input type=\"text\" name=\"file\" value=\"%s\" /><br />", node_info_export_file_name);
  add("Path: <input type=\"text\" name=\"path\" value=\"%s\" /><br />", node_info_export_path);
  add("<br /><input type=\"submit\" value=\"Config\"/></form><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PSOCK_END(&s->sout);
}

static httpd_cgi_call_t *
webserver_node_info_export_toggle(struct httpd_state *s)
{
  node_info_export_set_enable(!node_info_export_enable);
  return &webserver_node_info_export;
}

static httpd_cgi_call_t *
webserver_node_info_export_config(struct httpd_state *s)
{
  webserver_result_title = "Node Info Export";
  webserver_result_text = "Configuration updated";
  const char *ptr = s->query;
  char *next;

  while(ptr && *ptr) {
    const char *param = ptr;

    next = index(ptr, '=');
    if(!next)
      break;
    *next = 0;
    ptr = next + 1;
    const char *value = ptr;

    next = index(ptr, '&');
    if(next) {
      *next = 0;
      ptr = next + 1;
    } else {
      ptr += strlen(ptr);
    }

    LOG6LBR_DEBUG("Got param: '%s' = '%s'\n", param, value);
    if (0) {
    } else if(strcmp(param, "interval") == 0) {
      node_info_export_set_interval(atoi(value));
    } else if(strcmp(param, "file") == 0) {
      node_info_export_file_name = strdup(value);
    } else {
      LOG6LBR_INFO("Invalid param: '%s'", param);
    }
  }
  return &webserver_result_page;
}

HTTPD_CGI_CALL(webserver_node_info_export, "node_info_export.html", "Export", generate_node_info_export, 0);
HTTPD_CGI_CMD(webserver_node_info_export_toggle_cmd, "node-info-export-toggle", webserver_node_info_export_toggle, 0);
HTTPD_CGI_CMD(webserver_node_info_export_config_cmd, "node-info-export-config", webserver_node_info_export_config, 0);
